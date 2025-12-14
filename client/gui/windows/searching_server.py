import tkinter as tk
from src.network.discover_udp_list import serverlist_discover_UDP_server
from gui.windows.select_server import SelectServer
from gui.christmas.christmas_button import ChristmasButton

class SearchingServer(tk.Frame):
    def __init__(self, app):
        super().__init__(app)
        self.app = app

        self.canvas = tk.Canvas(self, width=800, height=600, highlightthickness=0)
        self.canvas.pack(fill="both", expand=True)
        if app.images.get("bg"):
            self.canvas.create_image(0, 0, image=app.images["bg"], anchor="nw")
        else:
            self.canvas.configure(bg="#1a1a1a")

        self.canvas.create_text(
            400, 150, 
            text="Recherche de serveurs...", 
            font=("Comic Sans MS", 24, "bold"), 
            fill="white"
        )

        self.status_text_id = self.canvas.create_text(
            400, 220, 
            text="", 
            font=("Comic Sans MS", 14), 
            fill="white"
        )

        self.btn_search = ChristmasButton(
            canvas=self.canvas,
            x=400, y=350, 
            text="Chercher un serveur",
            command=self.search_servers,
            app=self.app
        )

    def on_show(self):
        self.canvas.itemconfig(self.status_text_id, text="")

    def search_servers(self):
        self.canvas.itemconfig(self.status_text_id, text="Recherche en cours...")
        self.after(100, self._search_task)

    def _search_task(self):
        servers = serverlist_discover_UDP_server()

        if not servers:
            self.canvas.itemconfig(self.status_text_id, text="Aucun serveur trouvé.")
            return
        
        select_page = self.app.frames[SelectServer]
        select_page.update_server_list(servers)
        self.app.show_page(SelectServer)