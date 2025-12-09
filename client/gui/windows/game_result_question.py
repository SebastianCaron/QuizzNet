import tkinter as tk

class ResultsPage(tk.Frame):
    def __init__(self, app):
        super().__init__(app)
        self.app = app

        tk.Label(self, text="Résultats", font=("Arial", 22)).pack(pady=10)

        self.label_good_answer = tk.Label(self, text="").pack()
        self.label_explanation = tk.Label(self, text="", wraplength=400, fg="grey").pack(pady=10)

        tk.Label(self, text="En attente de la prochaine question...").pack(pady=25)

    def set_result(self, good_answer, exp, results):

        self.label_good_answer.config(text = f"Bonne réponse : {good_answer}")
        self.label_explanation.config(text = f"{exp}")

        for player in results:
            txt = f"{player['pseudo']} - {'✔️' if player['correct'] else '❌'} - Score {player['totalScore']}"
            tk.Label(self, text=txt).pack()