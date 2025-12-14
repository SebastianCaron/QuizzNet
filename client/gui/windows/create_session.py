import tkinter as tk
from tkinter import ttk, messagebox
from gui.windows.liste_session import SessionListPage
from src.session.session_infos import info_session  
from gui.christmas.christmas_button import ChristmasButton 

class SessionCreatePage(tk.Frame):
    """
    Page allowing the user to configure and create a new game session.
    
    Features:
    - Form fields for session name, difficulty, question count, time limit, mode, and max players.
    - Dynamic list of themes fetched from the server.
    - Validation logic before sending the creation request.
    """
    def __init__(self, app):
        """Initializes the Session Creation UI."""
        super().__init__(app)
        self.app = app

        # Canvas setup for background and layout
        self.canvas = tk.Canvas(self, width=800, height=600, highlightthickness=0)
        self.canvas.pack(fill="both", expand=True)

        if app.images.get("bg"):
            self.canvas.create_image(0, 0, image=app.images["bg"], anchor="nw")
        else:
            self.canvas.configure(bg="#1a1a1a")

        # Page Title
        self.canvas.create_text(
            400, 40, 
            text="Créer une session", 
            font=("Comic Sans MS", 26, "bold"), 
            fill="white"
        )

        # Layout configuration variables
        label_x = 280
        input_x = 450
        start_y = 100
        step_y = 40

        # Session Name
        self.canvas.create_text(label_x, start_y, text="Nom de la session :", font=("Comic Sans MS", 12, "bold"), fill="white", anchor="e")
        self.entry_name = tk.Entry(self, width=25)
        self.canvas.create_window(input_x, start_y, window=self.entry_name, anchor="w")

        # Difficulty (Combobox)
        y = start_y + step_y
        self.canvas.create_text(label_x, y, text="Difficulté :", font=("Comic Sans MS", 12, "bold"), fill="white", anchor="e")
        self.difficulty = ttk.Combobox(self, values=["facile", "moyen", "difficile"], width=23)
        self.difficulty.current(1) # Default to 'moyen'
        self.canvas.create_window(input_x, y, window=self.difficulty, anchor="w")

        # Number of Questions (Spinbox)
        y += step_y
        self.canvas.create_text(label_x, y, text="Nombre de questions :", font=("Comic Sans MS", 12, "bold"), fill="white", anchor="e")
        self.nb_questions = tk.Spinbox(self, from_=5, to=50, width=5)
        self.canvas.create_window(input_x, y, window=self.nb_questions, anchor="w")

        # Time Limit per question (Spinbox)
        y += step_y
        self.canvas.create_text(label_x, y, text="Temps limite (s) :", font=("Comic Sans MS", 12, "bold"), fill="white", anchor="e")
        self.time_limit = tk.Spinbox(self, from_=10, to=60, width=5)
        self.canvas.create_window(input_x, y, window=self.time_limit, anchor="w")

        # Game Mode (Solo/Battle)
        y += step_y
        self.canvas.create_text(label_x, y, text="Mode de jeu :", font=("Comic Sans MS", 12, "bold"), fill="white", anchor="e")
        self.mode = ttk.Combobox(self, values=["solo", "battle"], width=23)
        self.mode.current(0)
        self.canvas.create_window(input_x, y, window=self.mode, anchor="w")

        # Max Players (Spinbox)
        y += step_y
        self.canvas.create_text(label_x, y, text="Joueurs max :", font=("Comic Sans MS", 12, "bold"), fill="white", anchor="e")
        self.max_players = tk.Spinbox(self, from_=2, to=8, width=5)
        self.canvas.create_window(input_x, y, window=self.max_players, anchor="w")

        # A specific frame is created to hold the dynamic checkboxes
        self.theme_frame = tk.Frame(self, bg="#fdf5e6", bd=2, relief="groove")
        self.theme_window_id = self.canvas.create_window(400, y + 90, window=self.theme_frame, width=500, height=100)
        self.theme_vars = []

        #Buttons
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
        """
        Dynamically populates the theme selection area.
        
        This method is called when the server sends the list of available themes.
        It creates a Checkbutton for each theme and arranges them in a grid.
        
        :param themes: List of theme dictionaries (id, name).
        """
        # Clear existing widgets to avoid duplication on refresh
        for widget in self.theme_frame.winfo_children():
            widget.destroy()

        self.theme_vars = []

        # Grid configuration
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

            # Store the variable and the theme ID for later retrieval
            self.theme_vars.append((var, theme["id"]))

            # Move to next cell in grid
            col += 1
            if col >= cols:
                col = 0
                row += 1
                
    def send_create_request(self):
        """
        Collects form data, validates it, and sends the creation request to the server.
        """
        # Basic Validation
        name = self.entry_name.get()
        if not name:
            messagebox.showwarning("Erreur", "Veuillez entrer un nom.")
            return

        # Theme Validation (At least one must be selected)
        selected_theme_ids = [tid for var, tid in self.theme_vars if var.get() == 1]
        if not selected_theme_ids:
            messagebox.showwarning("Erreur", "Sélectionnez au moins un thème.")
            return

        # Construct the Protocol Message
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

        # Update local session state immediately
        info_session.set_nb_questions(int(self.nb_questions.get()))
        info_session.set_time_limit(int(self.time_limit.get()))
        
        # Send to Server
        self.app.tcp_client.send(message)