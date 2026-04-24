#include <memory>
#include "rclcpp/rclcpp.hpp"
#include "example_interfaces/srv/add_two_ints.hpp" // 서비스 타입 헤더

// 실무에서는 서버와 클라이언트가 분리되어 있거나, 하나의 노드에서 둘 다 수행할 수 있습니다.
// 이 템플릿은 확장을 위해 두 기능을 하나의 클래스에 모두 담은 구조입니다.
class SrvCliNode : public rclcpp::Node
{
public:
  SrvCliNode() : Node("srv_cli_node")
  {
    // [Service Server 생성] 
    // 누군가 "my_service" 채널로 요청을 보내면 handle_service 함수를 실행
    server_ = this->create_service<example_interfaces::srv::AddTwoInts>(
      "my_service", std::bind(&SrvCliNode::handle_service, this, std::placeholders::_1, std::placeholders::_2));

    // [Service Client 생성]
    // "my_service" 채널에 요청을 보낼 준비
    client_ = this->create_client<example_interfaces::srv::AddTwoInts>("my_service");
  }

  // 클라이언트로서 특정 시점에 서버로 요청을 날리는 커스텀 함수
  void send_request(int a, int b)
  {
    // 서버 노드가 켜져 있는지 확인 (1초 대기)
    if (!client_->wait_for_service(std::chrono::seconds(1))) {
      RCLCPP_WARN(this->get_logger(), "Waiting for service to appear...");
      return;
    }

    // 보낼 데이터 패키징 (Request 상자 생성 및 채우기)
    auto request = std::make_shared<example_interfaces::srv::AddTwoInts::Request>();
    request->a = a;
    request->b = b;

    // 비동기(Async) 요청 전송: 
    // 서버가 답을 줄 때까지 노드가 멈추지 않으며, 답이 오면 response_callback을 실행함
    client_->async_send_request(request, std::bind(&SrvCliNode::response_callback, this, std::placeholders::_1));
  }

private:
  // [서버 로직] 클라이언트의 요청이 들어왔을 때 수행할 작업
  void handle_service(const std::shared_ptr<example_interfaces::srv::AddTwoInts::Request> request,
                      std::shared_ptr<example_interfaces::srv::AddTwoInts::Response> response)
  {
    response->sum = request->a + request->b; // 계산 후 response 상자에 결과 담기
    RCLCPP_INFO(this->get_logger(), "Server responded with sum: %ld", response->sum);
  }

  // [클라이언트 로직] 서버로부터 최종 결과(응답)가 도착했을 때 수행할 작업
  void response_callback(rclcpp::Client<example_interfaces::srv::AddTwoInts>::SharedFuture future)
  {
    auto response = future.get(); // 도착한 응답 데이터 꺼내기
    RCLCPP_INFO(this->get_logger(), "Client received result: %ld", response->sum);
  }

  rclcpp::Service<example_interfaces::srv::AddTwoInts>::SharedPtr server_;
  rclcpp::Client<example_interfaces::srv::AddTwoInts>::SharedPtr client_;
};

int main(int argc, char **argv)
{
  rclcpp::init(argc, argv);
  // (만약 시작하자마자 요청을 보내고 싶다면 아래처럼 노드를 변수로 빼서 send_request 호출)
  // auto node = std::make_shared<SrvCliNode>();
  // node->send_request(3, 5); 
  // rclcpp::spin(node);
  rclcpp::spin(std::make_shared<SrvCliNode>());
  rclcpp::shutdown();
  return 0;
}