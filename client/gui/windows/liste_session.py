import tkinter as tk
from tkinter import messagebox
from src.session.session_infos import info_session 
from gui.christmas.christmas_button import ChristmasButton 

class SessionListPage(tk.Frame):
    """
    Displays the list of active game sessions available on the server.
    
    This page serves as the main hub after login. The user can:
    1. View available lobbies (Name, Player count).
    2. Refresh the list.
    3. Join an existing lobby.
    4. Start the process of creating a new session.
    """
    def __init__(self, app):
        """Initializes the Session List UI."""
        super().__init__(app)
        self.app = app
        self.sessions = [] # Local cache of session data

        # Canvas setup for background and layout
        self.canvas = tk.Canvas(self, width=800, height=600, highlightthickness=0)
        self.canvas.pack(fill="both", expand=True)

        if app.images.get("bg"):
            self.canvas.create_image(0, 0, image=app.images["bg"], anchor="nw")
        else:
            self.canvas.configure(bg="#1a1a1a")

        # Title
        self.canvas.create_text(
            400, 50, 
            text="Liste des sessions", 
            font=("Comic Sans MS", 28, "bold"), 
            fill="white"
        )

        # Listbox to display sessions
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

        # Refresh Button
        self.btn_refresh = ChristmasButton(
            canvas=self.canvas,
            x=200, y=450,
            text="Rafraichir",
            command=self.refresh,
            app=self.app
        )

        # Join Button
        self.btn_join = ChristmasButton(
            canvas=self.canvas,
            x=400, y=450,
            text="Rejoindre",
            command=self.join_session,
            app=self.app
        )

        # Create Session Button
        self.btn_create = ChristmasButton(
            canvas=self.canvas,
            x=600, y=450,
            text="Créer une session",
            command=self.create_session,
            app=self.app
        )

        # Status text for user feedback
        self.status_text_id = self.canvas.create_text(
            400, 530, 
            text="", 
            font=("Comic Sans MS", 12, "bold"), 
            fill="white"
        )

    def refresh(self):
        """
        Sends a request to the server to get the latest list of sessions.
        """
        self.app.tcp_client.send("GET session/list")
        self.canvas.itemconfig(self.status_text_id, text="Actualisation...")

    def update_sessions(self, sessions):
        """
        Callback to update the UI with data received from the server.
        
        :param sessions: A list of dictionaries containing session details.
        """
        self.sessions = sessions
        self.listbox.delete(0, tk.END)

        self.canvas.itemconfig(self.status_text_id, text="")

        # Format and display each session in the listbox
        for session in sessions:
            sid = session["id"]
            name = session["name"]
            nbplayers = session["nbPlayers"]
            maxp = session["maxPlayers"]
            self.listbox.insert(tk.END, f"{name} ({nbplayers}/{maxp}) — id:{sid}")

    def join_session(self):
        """
        Handles the logic when the user clicks 'Join'.
        
        1. Validates that a session is selected.
        2. Updates local state with session parameters (time limit, question count).
        3. Sends a join request to the server.
        """
        idx = self.listbox.curselection()
        if not idx:
            messagebox.showwarning("Erreur", "Veuillez choisir une session.")
            return

        session = self.sessions[idx[0]]
        sid = session["id"]
        
        # Load session settings into the local state early
        info_session.set_time_limit(session["timeLimit"])
        info_session.set_nb_questions(session["nbQuestions"])

        # Construct and send the Join request
        message = (
            "POST session/join\n"
            "{\n"
            f'  "sessionId":{sid}\n'
            "}\n"
        )
        self.app.tcp_client.send(message)
        self.canvas.itemconfig(self.status_text_id, text="Demande de connexion...")

    def create_session(self):
        """
        Initiates the session creation flow.
        
        """
        msg = "GET themes/list"
        self.app.tcp_client.send(msg)
        self.canvas.itemconfig(self.status_text_id, text="Création de session...")