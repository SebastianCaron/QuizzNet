import tkinter as tk
import json
from tkinter import ttk, messagebox

class SessionCreatePage(tk.Frame):
    def __init__(self, app):
        super().__init__(app)
        self.app = app

        tk.Label(self, text="Créer une session", font=("Arial", 20)).pack(pady=20)

        form = tk.Frame(self)
        form.pack()

        # NOM
        tk.Label(form, text="Nom de la session").grid(row=0, column=0, sticky="w")
        self.entry_name = tk.Entry(form, width=30)
        self.entry_name.grid(row=0, column=1, pady=5)

        # DIFFICULTÉ
        tk.Label(form, text="Difficulté").grid(row=1, column=0, sticky="w")
        self.difficulty = ttk.Combobox(form, values=["facile", "moyen", "difficile"])
        self.difficulty.current(1)
        self.difficulty.grid(row=1, column=1, pady=5)

        # NB QUESTIONS
        tk.Label(form, text="Nombre de questions").grid(row=2, column=0, sticky="w")
        self.nb_questions = tk.Spinbox(form, from_=5, to=50, width=5)
        self.nb_questions.grid(row=2, column=1, pady=5)

        # TEMPS PAR QUESTION
        tk.Label(form, text="Temps limite (s)").grid(row=3, column=0, sticky="w")
        self.time_limit = tk.Spinbox(form, from_=5, to=120, width=5)
        self.time_limit.grid(row=3, column=1, pady=5)

        # MODE
        tk.Label(form, text="Mode").grid(row=4, column=0, sticky="w")
        self.mode = ttk.Combobox(form, values=["solo", "multi"])
        self.mode.current(0)
        self.mode.grid(row=4, column=1, pady=5)

        # MAX PLAYERS
        tk.Label(form, text="Joueurs max").grid(row=5, column=0, sticky="w")
        self.max_players = tk.Spinbox(form, from_=1, to=10, width=5)
        self.max_players.grid(row=5, column=1, pady=5)

        # THEMES
        tk.Label(self, text="Choisir les thèmes :", font=("Arial", 14)).pack(pady=10)

        self.theme_vars = []
        self.theme_frame = tk.Frame(self)
        self.theme_frame.pack()

        # BUTTONS
        btn_frame = tk.Frame(self)
        btn_frame.pack(pady=20)

        tk.Button(btn_frame, text="Créer", width=15, command=self.send_create_request)\
            .grid(row=0, column=0, padx=10)

        tk.Button(btn_frame, text="Retour", width=15, 
                  command=lambda: app.show_page("SessionListPage"))\
            .grid(row=0, column=1, padx=10)

    # ----------------------
    # MISE À JOUR DES THÈMES
    # ----------------------
    def update_theme_list(self, themes):
        for widget in self.theme_frame.winfo_children():
            widget.destroy()

        self.theme_vars = []

        for theme in themes:
            var = tk.IntVar()
            cb = tk.Checkbutton(self.theme_frame, text=theme["name"], variable=var)
            cb.pack(anchor="w")
            self.theme_vars.append((var, theme["id"]))

    # ----------------------
    # ENVOI AU SERVEUR
    # ----------------------
    def send_create_request(self):
        name = self.entry_name.get()
        if not name:
            messagebox.showwarning("Erreur", "Veuillez entrer un nom.")
            return

        selected_theme_ids = [tid for var, tid in self.theme_vars if var.get() == 1]
        if not selected_theme_ids:
            messagebox.showwarning("Erreur", "Sélectionnez au moins un thème.")
            return

        payload = {
            "name": name,
            "themeIds": selected_theme_ids,
            "difficulty": self.difficulty.get(),
            "nbQuestions": int(self.nb_questions.get()),
            "timeLimit": int(self.time_limit.get()),
            "mode": self.mode.get(),
            "maxPlayers": int(self.max_players.get())
        }

        msg = "POST session/create\n" + json.dumps(payload, indent=2)
        self.app.tcp_client.send(msg)
