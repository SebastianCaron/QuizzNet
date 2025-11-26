import tkinter as tk
from tkinter import messagebox

class SessionListPage(tk.Frame):
    def __init__(self, app):
        super().__init__(app)
        self.app = app
        self.sessions = []

        tk.Label(self, text="Liste des sessions", font=("Arial", 20)).pack(pady=20)

        self.listbox = tk.Listbox(self, width=50, height=12)
        self.listbox.pack()

        btn_frame = tk.Frame(self)
        btn_frame.pack(pady=15)

        tk.Button(btn_frame, text="Rejoindre", command=self.join_session).grid(row=0, column=0, padx=10)
        tk.Button(btn_frame, text="Créer une session", command=self.create_session).grid(row=0, column=1, padx=10)

        self.status = tk.Label(self, text="", fg="red")
        self.status.pack()

    def update_sessions(self, sessions):
        self.sessions = sessions
        self.listbox.delete(0, tk.END)

        for session in sessions:
            sid = session["id"]
            name = session["name"]
            players = session["players"]
            maxp = session["max_players"]
            self.listbox.insert(tk.END, f"{name} ({players}/{maxp}) — id:{sid}")

    def join_session(self):
        idx = self.listbox.curselection()
        if not idx:
            messagebox.showwarning("Erreur", "Veuillez choisir une session.")
            return

        session = self.sessions[idx[0]]
        sid = session["id"]

        msg = f"POST session/join\n{{\"session_id\":{sid}}}"
        self.app.tcp_client.send(msg)
        self.status.config(text="Demande de connexion...")

    def create_session(self):
        msg = "POST session/create\n{}"
        self.app.tcp_client.send(msg)
        self.status.config(text="Création de session...")
