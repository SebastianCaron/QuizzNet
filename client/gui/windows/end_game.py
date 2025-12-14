import tkinter as tk
from gui.windows.liste_session import SessionListPage
from src.session.session_infos import info_session
from gui.christmas.christmas_button import ChristmasButton 

class EndGamePage(tk.Frame):
    def __init__(self, app):
        super().__init__(app)
        self.app = app

        # 1. SETUP CANVAS
        self.canvas = tk.Canvas(self, width=800, height=600, highlightthickness=0)
        self.canvas.pack(fill="both", expand=True)

        if app.images.get("bg"):
            self.canvas.create_image(0, 0, image=app.images["bg"], anchor="nw")
        else:
            self.canvas.configure(bg="#1a1a1a")

        self.canvas.create_text(
            400, 50, 
            text="Fin de la partie", 
            font=("Comic Sans MS", 30, "bold"), 
            fill="white"
        )

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

        # Cadre du tableau
        self.ranking_frame = tk.Frame(self.canvas, bg="#fdf5e6", bd=2, relief="groove")
        # On ne fixe pas de width ici, la frame s'adaptera au contenu de la grille
        self.canvas.create_window(400, 320, window=self.ranking_frame)

        self.btn_replay = ChristmasButton(
            canvas=self.canvas,
            x=300, y=550,
            text="Rejouer",
            command=self.replay,
            app=self.app
        )

        self.btn_quit = ChristmasButton(
            canvas=self.canvas,
            x=500, y=550,
            text="Quitter",
            command=self.quit_app,
            app=self.app
        )

    def set_ranking(self, mode, ranking, winner):
        # 1. Nettoyer le tableau précédent
        for w in self.ranking_frame.winfo_children():
            w.destroy()

        # 2. Mettre à jour les textes du Canvas
        self.canvas.itemconfig(self.mode_text_id, text=f"Mode : {mode}")
        
        if mode == "battle":
            self.canvas.itemconfig(self.winner_text_id, text=f"🏆 Gagnant : {winner} 🏆")
        else:
             self.canvas.itemconfig(self.winner_text_id, text="--- Résultats ---")

        # 3. Paramètres de style
        bg_color = "#fdf5e6"
        fg_color = "#165b33"
        header_font = ("Comic Sans MS", 10, "bold")
        row_font = ("Comic Sans MS", 10)
        
        headers = ["Rang", "Pseudo", "Score", "Bonnes rép."]
        if mode == "battle":
            headers.append("Vies")

        for col_idx, text in enumerate(headers):
            lbl = tk.Label(self.ranking_frame, text=text, font=header_font, bg=bg_color, fg=fg_color)
            lbl.grid(row=0, column=col_idx, padx=10, pady=5) 

        for row_idx, player in enumerate(ranking, start=1):
            
            vals = [
                str(player["rank"]),
                player["pseudo"],
                str(player["score"]),
                str(player["correctAnswers"])
            ]
            
            if mode == "battle":
                lives_txt = str(player.get("lives", 0))
                if lives_txt == "0" and "lives" in player:
                    lives_txt = "X"
                vals.append(lives_txt)

            for col_idx, val in enumerate(vals):
                lbl = tk.Label(self.ranking_frame, text=val, font=row_font, bg=bg_color, fg="black")
                lbl.grid(row=row_idx, column=col_idx, padx=10, pady=2)

    def replay(self):
        info_session.reset_for_new_game()
        self.app.show_page(SessionListPage)

    def quit_app(self):
        self.app.destroy()