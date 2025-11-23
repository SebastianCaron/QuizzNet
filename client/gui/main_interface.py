import tkinter as tk
from gui.windows.searching_server import SearchingServer
from gui.windows.select_server import SelectServer
from src.endpoints.routes import message_route

class MainInterfaceClient(tk.Tk):
    def __init__(self, tcp_client, session_info):
        super().__init__()

        self.title("QuizNet")
        self.geometry("600x400")
        self.resizable(False, False)

        self.tcp_client = tcp_client
        self.session_info = session_info

        self.frames = {}

        #Init pages
        for F in (SearchingServer, SelectServer):
            frame = F(self)
            self.frames[F] = frame
            frame.place(relwidth=1, relheight=1)

        self.show_page(SearchingServer)

    # Show a specific page
    def show_page(self, page_class):
        frame = self.frames[page_class]
        frame.tkraise()
        if hasattr(frame, "on_show"):
            frame.on_show()

    def handle_server_message(self, message):
        print("Message serveur reçu dans Tkinter :", message)
        # Ici on appelle route(message)
        message_route(message)
