import tkinter as tk
from tkinter import messagebox
from src.session.session_infos import info_session 
from gui.christmas.christmas_button import ChristmasButton 

class LoginPage(tk.Frame):
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
            400, 80, 
            text="Connexion / Inscription", 
            font=("Comic Sans MS", 28, "bold"), 
            fill="white"
        )

        self.canvas.create_text(400, 160, text="Nom d'utilisateur", font=("Comic Sans MS", 14, "bold"), fill="white")
        
        self.username_entry = tk.Entry(self, width=30, font=("Comic Sans MS", 12))
        self.canvas.create_window(400, 190, window=self.username_entry)

        self.canvas.create_text(400, 240, text="Mot de passe", font=("Comic Sans MS", 14, "bold"), fill="white")

        self.password_entry = tk.Entry(self, show="*", width=30, font=("Comic Sans MS", 12))
        self.canvas.create_window(400, 270, window=self.password_entry)

        self.status_text_id = self.canvas.create_text(
            400, 330, 
            text="", 
            font=("Comic Sans MS", 12, "bold"), 
            fill="white"
        )
        
        self.btn_login = ChristmasButton(
            canvas=self.canvas,
            x=280, y=400,
            text="Se connecter",
            command=self.login,
            app=self.app
        )

        self.btn_register = ChristmasButton(
            canvas=self.canvas,
            x=520, y=400,
            text="S'inscrire",
            command=self.register,
            app=self.app
        )

    def login(self):
        username = self.username_entry.get().strip()
        password = self.password_entry.get().strip()
        
        if not username or not password:
            messagebox.showwarning("Erreur", "Veuillez remplir tous les champs.")
            return

        info_session.set_pseudo(username)
        
        message = (
            "POST player/login\n"
            "{\n"
            f'  "pseudo":"{username}",\n'
            f'  "password":"{password}"\n'
            "}\n"
        )
        print("Message envoyé au serveur:", repr(message))
        self.app.tcp_client.send(message)
        self.canvas.itemconfig(self.status_text_id, text="Connexion en cours...")

    def register(self):
        username = self.username_entry.get().strip()
        password = self.password_entry.get().strip()
        
        if not username or not password:
            messagebox.showwarning("Erreur", "Veuillez remplir tous les champs.")
            return

        info_session.set_pseudo(username)
 
        message = (
            "POST player/register\n"
            "{\n"
            f'  "pseudo":"{username}",\n'
            f'  "password":"{password}"\n'
            "}\n"
        )
   
        self.app.tcp_client.send(message)
        print("Message envoyé au serveur:", repr(message))
        self.canvas.itemconfig(self.status_text_id, text="Inscription en cours...")

    def on_show(self):
        self.canvas.itemconfig(self.status_text_id, text="")
        self.username_entry.delete(0, tk.END)
        self.password_entry.delete(0, tk.END)