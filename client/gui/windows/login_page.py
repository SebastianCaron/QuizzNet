import tkinter as tk
from tkinter import messagebox

class LoginPage(tk.Frame):
    def __init__(self, app):
        super().__init__(app)
        self.app = app

        # --- UI elements ---
        title = tk.Label(self, text="Connexion / Inscription", font=("Arial", 22))
        title.pack(pady=20)

        # Username field
        tk.Label(self, text="Nom d'utilisateur").pack()
        self.username_entry = tk.Entry(self, width=30)
        self.username_entry.pack(pady=5)

        # Password field
        tk.Label(self, text="Mot de passe").pack()
        self.password_entry = tk.Entry(self, show="*", width=30)
        self.password_entry.pack(pady=5)

        # Status label
        self.status = tk.Label(self, text="", fg="red")
        self.status.pack(pady=10)

        # Buttons
        btn_frame = tk.Frame(self)
        btn_frame.pack(pady=10)

        login_btn = tk.Button(btn_frame, text="Se connecter", command=self.login)
        login_btn.grid(row=0, column=0, padx=10)

        register_btn = tk.Button(btn_frame, text="S'inscrire", command=self.register)
        register_btn.grid(row=0, column=1, padx=10)

    # ----------------------------------------------------------
    # ACTIONS
    # ----------------------------------------------------------

    def login(self):
        username = self.username_entry.get().strip()
        password = self.password_entry.get().strip()

        if not username or not password:
            messagebox.showwarning("Erreur", "Veuillez remplir tous les champs.")
            return

        
        message = (
            "POST player/login\n"
            "{\n"
            f'  "pseudo":"{username}",\n'
            f'  "password":"{password}"\n'
            "}\n"
        )
        print("Message envoyé au serveur:", repr(message))
        self.app.tcp_client.send(message)

        self.status.config(text="Connexion en cours...")

    def register(self):
        username = self.username_entry.get().strip()
        password = self.password_entry.get().strip()

        if not username or not password:
            messagebox.showwarning("Erreur", "Veuillez remplir tous les champs.")
            return

 
        message = (
            "POST player/register\n"
            "{\n"
            f'  "pseudo":"{username}",\n'
            f'  "password":"{password}"\n'
            "}\n"
        )
   
        self.app.tcp_client.send(message)
        print("Message envoyé au serveur:", repr(message))
        self.status.config(text="Inscription en cours...")

    # Appelée quand Tk raise la page
    def on_show(self):
        self.status.config(text="")
        self.username_entry.delete(0, tk.END)
        self.password_entry.delete(0, tk.END)
