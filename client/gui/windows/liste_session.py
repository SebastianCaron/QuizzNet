import tkinter as tk
from tkinter import messagebox
from src.session.session_infos import info_session 
from gui.christmas.christmas_button import ChristmasButton 

class SessionListPage(tk.Frame):
    def __init__(self, app):
        super().__init__(app)
        self.app = app
        self.sessions = []

        self.canvas = tk.Canvas(self, width=800, height=600, highlightthickness=0)
        self.canvas.pack(fill="both", expand=True)

        if app.images.get("bg"):
            self.canvas.create_image(0, 0, image=app.images["bg"], anchor="nw")
        else:
            self.canvas.configure(bg="#1a1a1a")

        self.canvas.create_text(
            400, 50, 
            text="Liste des sessions", 
            font=("Comic Sans MS", 28, "bold"), 
            fill="white"
        )

        self.listbox = tk.Listbox(
            self.canvas, 
            width=50, 
            height=12,
            font=("Comic Sans MS", 12),
            bg="#fdf5e6",
            fg="#165b33",
            selectbackground="#ff4a4a",
            selectforeground="white",
            bd=0,
            highlightthickness=0
        )
        self.canvas.create_window(400, 250, window=self.listbox)

        self.btn_refresh = ChristmasButton(
            canvas=self.canvas,
            x=200, y=450,
            text="Rafraichir",
            command=self.refresh,
            app=self.app
        )

        self.btn_join = ChristmasButton(
            canvas=self.canvas,
            x=400, y=450,
            text="Rejoindre",
            command=self.join_session,
            app=self.app
        )

        self.btn_create = ChristmasButton(
            canvas=self.canvas,
            x=600, y=450,
            text="Créer une session",
            command=self.create_session,
            app=self.app
        )

        self.status_text_id = self.canvas.create_text(
            400, 530, 
            text="", 
            font=("Comic Sans MS", 12, "bold"), 
            fill="white"
        )

    def refresh(self):
        self.app.tcp_client.send("GET session/list")
        self.canvas.itemconfig(self.status_text_id, text="Actualisation...")

    def update_sessions(self, sessions):
        self.sessions = sessions
        self.listbox.delete(0, tk.END)

        self.canvas.itemconfig(self.status_text_id, text="")

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
        self.canvas.itemconfig(self.status_text_id, text="Demande de connexion...")

    def create_session(self):
        msg = "GET themes/list"
        self.app.tcp_client.send(msg)
        self.canvas.itemconfig(self.status_text_id, text="Création de session...")