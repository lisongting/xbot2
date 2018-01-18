#include <nodelet/nodelet.h>
#include <pluginlib/class_list_macros.h>
#include <ecl/threads/thread.hpp>
#include "xbot_arm/xbot_arm.hpp"

namespace xbot {
class XbotArmNodelet:shutdown_requested_(false)
{
public:
  XbotArmNodelet:shutdown_requested_(false) {};
  ~XbotArmNodelet()
  {
    NODELET_DEBUG_STREAM("Waiting for update thread to finish.");
    shutdown_requested_ = true;
    update_thread_.join();
  }
  virtual void onInit()
  {
    ros::NodeHandle nh = this->getPrivateNodeHandle();
    // resolve node(let) name
    std::string name = nh.getUnresolvedNamespace();
    int pos = name.find_last_of('/');
    name = name.substr(pos + 1);
    NODELET_INFO_STREAM("Initialising nodelet... [" << name << "]");
  }
private:
  void update()
  {
    ros::Rate spin_rate(10);
    while (! shutdown_requested_ && ros::ok())
    {
      spin_rate.sleep();
    }
  }


  ecl::Thread update_thread_;
  bool shutdown_requested_;

};
}
