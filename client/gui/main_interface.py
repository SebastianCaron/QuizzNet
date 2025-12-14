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
    """
    Main application class inheriting from Tkinter.
    
    It acts as the root window and the controller for the application.
    It manages the navigation between different pages (Frames) and holds
    shared resources like images and the network client connection.
    """
    def __init__(self, session_info):
        super().__init__()

        # Window configuration
        self.title("QuizNet")
        self.geometry("800x600")
        self.resizable(False, False)
        
        # Image cache to prevent garbage collection
        self.images = {}
        self.load_assets()
        
        self.current_page = None
        self.tcp_client = None
        self.session_info = session_info

        # Dictionary to store all page instances
        self.frames = {}

        # Initialize and stack all pages in the same window area.
        # We create all pages at startup, but only one is visible at a time.
        for F in (SearchingServer, SelectServer, LoginPage, SessionListPage, SessionCreatePage, WaitingRoomPage, CountdownPage, QuestionPage,ResultsPage, EndGamePage):
            frame = F(self)
            self.frames[F] = frame
            # Place them all in the same grid cell (overlapping)
            frame.place(relwidth=1, relheight=1)

        # Start by showing the server discovery page
        self.show_page(SearchingServer)

    def show_page(self, page_class):
        """
        Brings the requested page to the front (visible layer).
        
        :param page_class: The class of the page to display.
        """
        self.current_page = page_class
        frame = self.frames[page_class]
        
        # Lift the frame to the top of the stacking order
        frame.tkraise()
        
        # Trigger the 'on_show' hook if the page has one (to refresh data)
        if hasattr(frame, "on_show"):
            frame.on_show()

    def handle_server_message(self, message):
        """
        Callback triggered by the TCP thread when a message arrives.
        It routes the message to the appropriate handler.
        """
        print("----------- Message recieved in Tkinter -----------\n", message)
        try:
            message_route(message, self)
        except Exception as e:
            print("Erreur route:", e)

    def show_error_banner(self, text):
        """
        Displays a temporary red error banner at the top of the current page.
        Automatically disappears after 3 seconds.
        """
        page = self.frames[self.current_page]
    
        # Only draw if the current page has a canvas (game pages)
        if hasattr(page, 'canvas'):
            self._hide_banner(page) # Remove existing banner if any
            
            # Draw red background
            bg_id = page.canvas.create_rectangle(0, 0, 800, 40, fill="#ff4a4a", outline="")
            
            # Draw error text
            text_id = page.canvas.create_text(
                400, 20, 
                text=text, 
                fill="white", 
                font=("Arial", 12, "bold")
            )

            # Store IDs to delete them later
            page.banner_ids = [bg_id, text_id]
            
            # Cancel previous timer if exists
            if hasattr(page, 'banner_timer') and page.banner_timer:
                self.after_cancel(page.banner_timer)
            
            # Set timer to auto-hide
            page.banner_timer = self.after(3000, lambda: self._hide_banner(page))
        else:
            print(f"ERROR Canvas : {text}")

    def _hide_banner(self, page):
        """Internal helper to remove the error banner graphics."""
        if hasattr(page, 'banner_ids') and page.banner_ids:
            for item_id in page.banner_ids:
                try:
                    page.canvas.delete(item_id)
                except Exception:
                    pass
            page.banner_ids = []
            page.banner_timer = None

    def set_tcp_client(self, tcp_client):
        """Stores the network client instance for global access."""
        self.tcp_client = tcp_client

    def load_assets(self):
        """
        Loads UI images (backgrounds, buttons) from disk into memory.
        We keep references in self.images to avoid Tkinter garbage collection issues.
        """
        try:
            assets_path = os.getcwd() 
            
            def load_img(filename, size=None):
                path = os.path.join(assets_path, filename)
                if not os.path.exists(path):
                    print(f"WARNING: Image not found -> {path}")
                    return None
                
                img = Image.open(path)
                if size:
                    img = img.resize(size, Image.Resampling.LANCZOS)
                return ImageTk.PhotoImage(img)
            
            # Load and resize specific assets
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