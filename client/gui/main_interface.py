import tkinter as tk
import os
from gui.windows.searching_server import SearchingServer
from gui.windows.select_server import SelectServer
from gui.windows.login_page import LoginPage
from gui.windows.liste_session import SessionListPage
from gui.windows.create_session import SessionCreatePage
from gui.windows.waiting_room import WaitingRoomPage
from gui.windows.countdown import CountdownPage
from gui.windows.game_questions import QuestionPage
from gui.windows.game_result_question import ResultsPage
from gui.windows.end_game import EndGamePage
from src.endpoints.routes import message_route
from PIL import Image, ImageTk

class MainInterfaceClient(tk.Tk):
    def __init__(self, session_info):
        super().__init__()

        self.title("QuizNet")
        self.geometry("800x600")
        self.resizable(False, False)
        self.images = {}
        self.load_assets()
        self.current_page = None

        self.tcp_client = None
        self.session_info = session_info

        self.frames = {}

        #Init all the different pages
        for F in (SearchingServer, SelectServer, LoginPage, SessionListPage, SessionCreatePage, WaitingRoomPage, CountdownPage, QuestionPage,ResultsPage, EndGamePage):
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
        print("-----------Message serveur reçu dans Tkinter :", message)
        try:
            message_route(message, self)
        except Exception as e:
            print("Erreur route:", e)

    def show_error_banner(self, text):
        page = self.frames[self.current_page]
    
        if hasattr(page, 'canvas'):
            self._hide_banner(page)
            bg_id = page.canvas.create_rectangle(0, 0, 800, 40, fill="#ff4a4a", outline="")
            
            text_id = page.canvas.create_text(
                400, 20, 
                text=text, 
                fill="white", 
                font=("Arial", 12, "bold")
            )

            page.banner_ids = [bg_id, text_id]
            if hasattr(page, 'banner_timer') and page.banner_timer:
                self.after_cancel(page.banner_timer)
            
            page.banner_timer = self.after(3000, lambda: self._hide_banner(page))
        else:
            print(f"ERREUR (Pas de canvas) : {text}")

    def _hide_banner(self, page):
        if hasattr(page, 'banner_ids') and page.banner_ids:
            for item_id in page.banner_ids:
                try:
                    page.canvas.delete(item_id)
                except Exception:
                    pass
            page.banner_ids = []
            page.banner_timer = None

    def set_tcp_client(self, tcp_client):
        self.tcp_client = tcp_client

    def load_assets(self):
        """Charge les images avec des chemins absolus pour éviter les erreurs."""
        try:
            assets_path = os.getcwd() 
            
            def load_img(filename, size=None):
                path = os.path.join(assets_path, filename)
                if not os.path.exists(path):
                    print(f"ATTENTION: Image introuvable -> {path}")
                    return None
                
                img = Image.open(path)
                if size:
                    img = img.resize(size, Image.Resampling.LANCZOS)
                return ImageTk.PhotoImage(img)
            
            self.images["bg"] = load_img("gui/images/bg_test.jpeg", (800, 600))

            btn_size = (200, 60)
            self.images["btn_normal"] = load_img("gui/images/btn_test.png", btn_size)
            self.images["btn_hover"] = load_img("gui/images/btn_hover_test.png", btn_size)
            self.images["btn_click"] = load_img("gui/images/btn_click_test.png", btn_size)
                
            print("Sucess")

        except Exception as e:
            print(f"\nError assets: {e}")
            self.images["bg"] = None
            self.images["btn_normal"] = None
            self.images["btn_hover"] = None
            self.images["btn_click"] = None