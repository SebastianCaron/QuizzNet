import tkinter as tk
from tkinter import messagebox
from src.session.session_infos import info_session  

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
        tk.Button(btn_frame, text="Rafraichir", command=self.refresh).grid(row=0, column=2, padx=10)

        self.status = tk.Label(self, text="", fg="red")
        self.status.pack()

    def refresh(self):
        self.app.tcp_client.send("GET session/list")


    def update_sessions(self, sessions):
        self.sessions = sessions
        self.listbox.delete(0, tk.END)

        dico_timer = {}
        dico_nbquest = {}
        for session in sessions:
            sid = session["id"]
            name = session["name"]
            nbplayers = session["nbPlayers"]
            maxp = session["maxPlayers"]
            self.listbox.insert(tk.END, f"{name} ({nbplayers}/{maxp}) — id:{sid}")

    def join_session(self):
        idx = self.listbox.curselection()
        if not idx:
            messagebox.showwarning("Erreur", "Veuillez choisir une session.")
            return

        session = self.sessions[idx[0]]
        sid = session["id"]
        
        info_session.set_time_limit(session["timeLimit"])
        info_session.set_nb_questions(session["nbQuestions"])

        message = (
            "POST session/join\n"
            "{\n"
            f'  "sessionId":{sid}\n'
            "}\n"
        )
        self.app.tcp_client.send(message)
        self.status.config(text="Demande de connexion...")

    def create_session(self):
        msg = "GET themes/list"
        self.app.tcp_client.send(msg)
        self.status.config(text="Création de session...")
