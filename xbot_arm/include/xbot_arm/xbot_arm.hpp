#ifndef XBOT_ARM_HPP_
#define XBOT_ARM_HPP_

#include <string.h>
#include <ros/ros.h>

namespace xbot
{
class XbotArm
{
public:
  XbotArm(ros::NodeHandle& nh, std::string& name):
    nh_(nh),
    name_(name){};
  ~XbotArm(){};
  bool init()
  {

    return true;
  };
  void spin();

private:
  ros::NodeHandle nh_;
  std::string name_;

};
void XbotArm::spin()
{

}
}




#endif
