#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <fstream>
#include "AIUITest.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "Talker.h"
#include "speech_recognizer.h"
#include <thread>
#include <mutex>
#include <condition_variable>
#include <pthread.h>
#include <signal.h>
#include <ros/ros.h>
#include <std_msgs/String.h>
#include <std_msgs/UInt32.h>
#include <xbot_msgs/FaceResult.h>

//#include "../include/AIUITest.h"
//#include "../include/rapidjson/writer.h"
//#include "../include/rapidjson/stringbuffer.h"
//#include "../include/Talker.h"
//#include "../include/speech_recognizer.h"


using namespace std;
#define FRAME_LEN	640
#define	BUFFER_SIZE	4096

void onGoalReached(const std_msgs::String& msg);
void onGetFaceResult(const xbot_msgs::FaceResult& faceResult);
void onPlayFinished(int code,string message);
void on_result(const char *result, char is_last);
void on_speech_begin();
void on_speech_end(int reason);
void start_recording( char* session_begin_params);
void signal_handler(int s);
void* record_thread(void* session_begin_params);

const char* subscribe_topic_goal = "/office/goal_reached";
const char* subscribe_topic_face_recog ="/office/face_result";
const char* goal_name_pub_topic ="/office/goal_name";//携带的数据为拼音
const char* next_loop_pub_topic = "/office/next_loop";
string basePath = "/home/lee/catkin_ws/src/xbot2/talker";
ros::Publisher goal_name_pub;
ros::Publisher next_loop_pub;
ros::Subscriber faceRecogSubscriber;
ros::Subscriber goalReachSubscriber;
IAIUIAgent *agent ;
Talker talker;
ISpeechUtility* speechUtility;

char *g_result = NULL;
unsigned int g_buffersize = BUFFER_SIZE;
int ret;
bool isPlayingAudio;
string lastGoal;
int recordCount = 0;

mutex mutex_chat , mutex_playing_audio;
condition_variable condition_chat , condition_playing_audio;
bool isChatting;
pthread_t record_thread_id;

int main(int argc,char** argv){
    ros::init(argc,argv,"talker");
    ros::NodeHandle nodeHandle;
    goal_name_pub =  nodeHandle.advertise<std_msgs::String>(goal_name_pub_topic,1);
    next_loop_pub = nodeHandle.advertise<std_msgs::UInt32>(next_loop_pub_topic,1);

    faceRecogSubscriber = nodeHandle.subscribe(subscribe_topic_face_recog,10,onGetFaceResult);
    goalReachSubscriber = nodeHandle.subscribe(subscribe_topic_goal,10,onGoalReached);

//    nodeHandle.param("/talker/base_path",basePath, string("/home/lee/catkin_ws/src/xbot2/talker"));
    nodeHandle.param("/talker/base_path",basePath, string("/home/xbot/catkin_ws/src/xbot2/talker"));
//    ROS_ERROR("%s\n",basePath.c_str());
    ret = talker.init(basePath);
    if(ret==-1){
        cout<<"Talker init failed"<<endl;
        exit(0);
    }else{
        cout<<"Talker init success"<<endl;
    }
    isChatting = false;
    pthread_create(&record_thread_id,NULL,record_thread,(void*)session_record_begin_params);
    ros::spin();
    signal(SIGINT,signal_handler);

     return 0;
}

void onGoalReached(const std_msgs::String& msg){
    string goal_name = msg.data;
    cout<<"onGoalReached:"<<goal_name<<endl;
    if(goal_name.find("abort")!=-1){
        string file = basePath+"/assets/wav/abort.wav";
        talker.play((char*)file.c_str(),REQUEST_MOVE_ABORT,onPlayFinished);
    }else if(goal_name.find("origin")==-1){
        //表示到达了员工位置处
        talker.informWhenReachGoal(goal_name,onPlayFinished);
    }else{
        //表示回到了出发点
        isPlayingAudio = false;
        isChatting = false;
        lastGoal = goal_name;
    }

}

void onGetFaceResult(const xbot_msgs::FaceResult& faceResult){
//    cout<<"onGetFaceResult  ---- tid: "<<this_thread::get_id()<<endl;
    if(!isChatting){
        cout<<"getFaceResult:"<<faceResult.name<<endl;
        string staffName = faceResult.name;
        talker.greetByName(staffName,&onPlayFinished);
    }

}

