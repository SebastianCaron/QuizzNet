from src.network.connection_tcp_server import TCPClient
from src.session.session_infos import info_session
from gui.main_interface import MainInterfaceClient

def main():
    app = MainInterfaceClient(info_session)
    #tcp_client.set_ui_callback(lambda msg: app.after(0, app.handle_server_message, msg))
    app.mainloop()

if __name__ == "__main__":
    main()
