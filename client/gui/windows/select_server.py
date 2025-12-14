import tkinter as tk
from tkinter import messagebox
from src.network.connection_tcp_server import TCPClient
from gui.windows.login_page import LoginPage
from gui.christmas.christmas_button import ChristmasButton

class SelectServer(tk.Frame):
    def __init__(self, app):
        super().__init__(app)
        self.app = app
        self.servers = []

        self.canvas = tk.Canvas(self, width=800, height=600, highlightthickness=0)
        self.canvas.pack(fill="both", expand=True)

        if app.images.get("bg"):
            self.canvas.create_image(0, 0, image=app.images["bg"], anchor="nw")
        else:
            self.canvas.configure(bg="#1a1a1a")

        self.canvas.create_text(
            400, 50, 
            text="Serveurs trouvés", 
            font=("Comic Sans MS", 24, "bold"), 
            fill="white"
        )

        self.listbox = tk.Listbox(
            self.canvas, 
            height=10, 
            width=50, 
            font=("Comic Sans MS", 12),
            bg="#fdf5e6",
            fg="#165b33",
            selectbackground="#ff4a4a",
            selectforeground="white",
            bd=0,
            highlightthickness=0
        )
        self.canvas.create_window(400, 250, window=self.listbox)

        self.btn_connect = ChristmasButton(
            canvas=self.canvas,
            x=400, y=450,
            text="Se connecter",
            command=self.connect,
            app=self.app
        )

        self.status_text_id = self.canvas.create_text(
            400, 520, 
            text="", 
            font=("Comic Sans MS", 12, "bold"), 
            fill="white"
        )

    def update_server_list(self, servers):
        self.servers = servers
        self.listbox.delete(0, tk.END)
        for name, ip, port in servers:
            self.listbox.insert(tk.END, f"{name} — {ip}:{port}")

    def connect(self):
        idx = self.listbox.curselection()
        if not idx:
            messagebox.showwarning("Erreur", "Veuillez sélectionner un serveur.")
            return

        name, ip, port = self.servers[idx[0]]
        self.canvas.itemconfig(self.status_text_id, text=f"Connexion à {name}...")
        self.canvas.update() 

        client = TCPClient(ip=ip, port=port, app=self.app)

        if client.connect():
            self.canvas.itemconfig(self.status_text_id, text="Connecté !")
            self.app.set_tcp_client(client)
            self.app.show_page(LoginPage)
        else:
            self.canvas.itemconfig(self.status_text_id, text="Échec de la connexion.")