import rclpy
from rclpy.node import Node
from std_msgs.msg import String # 사용할 메시지 타입 import

# Node 클래스를 상속받아 객체 지향적으로 구성
class PubSubNode(Node):
    def __init__(self):
        # 1. 노드 이름 설정 (터미널에 표시될 이름)
        super().__init__('pub_sub_node')
        
        # 2. Publisher 생성 (타입, 토픽명, 큐 사이즈)
        self.publisher_ = self.create_publisher(String, 'my_topic', 10)
        
        # 3. Timer 생성 (1초 주기, 실행할 콜백 함수)
        self.timer = self.create_timer(1.0, self.timer_callback)
        
        # 4. Subscriber 생성 (타입, 토픽명, 실행할 콜백 함수, 큐 사이즈)
        self.subscription = self.create_subscription(
            String, 'my_topic', self.listener_callback, 10)
        self.subscription  # 가비지 컬렉터에 의해 삭제되는 것을 방지

    # 퍼블리셔가 데이터를 전송하는 로직
    def timer_callback(self):
        msg = String()
        msg.data = 'Data from Python Publisher'
        self.publisher_.publish(msg)

    # 서브스크라이버가 데이터를 받았을 때 처리하는 로직
    def listener_callback(self, msg):
        self.get_logger().info(f'Received: "{msg.data}"')

def main(args=None):
    rclpy.init(args=args) # ROS 2 초기화
    node = PubSubNode()   # 클래스 인스턴스 생성
    rclpy.spin(node)      # 콜백이 실행될 수 있도록 노드를 멈추지 않고 대기시킴
    node.destroy_node()   # 종료 시 메모리 정리
    rclpy.shutdown()      # ROS 2 종료

if __name__ == '__main__':
    main()