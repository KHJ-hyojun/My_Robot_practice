#include <chrono>
#include <memory>
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp" // 사용할 메시지 타입 헤더

using namespace std::chrono_literals; // 1000ms 같은 시간 단위를 쓰기 위함

// Node를 상속받아 하나의 독립적인 모듈(클래스)로 설계 (실무 표준)
class PubSubNode : public rclcpp::Node
{
private:
  // 타이머가 울릴 때마다 실행되는 함수 (데이터 발행)
  void timer_callback()
  {
    auto msg = std_msgs::msg::String();
    msg.data = "Data from C++ Publisher";
    publisher_->publish(msg); // 실제 네트워크로 전송
  }

  // 토픽에 새로운 데이터가 들어올 때마다 실행되는 함수 (데이터 수신)
  void topic_callback(const std_msgs::msg::String::SharedPtr msg) const
  {
    RCLCPP_INFO(this->get_logger(), "Received: '%s'", msg->data.c_str());
  }

  // 포인터 변수 선언 (메모리 누수 방지를 위해 SharedPtr 사용)
  rclcpp::Publisher<std_msgs::msg::String>::SharedPtr publisher_;
  rclcpp::Subscription<std_msgs::msg::String>::SharedPtr subscription_;
  rclcpp::TimerBase::SharedPtr timer_;
public:
  // 생성자: 노드 이름 지정 및 통신 채널 초기화
  PubSubNode() : Node("pub_sub_node")
  {
    // [Publisher 생성] 타입, 토픽명, 큐(버퍼) 사이즈
    publisher_ = this->create_publisher<std_msgs::msg::String>("my_topic", 10);
    
    // [Timer 생성] 1000ms(1초)마다 timer_callback 함수를 실행
    // std::bind를 통해 클래스 내부의 멤버 함수를 콜백으로 연결
    timer_ = this->create_wall_timer(
      1000ms, std::bind(&PubSubNode::timer_callback, this));

    // [Subscriber 생성] 타입, 토픽명, 큐 사이즈, 실행될 콜백 함수
    // std::placeholders::_1 은 들어오는 메시지 데이터를 함수의 첫 번째 인자로 넘기겠다는 뜻
    subscription_ = this->create_subscription<std_msgs::msg::String>(
      "my_topic", 10, std::bind(&PubSubNode::topic_callback, this, std::placeholders::_1));
  }
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv); // 통신망 초기화
  rclcpp::spin(std::make_shared<PubSubNode>()); // 노드 실행 및 대기 (무한루프)
  rclcpp::shutdown(); // 안전하게 종료
  return 0;
}