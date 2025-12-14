import tkinter as tk
from tkinter import ttk, messagebox
from gui.windows.liste_session import SessionListPage
from src.session.session_infos import info_session  
from gui.christmas.christmas_button import ChristmasButton 

class SessionCreatePage(tk.Frame):
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
            400, 40, 
            text="Créer une session", 
            font=("Comic Sans MS", 26, "bold"), 
            fill="white"
        )

        label_x = 280
        input_x = 450
        start_y = 100
        step_y = 40

        self.canvas.create_text(label_x, start_y, text="Nom de la session :", font=("Comic Sans MS", 12, "bold"), fill="white", anchor="e")
        
        self.entry_name = tk.Entry(self, width=25)
        self.canvas.create_window(input_x, start_y, window=self.entry_name, anchor="w")

        y = start_y + step_y
        self.canvas.create_text(label_x, y, text="Difficulté :", font=("Comic Sans MS", 12, "bold"), fill="white", anchor="e")
        
        self.difficulty = ttk.Combobox(self, values=["facile", "moyen", "difficile"], width=23)
        self.difficulty.current(1)
        self.canvas.create_window(input_x, y, window=self.difficulty, anchor="w")

        y += step_y
        self.canvas.create_text(label_x, y, text="Nombre de questions :", font=("Comic Sans MS", 12, "bold"), fill="white", anchor="e")
        
        self.nb_questions = tk.Spinbox(self, from_=5, to=50, width=5)
        self.canvas.create_window(input_x, y, window=self.nb_questions, anchor="w")

        y += step_y
        self.canvas.create_text(label_x, y, text="Temps limite (s) :", font=("Comic Sans MS", 12, "bold"), fill="white", anchor="e")
        
        self.time_limit = tk.Spinbox(self, from_=10, to=60, width=5)
        self.canvas.create_window(input_x, y, window=self.time_limit, anchor="w")

        y += step_y
        self.canvas.create_text(label_x, y, text="Mode de jeu :", font=("Comic Sans MS", 12, "bold"), fill="white", anchor="e")
        
        self.mode = ttk.Combobox(self, values=["solo", "battle"], width=23)
        self.mode.current(0)
        self.canvas.create_window(input_x, y, window=self.mode, anchor="w")

        y += step_y
        self.canvas.create_text(label_x, y, text="Joueurs max :", font=("Comic Sans MS", 12, "bold"), fill="white", anchor="e")
        
        self.max_players = tk.Spinbox(self, from_=2, to=8, width=5)
        self.canvas.create_window(input_x, y, window=self.max_players, anchor="w")

        self.theme_frame = tk.Frame(self, bg="#fdf5e6", bd=2, relief="groove")
        
        self.theme_window_id = self.canvas.create_window(400, y + 90, window=self.theme_frame, width=500, height=100)

        self.theme_vars = []

        btn_y = 530

        self.btn_create = ChristmasButton(
            canvas=self.canvas,
            x=300, y=btn_y,
            text="Créer",
            command=self.send_create_request,
            app=self.app
        )

        self.btn_back = ChristmasButton(
            canvas=self.canvas,
            x=500, y=btn_y,
            text="Retour",
            command=lambda: app.show_page(SessionListPage),
            app=self.app
        )


    def update_theme_list(self, themes):
        for widget in self.theme_frame.winfo_children():
            widget.destroy()

        self.theme_vars = []

        cols = 3
        row = 0
        col = 0
        
        bg_color = "#fdf5e6"

        for theme in themes:
            var = tk.IntVar()
            cb = tk.Checkbutton(
                self.theme_frame, 
                text=theme["name"], 
                variable=var,
                bg=bg_color,
                activebackground=bg_color,
                font=("Comic Sans MS", 10, "bold")
            )

            cb.grid(row=row, column=col, sticky="w", padx=10, pady=5)

            self.theme_vars.append((var, theme["id"]))

            col += 1
            if col >= cols:
                col = 0
                row += 1
                
    def send_create_request(self):
        name = self.entry_name.get()
        if not name:
            messagebox.showwarning("Erreur", "Veuillez entrer un nom.")
            return

        selected_theme_ids = [tid for var, tid in self.theme_vars if var.get() == 1]
        if not selected_theme_ids:
            messagebox.showwarning("Erreur", "Sélectionnez au moins un thème.")
            return

        message = (
            "POST session/create\n"
            "{\n"
            f'  "name":"{name}",\n'
            f'  "themeIds":{selected_theme_ids},\n'
            f'  "difficulty":"{self.difficulty.get()}",\n'
            f'  "nbQuestions":{int(self.nb_questions.get())},\n'
            f'  "timeLimit":{int(self.time_limit.get())},\n'
            f'  "mode":"{self.mode.get()}",\n'
            f'  "maxPlayers":{int(self.max_players.get())}\n'
            "}\n"
        )

        info_session.set_nb_questions(int(self.nb_questions.get()))
        info_session.set_time_limit(int(self.time_limit.get()))
        self.app.tcp_client.send(message)