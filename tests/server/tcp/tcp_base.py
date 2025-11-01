import socket
from typing import Tuple
from tests.server.utils.server import ServerMaker


class TCPServerTester:
    """Classe de base pour les tests TCP"""
    
    def __init__(self, server_class: ServerMaker):
        self.server = server_class
    
    def send_tcp_request(self, request: str, timeout: float = 5.0) -> Tuple[bool, str]:
        try:
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            sock.settimeout(timeout)
            
            server_address = ('localhost', int(self.server.tcp_port))
            sock.connect(server_address)
            
            sock.sendall(request.encode('utf-8'))
            
            response_data = b""
            while True:
                chunk = sock.recv(4096)
                if not chunk:
                    break
                response_data += chunk
                if len(chunk) < 4096:
                    break
            
            response_str = response_data.decode('utf-8')
            sock.close()
            return True, response_str
            
        except socket.timeout:
            return False, "Timeout - aucune réponse reçue"
        except Exception as e:
            return False, f"Erreur: {e}"

