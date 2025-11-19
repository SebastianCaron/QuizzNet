import client.src.network.discover_udp_list as discover_udp_list
from client.src.network.connection_tcp_server import TCPClient
import sys

continue_searching = True

# C'est fait sans interface graphique pour le moment juste en
# vieux truc texte 


while continue_searching:
    discovered_servers = discover_udp_list.serverlist_discover_UDP_server()
    if not discovered_servers:
        choice = input("Do you want to search again ? y/n > ")
        if choice.lower() != 'y':
            sys.exit(1)
    else :
        for i, (name, ip, port) in enumerate(discovered_servers):
            print(f"[{i}] : {name} : {ip}:{port}")

        choice = int(input("Chose a server (please give the server number) > "))
        if choice >= len(discovered_servers) or choice <= 0 :
            print ("Invalid number - searching again...")
        else :
            name, ip, port = discovered_servers[choice]
            continue_searching = False

print("Trying to connect to {name} : {ip}:{port}...")

tcp_client = TCPClient(ip, port)
if tcp_client.connect():
    while True:
        #Crée mais je pense que cette boucle va dégager à voir comment gérer
        #Je pense sera géré dans la boucle d'écoute de la classe
        msg_tmp = input("> ")
        if msg_tmp.lower() in ("quit", "exit", "q"):
            tcp_client.disconnect()
            break
        tcp_client.send(msg_tmp)
else:
    print("Connection failded")
