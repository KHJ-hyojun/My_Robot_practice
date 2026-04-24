import rclpy
from rclpy.node import Node
from std_msgs.msg import String

class PubSubNode(Node):
    def __init__(self):
        super().__init__('pub_sub_node')
        
        self.publisher_ = self.create_publisher(String, 'my_topic', 10)
        self.timer = self.create_timer(1.0, self.timer_callback)
        
        self.subscription = self.create_subscription(
            String, 'my_topic', self.listener_callback, 10)
        self.subscription  

    def timer_callback(self):
        msg = String()
        msg.data = 'Data from Python Publisher'
        self.publisher_.publish(msg)

    def listener_callback(self, msg):
        self.get_logger().info(f'Received: "{msg.data}"')

def main(args=None):
    rclpy.init(args=args)
    node = PubSubNode()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()