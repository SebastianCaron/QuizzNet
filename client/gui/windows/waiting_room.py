import tkinter as tk
from tkinter import messagebox
from gui.windows.liste_session import SessionListPage
from gui.christmas.christmas_button import ChristmasButton 

class WaitingRoomPage(tk.Frame):
    """
    Represents the Waiting Room (Lobby) page.
    
    This page is displayed after creating or joining a session.
    It lists all connected players in real-time.
    
    Key Features:
    - Real-time player list updates.
    - Dynamic "Start Game" button: Only visible and active for the Session Creator.
    - Status text indicating the game state (e.g., waiting for more players).
    """
    def __init__(self, app):
        """Initializes the Waiting Room UI components."""
        super().__init__(app)
        self.app = app

        # Canvas setup for background and flexible positioning
        self.canvas = tk.Canvas(self, width=800, height=600, highlightthickness=0)
        self.canvas.pack(fill="both", expand=True)

        if app.images.get("bg"):
            self.canvas.create_image(0, 0, image=app.images["bg"], anchor="nw")
        else:
            self.canvas.configure(bg="#1a1a1a")

        # Title
        self.canvas.create_text(
            400, 50, 
            text="Salle d'attente", 
            font=("Comic Sans MS", 28, "bold"), 
            fill="white"
        )

        # Player Listbox (styled to match the theme)
        self.players_listbox = tk.Listbox(
            self.canvas, 
            width=40, 
            height=10,
            font=("Comic Sans MS", 14),
            bg="#fdf5e6",
            fg="#165b33",
            selectbackground="#ff4a4a",
            selectforeground="white",
            bd=0,
            highlightthickness=0
        )
        self.canvas.create_window(400, 250, window=self.players_listbox)

        # Status text (feedback for the user)
        self.status_text_id = self.canvas.create_text(
            400, 420, 
            text="", 
            font=("Comic Sans MS", 14, "bold"), 
            fill="white"
        )

        # "Start Game" Button (initially hidden)
        self.button_start = ChristmasButton(
            canvas=self.canvas,
            x=400, y=500,
            text="Démarrer la partie",
            command=self.send_start_request,
            app=self.app
        )
        
        # Hide the start button by default (it will be shown only if the user is the creator)
        self._set_button_visibility(False)

    def update_players(self, players, is_creator):
        """
        Updates the list of players and the visibility of the Start button.
        
        This is called whenever the server notifies of a player joining/leaving.
        
        :param players: List of player pseudos currently in the session.
        :param is_creator: Boolean indicating if the local user is the host.
        """
        # Refresh the Listbox
        self.players_listbox.delete(0, tk.END)
        for p in players:
            self.players_listbox.insert(tk.END, p)
        
        # Update button visibility based on role
        self._set_button_visibility(is_creator)

        # Update status text based on game readiness
        if is_creator and len(players) < 2:
            self.canvas.itemconfig(self.status_text_id, text="En attente de joueurs...")
        elif is_creator:
            self.canvas.itemconfig(self.status_text_id, text="Prêt à démarrer !")
        else:
            self.canvas.itemconfig(self.status_text_id, text="Seul le créateur peut démarrer la partie")

    def _set_button_visibility(self, visible):
        """
        Internal helper to toggle the 'Start Game' button's visibility.
        Used to ensure only the Host can see/click the start button.
        """
        state = "normal" if visible else "hidden"
        self.canvas.itemconfig(self.button_start.img_id, state=state)
        self.canvas.itemconfig(self.button_start.text_id, state=state)

    def send_start_request(self):
        """
        Callback for the 'Start Game' button.
        Checks if enough players are present before sending the request to server.
        """
        if self.players_listbox.size() < 2:
            messagebox.showwarning("Erreur", "Il faut au moins 2 joueurs.")
            return

        self.app.tcp_client.send("POST session/start")