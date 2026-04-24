#include <memory>
#include "rclcpp/rclcpp.hpp"
#include "example_interfaces/srv/add_two_ints.hpp"

class SrvCliNode : public rclcpp::Node
{
public:
  SrvCliNode() : Node("srv_cli_node")
  {
    server_ = this->create_service<example_interfaces::srv::AddTwoInts>(
      "my_service", std::bind(&SrvCliNode::handle_service, this, std::placeholders::_1, std::placeholders::_2));

    client_ = this->create_client<example_interfaces::srv::AddTwoInts>("my_service");
  }

  void send_request(int a, int b)
  {
    if (!client_->wait_for_service(std::chrono::seconds(1))) {
      RCLCPP_WARN(this->get_logger(), "Waiting for service to appear...");
      return;
    }

    auto request = std::make_shared<example_interfaces::srv::AddTwoInts::Request>();
    request->a = a;
    request->b = b;

    client_->async_send_request(request, std::bind(&SrvCliNode::response_callback, this, std::placeholders::_1));
  }

private:
  void handle_service(const std::shared_ptr<example_interfaces::srv::AddTwoInts::Request> request,
                      std::shared_ptr<example_interfaces::srv::AddTwoInts::Response> response)
  {
    response->sum = request->a + request->b;
    RCLCPP_INFO(this->get_logger(), "Server responded with sum: %ld", response->sum);
  }

  void response_callback(rclcpp::Client<example_interfaces::srv::AddTwoInts>::SharedFuture future)
  {
    auto response = future.get();
    RCLCPP_INFO(this->get_logger(), "Client received result: %ld", response->sum);
  }

  rclcpp::Service<example_interfaces::srv::AddTwoInts>::SharedPtr server_;
  rclcpp::Client<example_interfaces::srv::AddTwoInts>::SharedPtr client_;
};

int main(int argc, char **argv)
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<SrvCliNode>());
  rclcpp::shutdown();
  return 0;
}