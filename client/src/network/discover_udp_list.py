import socket
import time

# Protocol constants used to identify legitimate QuizNet servers
DISCOVERY_MSG = "looking for quiznet servers"
EXPECTED_RESPONSE_PREFIX = "hello i'm a quiznet server:"
UDP_TIMEOUT = 5.0
BROADCAST_PORT = 5555

def serverlist_discover_UDP_server(broadcast_port: int = BROADCAST_PORT,
                        timeout: float = UDP_TIMEOUT):
    """
    Broadcasts a discovery message to find active QuizNet servers on the network.
    Listens for responses and parses them to build a list of available servers.

    Returns:
        list: A list of tuples containing (server_name, ip_address, tcp_port).
    """

    # Create a UDP socket
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    
    # Enable broadcast mode to allow sending to the network address
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
    
    # Set a timeout to prevent the socket from blocking indefinitely
    sock.settimeout(timeout)

    # Define target address (using localhost for local testing)
    broadcast_address = ('127.0.0.1', broadcast_port)
    try:
        # Send the specific discovery payload
        sock.sendto(DISCOVERY_MSG.encode('utf-8'), broadcast_address)
    except Exception as e:
        print(f"Error while sending broadcast message: {e}")
        sock.close()
        return []           

    responses_list = []
    start_time = time.time()

    # Loop to listen for incoming responses until the timeout expires
    while time.time() - start_time < timeout :
        try :
            # Receive data (buffer size 1024 bytes)
            response, address = sock.recvfrom(1024)
            response_str = response.decode('utf-8')

            # Validate the protocol: check if the message starts with the expected prefix
            if response_str.startswith(EXPECTED_RESPONSE_PREFIX):
                
                # Extract the payload (ServerName:Port)
                tail = response_str[len(EXPECTED_RESPONSE_PREFIX):].strip()
                if ':' in tail:
                    serv_name, serv_port_tcp_str = tail.split(':', 1)
                    serv_port_tcp = int(serv_port_tcp_str)
                    
                    # Store valid server info: (Name, IP, TCP Port)
                    responses_list.append((serv_name, address[0], serv_port_tcp))
                    print(f"Server found : {serv_name}.")
        
        except socket.timeout:
            # Stop listening when the time limit is reached
            break
        except Exception as e:
            print(f"Error reception : {e}")
            break

    sock.close()
    
    if not responses_list:
        print("No server found.")        
    return responses_list