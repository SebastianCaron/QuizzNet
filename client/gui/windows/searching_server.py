import tkinter as tk
from src.network.discover_udp_list import serverlist_discover_UDP_server
from gui.windows.select_server import SelectServer

class SearchingServer(tk.Frame):
    def __init__(self, app):
        super().__init__(app)
        self.app = app

        tk.Label(self, text="Recherche de serveurs...", font=("Arial", 20)).pack(pady=40)

        self.status = tk.Label(self, text="", font=("Arial", 12))
        self.status.pack(pady=10)

        tk.Button(self, text="Chercher un serveur", command=self.search_servers)\
            .pack(pady=20)

    def on_show(self):
        self.status.config(text="")

    def search_servers(self):
        self.status.config(text="Recherche en cours...")
        self.after(100, self._search_task)

    def _search_task(self):
        servers = serverlist_discover_UDP_server()

        if not servers:
            self.status.config(text="Aucun serveur trouvé.")
            return
        
        select_page = self.app.frames[SelectServer]
        select_page.update_server_list(servers)
        self.app.show_page(SelectServer)
