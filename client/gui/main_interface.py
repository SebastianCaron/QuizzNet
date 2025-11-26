import tkinter as tk
from gui.windows.searching_server import SearchingServer
from gui.windows.select_server import SelectServer
from gui.windows.login_page import LoginPage
from gui.windows.liste_session import SessionListPage

from src.endpoints.routes import message_route

class MainInterfaceClient(tk.Tk):
    def __init__(self, session_info):
        super().__init__()

        self.title("QuizNet")
        self.geometry("600x400")
        self.resizable(False, False)

        self.tcp_client = None
        self.session_info = session_info

        self.frames = {}

        #Init pages
        for F in (SearchingServer, SelectServer, LoginPage, SessionListPage):
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
        try:
            message_route(message, self)
        except Exception as e:
            print("Erreur route:", e)


    def set_tcp_client(self, tcp_client):
        self.tcp_client = tcp_client
        self.tcp_client.set_gui_callback(lambda msg: self.after(0, self.handle_server_message, msg))
