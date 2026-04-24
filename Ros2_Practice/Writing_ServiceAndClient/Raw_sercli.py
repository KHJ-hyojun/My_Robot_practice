import rclpy
from rclpy.node import Node
from example_interfaces.srv import AddTwoInts

class SrvCliNode(Node):
    def __init__(self):
        super().__init__('srv_cli_node')
        
        self.srv = self.create_service(AddTwoInts, 'my_service', self.service_callback)
        self.cli = self.create_client(AddTwoInts, 'my_service')

    def send_request(self, a, b):
        while not self.cli.wait_for_service(timeout_sec=1.0):
            self.get_logger().info('Service not available, waiting...')
        
        request = AddTwoInts.Request()
        request.a = a
        request.b = b
        
        future = self.cli.call_async(request)
        future.add_done_callback(self.response_callback)

    def service_callback(self, request, response):
        response.sum = request.a + request.b
        self.get_logger().info(f'Server logic executed. Sum: {response.sum}')
        return response

    def response_callback(self, future):
        try:
            response = future.result()
            self.get_logger().info(f'Client received response: {response.sum}')
        except Exception as e:
            self.get_logger().error(f'Service call failed: {e}')

def main(args=None):
    rclpy.init(args=args)
    node = SrvCliNode()
    
    # node.send_request(3, 5) # Uncomment to trigger a request
    
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()