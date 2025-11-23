import tkinter as tk
from tkinter import messagebox

class SelectServer(tk.Frame):
    def __init__(self, app):
        super().__init__(app)
        self.app = app
        self.servers = []

        tk.Label(self, text="Serveurs trouvés", font=("Arial", 20)).pack(pady=20)

        self.listbox = tk.Listbox(self, height=10, width=50)
        self.listbox.pack()

        tk.Button(self, text="Se connecter", command=self.connect).pack(pady=10)

        self.status = tk.Label(self, text="", fg="red")
        self.status.pack()

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
        client = self.app.tcp_client

        self.status.config(text=f"Connexion à {name}...")

        if client.connect():
            self.status.config(text="Connecté !")
            # self.app.show_page(LoginPage)
        else:
            self.status.config(text="Échec de la connexion.")