void onPlayFinished(int code,string message){
//    cout<<"-----------------------  onPlayFinished  ---- tid: "<<this_thread::get_id()<<endl;
    //cout<<"code: "<<code<<endl;
    cout<<"OnPlayFinished -- message:"<<message<<endl;
    std_msgs::String mes;

    switch(code){
        case REQUEST_GREET_STAFF:
        {
            cout<<"REQUEST_GREET_STAFF"<<endl;
            sleep(10);
            std_msgs::UInt32 msg;
            msg.data = 255;
            next_loop_pub.publish(msg);

        }
        break;
        case REQUEST_GREET_VISITOR:
        {
            cout<<"REQUEST_GREET_VISITOR"<<endl;
            recordCount = 0;
            isPlayingAudio = false;
            condition_playing_audio.notify_all();
            isChatting = true;
            condition_chat.notify_all();
        }

        break;
        case REQUEST_CHAT:
        {
            cout<<"REQUEST_CHAT"<<endl;
            isPlayingAudio = false;
            condition_playing_audio.notify_all();
        }
        break;
        case REQUEST_CHAT_QUERY_GOAL:
        {
            isChatting = false;
            cout<<"REQUEST_CHAT_QUERY_GOAL"<<endl;
            string file = basePath+"/assets/wav/followme.wav";
            talker.play((char*)file.c_str(),REQUEST_FOLLOW_ME,onPlayFinished);
            lastGoal = message;
            mes.data = message;
            goal_name_pub.publish(mes);
            cout<<"Publish Goal: "<<message<<endl;
        }
        break;
       case REQUEST_FOLLOW_ME:
       {
            cout<<"REQUEST_FOLLOW_ME"<<endl;
       }
       break;
       case REQUEST_REACH_GOAL:
       {
            sleep(5);
            cout<<"REQUEST_REACH_GOAL"<<endl;
            string str = "origin";
            lastGoal = str;
            mes.data = str;
            isChatting = false;
            //发布消息给SLAM，以回到出发点
            goal_name_pub.publish(mes);
            cout<<"Publish Goal: "<<str<<endl;
       }
       break;
       case REQUEST_MOVE_ABORT:
       {
            cout<<"REQUEST_MOVE_ABORT"<<endl;
            sleep(5);
            mes.data = lastGoal;
            goal_name_pub.publish(mes);
            cout<<"Publish Goal: "<<lastGoal<<endl;
       }
       case REQUEST_SIMPLE_PLAY:
       {
           cout<<"REQUEST_SIMPLE_PLAY"<<endl;
       }
       break;
       default:
       break;
    }
}

void* record_thread(void* session_begin_params){
//    cout<<"start record thread  "<<this_thread::get_id()<<endl;
    string login_parameters = "appid = 5a52e95f, work_dir = "+basePath+"/assets";
    int ret = MSPLogin(NULL, NULL, login_parameters.c_str());
    if (MSP_SUCCESS != ret)	{
        cout<<"MSPLogin failed , Error code  "<<ret<<endl;
        return NULL;
    }
    while(true){
        int errcode;
        struct speech_rec iat;
        struct speech_rec_notifier recnotifier = {
                    on_result,
                    on_speech_begin,
                    on_speech_end
        };

        //对话同步锁
        //问候完访客时 -- 开启该线程
        //对话60s超时或得到目标位置点  -- 挂起该线程
        unique_lock<mutex> lock1(mutex_chat);
        condition_chat.wait(lock1,[]{return isChatting;});

        signal(SIGINT,signal_handler);
        cout<<"\n-----------Start Chatting--------"<<endl;
        cout<<"You can speak to me(record  for 10 seconds) : "<<endl;

        errcode = sr_init(&iat, (char*)session_begin_params, SR_MIC, &recnotifier);
        if (errcode) {
            cout<<"Speech recognizer init failed   code:"<<errcode<<endl;
            return NULL;
        }
        errcode = sr_start_listening(&iat);
        if (errcode) {
            cout<<"Start listening failed . code:"<<errcode<<endl;
        }

        /* record for 10 seconds */
        sleep(10);

        errcode = sr_stop_listening(&iat);
        if (errcode) {
            cout<<"Stop listening failed  code:"<<errcode<<endl;
        }
        cout<<"Recording completed"<<endl;

        //语音播放同步锁
        //即将开启下一轮录音，判断当前是否在播放语音
        //如果在播放语音，则挂起该线程，等待语音播放完
        if(isPlayingAudio){
            unique_lock<mutex> lock2(mutex_playing_audio);
            condition_playing_audio.wait(lock2,[]{return !isPlayingAudio;});
        }

        sr_uninit(&iat);
        recordCount++;
        if(recordCount==6){
            isChatting = false;
            cout<<"----------- Interacting Timeout. Stop Chatting --------"<<endl;
            std_msgs::UInt32 msg;
            msg.data = 255;
            next_loop_pub.publish(msg);
            recordCount = 0;
        }

    }

}


//语音识别的结果回调
void on_result(const char *result, char is_last){
//    cout<<"on_result       ----  tid: "<<this_thread::get_id()<<endl;
    if(result){
        size_t left = g_buffersize - 1 - strlen(g_result);
        size_t size = strlen(result);
        if (left < size) {
            g_result = (char*)realloc(g_result, g_buffersize + BUFFER_SIZE);
            if (g_result)
                g_buffersize += BUFFER_SIZE;
            else {
                printf("mem alloc failed\n");
                return;
            }
        }

        strncat(g_result, result, size);
        if(is_last){
            isPlayingAudio = true;
            recordCount = 0;
            cout<<"Speech result : "<<g_result<<endl;
            talker.chat(g_result,onPlayFinished);
        }
    }
}

//语音识别开始
void on_speech_begin(){
//    cout<<"on_speech_begin       ----  tid: "<<this_thread::get_id()<<endl;
    cout<<"Start listening........"<<endl;
    if (g_result){
        free(g_result);
    }
    g_result = (char*)malloc(BUFFER_SIZE);
    g_buffersize = BUFFER_SIZE;
    memset(g_result, 0, g_buffersize);

}

//语音识别结束
void on_speech_end(int reason){
//    cout<<"on_speech_end     ----  tid: "<<this_thread::get_id()<<endl;
    if (reason == END_REASON_VAD_DETECT){
        cout<<"Speaking done.  ";
    }

    if(reason==10114){
        cout<<"Request Timeout"<<endl;
    }

}

void signal_handler(int s){
    ros::shutdown();
    MSPLogout();
    exit(0);
}


