import tkinter as tk
from gui.windows.searching_server import SearchingServer
from gui.windows.select_server import SelectServer
from gui.windows.login_page import LoginPage
from gui.windows.liste_session import SessionListPage
from gui.windows.create_session import SessionCreatePage

from src.endpoints.routes import message_route

class MainInterfaceClient(tk.Tk):
    def __init__(self, session_info):
        super().__init__()

        self.title("QuizNet")
        self.geometry("600x400")
        self.resizable(False, False)
        self.current_page = None

        self.tcp_client = None
        self.session_info = session_info

        self.frames = {}

        #Init all the different pages
        for F in (SearchingServer, SelectServer, LoginPage, SessionListPage, SessionCreatePage):
            frame = F(self)
            self.frames[F] = frame
            frame.place(relwidth=1, relheight=1)

        self.show_page(SearchingServer)

    # Shows a specific page
    def show_page(self, page_class):
        self.current_page = page_class
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

    def show_error_banner(self, text):
        frame = self.frames[self.current_page]

        if not hasattr(frame, "error_label"):
            frame.error_label = tk.Label(frame, fg="white", bg="red")
            frame.error_label.pack(fill="x")

        frame.error_label.config(text=text)


    def set_tcp_client(self, tcp_client):
        self.tcp_client = tcp_client