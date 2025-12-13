import tkinter as tk
from gui.windows.waiting_room import WaitingRoomPage
from src.session.session_infos import info_session

class EndGamePage(tk.Frame):
    def __init__(self, app):
        super().__init__(app)
        self.app = app

        tk.Label(self, text="Fin de la partie", font=("Arial", 22)).pack(pady=15)

        self.mode_label = tk.Label(self, text="", font=("Arial", 12))
        self.mode_label.pack(pady=5)
        self.winner_label = tk.Label(self, text="", font=("Arial", 14))
        self.winner_label.pack(pady=5)

        self.ranking_frame = tk.Frame(self)
        self.ranking_frame.pack(pady=15)

        btn_frame = tk.Frame(self)
        btn_frame.pack(pady=20)

        tk.Button(
            btn_frame, text="Rejouer", width=15,
            command=self.replay
        ).grid(row=0, column=0, padx=10)

        tk.Button(
            btn_frame, text="Quitter", width=15,
            command=self.quit_app
        ).grid(row=0, column=1, padx=10)

    def set_ranking(self, mode, ranking, winner):
        for w in self.ranking_frame.winfo_children():
            w.destroy()

        self.mode_label.config(text=f"Mode : {mode}")
        if mode == "battle" :
            self.winner_label.config(text=f"Gagnant : {winner}")
        header = tk.Frame(self.ranking_frame)
        header.pack(fill="x", pady=5)

        tk.Label(header, text="Rang", width=6, font=("Arial", 10, "bold")).grid(row=0, column=0)
        tk.Label(header, text="Pseudo", width=15, font=("Arial", 10, "bold")).grid(row=0, column=1)
        tk.Label(header, text="Score", width=10, font=("Arial", 10, "bold")).grid(row=0, column=2)
        tk.Label(header, text="Bonnes réponses", width=16, font=("Arial", 10, "bold")).grid(row=0, column=3)
        if mode == "battle" :
            tk.Label(header, text="Vies restantes", width=16, font=("Arial", 10, "bold")).grid(row=0, column=4)


        for player in ranking:
            row = tk.Frame(self.ranking_frame)
            row.pack(fill="x")

            tk.Label(row, text=player["rank"], width=6).grid(row=0, column=0)
            tk.Label(row, text=player["pseudo"], width=15).grid(row=0, column=1)
            tk.Label(row, text=player["score"], width=10).grid(row=0, column=2)
            tk.Label(row, text=player["correctAnswers"], width=16).grid(row=0, column=3)
            if mode == "battle" :
                tk.Label(row, text=player["lives"], width=16).grid(row=0, column=4)

    def replay(self):
        info_session.reset_for_new_game()
        self.app.show_page(WaitingRoomPage)

    def quit_app(self):
        self.app.destroy()