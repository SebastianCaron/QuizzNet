import socket
import threading
from src.endpoints.routes import message_route

class TCPClient:

    def __init__(self, ip: str, port: int, app = None, buffer_size: int = 1024, timeout: float = 5.0):
        self.ip = ip
        self.port = port
        self.buffer_size = buffer_size
        self.timeout = timeout
        self.sock = None
        self.is_connected = False
        self.listener_thread = None
        self.running = False
        self.gui_callback = None
        self.app = app

    # Connexion
    def connect(self) -> bool:
        """Try to establish a tcp connection with the server"""
        try:
            self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.sock.settimeout(self.timeout)

            self.sock.connect((self.ip, self.port))
            self.is_connected = True
            self.running = True

            self.listener_thread = threading.Thread(target=self._listen_server, daemon=True)
            self.listener_thread.start()

            return True

        except (ConnectionRefusedError, TimeoutError, socket.timeout):
            print(f"Connection failed : {self.ip}:{self.port}")
            self.is_connected = False
            return False

        except Exception as e:
            print(f"Connection error: {e}")
            self.is_connected = False
            return False

    # Listening
    def _listen_server(self):
        buffer = b""
        while self.running and self.is_connected:
            try:
                message = self.sock.recv(self.buffer_size)
                if not message:
                   print("Server closed the connection")
                   self.disconnect()
                   break
                buffer+=message
                while b"\n}" in buffer:
                    idx = buffer.find(b"\n}") + 2
                    msg_bytes = buffer[:idx]
                    buffer = buffer[idx:]
                    try:
                        message = msg_bytes.decode("utf-8").strip()
                        print("Server sended:", message)
                        if self.app:
                            self.app.handle_server_message(message)
                    except Exception as e:
                        print(f"Error decoding message: {e}")
            except socket.timeout:
                continue
            except Exception as e:
                print(f"Error receiving message: {e}")
                self.disconnect()
                break

    # Sendind messages
    def send(self, message: str):
        """Send a message to the server"""
        if not self.is_connected or self.sock is None:
            print("Impossible to send a message: Not connected to any server.")
            if (self.sock is None):
                print("NONE SOCK")
            return
        try:
            self.sock.sendall(message.encode("utf-8"))
        except Exception as e:
            print(f"Send failed: {e}")
            self.disconnect()

    # Disconnect
    def disconnect(self):
        """Disconnect to the server"""
        if self.sock:
            try:
                self.running = False
                self.sock.close()
                print("Disconnected from server.")
            except Exception as e:
                print(f"Error when closing socket: {e}")
        self.is_connected = False

