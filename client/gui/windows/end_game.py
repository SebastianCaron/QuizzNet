import tkinter as tk
from gui.windows.liste_session import SessionListPage
from src.session.session_infos import info_session
from gui.christmas.christmas_button import ChristmasButton 

class EndGamePage(tk.Frame):
    """
    Displays the final scoreboard at the end of a session.
    
    This page shows:
    - A detailed ranking table (Rank, Pseudo, Score, Correct Answers, Lives).
    - Options to play again (return to lobby list) or quit the application.
    """
    def __init__(self, app):
        """Initializes the End Game UI components."""
        super().__init__(app)
        self.app = app

        # Canvas setup
        self.canvas = tk.Canvas(self, width=800, height=600, highlightthickness=0)
        self.canvas.pack(fill="both", expand=True)

        if app.images.get("bg"):
            self.canvas.create_image(0, 0, image=app.images["bg"], anchor="nw")
        else:
            self.canvas.configure(bg="#1a1a1a")

        # Main Title
        self.canvas.create_text(
            400, 50, 
            text="Fin de la partie", 
            font=("Comic Sans MS", 30, "bold"), 
            fill="white"
        )

        # Game Mode Label
        self.mode_text_id = self.canvas.create_text(
            400, 100, 
            text="", 
            font=("Comic Sans MS", 14), 
            fill="white"
        )
        
        self.winner_text_id = self.canvas.create_text(
            400, 140, 
            text="", 
            font=("Comic Sans MS", 18, "bold"), 
            fill="white"
        )

        # Container for the Scoreboard Table
        self.ranking_frame = tk.Frame(self.canvas, bg="#fdf5e6", bd=2, relief="groove")
        self.canvas.create_window(400, 320, window=self.ranking_frame)

        # Replay Button (Go back to Session List)
        self.btn_replay = ChristmasButton(
            canvas=self.canvas,
            x=300, y=550,
            text="Rejouer",
            command=self.replay,
            app=self.app
        )

        # Quit Button (Close App)
        self.btn_quit = ChristmasButton(
            canvas=self.canvas,
            x=500, y=550,
            text="Quitter",
            command=self.quit_app,
            app=self.app
        )

    def set_ranking(self, mode, ranking, winner):
        """
        Populates the scoreboard with the final results received from the server.
        
        :param mode: Game mode ("battle" or "classic").
        :param ranking: List of player dictionaries sorted by rank.
        :param winner: Pseudo of the winner (only relevant for battle mode).
        """
        # Clear previous table data
        for w in self.ranking_frame.winfo_children():
            w.destroy()
            
        self.canvas.itemconfig(self.mode_text_id, text=f"Mode : {mode}")
        
        # Display Winner if applicable
        if mode == "battle":
            self.canvas.itemconfig(self.winner_text_id, text=f"🏆 Gagnant : {winner} 🏆")
        else:
             self.canvas.itemconfig(self.winner_text_id, text="--- Résultats ---")

        # Table Styling
        bg_color = "#fdf5e6"
        fg_color = "#165b33"
        header_font = ("Comic Sans MS", 10, "bold")
        row_font = ("Comic Sans MS", 10)
        
        # Define Table Headers
        headers = ["Rang", "Pseudo", "Score", "Bonnes rép."]
        if mode == "battle":
            headers.append("Vies")

        # Create Header Row
        for col_idx, text in enumerate(headers):
            lbl = tk.Label(self.ranking_frame, text=text, font=header_font, bg=bg_color, fg=fg_color)
            lbl.grid(row=0, column=col_idx, padx=10, pady=5) 

        # Create Data Rows
        for row_idx, player in enumerate(ranking, start=1):
            
            vals = [
                str(player["rank"]),
                player["pseudo"],
                str(player["score"]),
                str(player["correctAnswers"])
            ]
            
            # Handle 'Lives' column for Battle Mode
            if mode == "battle":
                lives_txt = str(player.get("lives", 0))
                if lives_txt == "0" and "lives" in player:
                    lives_txt = "X" # Visual indicator for eliminated players
                vals.append(lives_txt)

            for col_idx, val in enumerate(vals):
                lbl = tk.Label(self.ranking_frame, text=val, font=row_font, bg=bg_color, fg="black")
                lbl.grid(row=row_idx, column=col_idx, padx=10, pady=2)

    def replay(self):
        """Resets local session data and navigates back to the session list."""
        info_session.reset_for_new_game()
        self.app.show_page(SessionListPage)

    def quit_app(self):
        """Closes the application window."""
        self.app.destroy()