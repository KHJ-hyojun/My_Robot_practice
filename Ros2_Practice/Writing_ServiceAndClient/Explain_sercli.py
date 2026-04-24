import rclpy
from rclpy.node import Node
from example_interfaces.srv import AddTwoInts # 서비스 인터페이스 import

# 클래스 하나에 서버와 클라이언트 구조를 모두 담아둔 템플릿
class SrvCliNode(Node):
    def __init__(self):
        super().__init__('srv_cli_node')
        
        # [Service Server 생성] 타입, 서비스명, 요청이 오면 실행될 콜백 함수
        self.srv = self.create_service(AddTwoInts, 'my_service', self.service_callback)
        
        # [Service Client 생성] 타입, 서비스명
        self.cli = self.create_client(AddTwoInts, 'my_service')

    # 클라이언트가 서버로 데이터를 전송하는 커스텀 함수
    def send_request(self, a, b):
        # 대상 서버가 켜질 때까지 1초 간격으로 무한 대기 (방어 로직)
        while not self.cli.wait_for_service(timeout_sec=1.0):
            self.get_logger().info('Service not available, waiting...')
        
        # 보낼 Request 메시지 세팅
        request = AddTwoInts.Request()
        request.a = a
        request.b = b
        
        # 비동기 통신 (노드를 멈추지 않고 백그라운드에서 요청 전송)
        future = self.cli.call_async(request)
        
        # 미래(future)에 서버로부터 응답이 돌아오면 response_callback 함수를 실행하라고 예약
        future.add_done_callback(self.response_callback)

    # [서버 로직] 누군가에게서 요청(request)이 들어왔을 때 실행
    def service_callback(self, request, response):
        response.sum = request.a + request.b # 연산 후 response에 결과 담기
        self.get_logger().info(f'Server logic executed. Sum: {response.sum}')
        return response # 반드시 response 객체를 return 해야 함

    # [클라이언트 로직] 서버로부터 최종 결과가 돌아왔을 때 실행
    def response_callback(self, future):
        try:
            response = future.result() # 에러 없이 돌아왔는지 확인하고 결과 추출
            self.get_logger().info(f'Client received response: {response.sum}')
        except Exception as e:
            self.get_logger().error(f'Service call failed: {e}')

def main(args=None):
    rclpy.init(args=args)
    node = SrvCliNode()
    
    # 필요시 특정 조건(타이머 등)에서 send_request를 호출하여 클라이언트 로직 가동
    # node.send_request(3, 5) 
    
    rclpy.spin(node) # 프로그램이 죽지 않고 콜백들을 처리하도록 유지
    node.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()