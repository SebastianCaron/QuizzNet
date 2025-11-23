import socket

UDP_PORT = 5555
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)

sock.bind(("", UDP_PORT))  # écoute toutes les interfaces, y compris broadcast

print(f"Listening on UDP port {UDP_PORT}...")

while True:
    data, addr = sock.recvfrom(1024)
    msg = data.decode()
    print(f"Received: {msg} from {addr}")
    if msg == "looking for quiznet servers":
        response = b"hello i'm a quiznet server:test:1234"
        sock.sendto(response, addr)
        print(f"Responded to {addr}")

