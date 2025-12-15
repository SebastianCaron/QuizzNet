import socket
import threading
from src.endpoints.routes import message_route

class TCPClient:
    """
    Handles the TCP network connection with the game server.
    
    It runs a background thread to listen for incoming messages without blocking
    the main GUI thread. When a message is received, it triggers a callback
    to update the Tkinter interface safely.
    """

    def __init__(self, ip: str, port: int, app = None, buffer_size: int = 1024, timeout: float = 5.0):
        """
        Initializes the TCP client settings.

        :param ip: Server IP address.
        :param port: Server TCP port.
        :param app: Reference to the main Tkinter application (for callbacks).
        :param buffer_size: Size of the network receive buffer.
        :param timeout: Connection timeout in seconds.
        """
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

    def connect(self) -> bool:
        """
        Attempts to establish a TCP connection with the server.
        If successful, it starts the background listening thread.

        :return: True if connected successfully, False otherwise.
        """
        try:
            self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.sock.settimeout(self.timeout)

            self.sock.connect((self.ip, self.port))
            self.is_connected = True
            self.running = True

            # Start the listener in a separate daemon thread so it closes when the app closes
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

    def _listen_server(self):
        """
        Background loop that continuously listens for data from the server.
        
        It handles:
        1. Receiving raw bytes.
        2. Buffering data until a complete message is found (delimited by '}\\n\\n').
        3. Decoding the message.
        4. Sending the message to the main GUI thread via self.app.after().
        """
        buffer = b""
        while self.running and self.is_connected:
            try:
                # Receive raw bytes from the socket
                message = self.sock.recv(self.buffer_size)
                if not message:
                   print("Server closed the connection")
                   self.disconnect()
                   break
                
                # Append new data to the buffer
                buffer += message
                
                # Process the buffer to extract complete messages based on the protocol delimiter
                while b"\n}" in buffer:
                    # Look for the specific end-of-message sequence used in this protocol
                    idx = buffer.find(b"}\n\n") + 3
                    
                    # Extract the complete message bytes
                    msg_bytes = buffer[:idx]
                    
                    # Keep the rest of the data in the buffer for the next loop
                    buffer = buffer[idx:]
                    
                    try:
                        message = msg_bytes.decode("utf-8").strip()
                        
                        if self.app:
                            # Thread-safe call to Tkinter: schedule the handler in the main loop
                            self.app.after(0, self.app.handle_server_message, message)
                    
                    except Exception as e:
                        print(f"Error decoding message: {e}")
            
            except socket.timeout:
                continue # Timeout is normal in non-blocking loops, just continue
            except Exception as e:
                print(f"Error receiving message: {e}")
                self.disconnect()
                break

    def send(self, message: str):
        """
        Encodes and sends a string message to the server.
        
        :param message: The string payload to send.
        """
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

    def disconnect(self):
        """
        Cleanly closes the socket and stops the listening thread.
        """
        if self.sock:
            try:
                self.running = False
                self.sock.close()
                print("Disconnected from server.")
            except Exception as e:
                print(f"Error when closing socket: {e}")
        self.is_connected = False