#include <chrono>
#include <memory>
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"

using namespace std::chrono_literals;

class PubSubNode : public rclcpp::Node
{
public:
  PubSubNode() : Node("pub_sub_node")
  {
    publisher_ = this->create_publisher<std_msgs::msg::String>("my_topic", 10);
    
    timer_ = this->create_wall_timer(
      1000ms, std::bind(&PubSubNode::timer_callback, this));

    subscription_ = this->create_subscription<std_msgs::msg::String>(
      "my_topic", 10, std::bind(&PubSubNode::topic_callback, this, std::placeholders::_1));
  }

private:
  void timer_callback()
  {
    auto msg = std_msgs::msg::String();
    msg.data = "Data from C++ Publisher";
    publisher_->publish(msg);
  }

  void topic_callback(const std_msgs::msg::String::SharedPtr msg) const
  {
    RCLCPP_INFO(this->get_logger(), "Received: '%s'", msg->data.c_str());
  }

  rclcpp::Publisher<std_msgs::msg::String>::SharedPtr publisher_;
  rclcpp::Subscription<std_msgs::msg::String>::SharedPtr subscription_;
  rclcpp::TimerBase::SharedPtr timer_;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<PubSubNode>());
  rclcpp::shutdown();
  return 0;
}