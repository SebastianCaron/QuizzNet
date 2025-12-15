from src.network.connection_tcp_server import TCPClient
from src.session.session_infos import info_session
from gui.main_interface import MainInterfaceClient

def main():
    app = MainInterfaceClient(info_session)
    app.mainloop()

if __name__ == "__main__":
    main()
