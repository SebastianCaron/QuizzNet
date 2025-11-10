import socket
import time

DISCOVERY_MSG = "looking for quiznet servers"
EXPECTED_RESPONSE_PREFIX = "hello i'm a quiznet server:"
UDP_TIMEOUT = 5.0
BROADCAST_PORT = 5555

def serverlist_discover_UDP_server(broadcast_port: int = BROADCAST_PORT,
                        timeout: float = UDP_TIMEOUT):
    """
    send the discover message, then return the server list that
    sended a response
    """

    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
    sock.settimeout(timeout)

    broadcast_address = ('<broadcast>', broadcast_port)
    try:
        sock.sendto(DISCOVERY_MSG.encode('utf-8'), broadcast_address)
    except Exception as e:
        print(f"Error while sending broadcast message: {e}")
        sock.close()
        return []           

    responses_list = []
    start_time = time.time()
    while time.time() - start_time < timeout :
        try :
            response, address = sock.recvfrom(1024)
            response_str = response.decode('utf-8')
            if response_str.startswith(EXPECTED_RESPONSE_PREFIX):
                tail = response_str[len(EXPECTED_RESPONSE_PREFIX):].strip()
                if ':' in tail:
                    serv_name, serv_port_tcp_str = tail.split(':', 1)
                    serv_port_tcp = int(serv_port_tcp_str)
                    responses_list.append((serv_name, address[0], serv_port_tcp))
                    print(f"Server found : {serv_name}.")
        except socket.timeout:
            break
        except Exception as e:
            print(f"Error reception : {e}")
            break

    sock.close()
    
    if not responses_list:
        print("No server found.")        
    return responses_list

