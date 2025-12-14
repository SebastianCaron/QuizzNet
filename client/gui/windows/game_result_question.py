import tkinter as tk

class ResultsPage(tk.Frame):
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
            400, 50, 
            text="Résultats", 
            font=("Comic Sans MS", 28, "bold"), 
            fill="white"
        )

        self.good_answer_id = self.canvas.create_text(
            400, 120, 
            text="", 
            font=("Comic Sans MS", 18, "bold"), 
            fill="#32cd32"
        )

        self.explanation_id = self.canvas.create_text(
            400, 180, 
            text="", 
            font=("Comic Sans MS", 14), 
            fill="#eeeeee",
            width=600,      
            justify="center"
        )

        self.results_listbox = tk.Listbox(
            self.canvas, 
            width=60, 
            height=10,
            font=("Comic Sans MS", 12),
            bg="#fdf5e6",
            fg="#165b33",
            bd=0,
            highlightthickness=0
        )
        self.canvas.create_window(400, 350, window=self.results_listbox)

        self.canvas.create_text(
            400, 500, 
            text="En attente de la prochaine question...", 
            font=("Comic Sans MS", 12, "italic"), 
            fill="white"
        )

    def set_result(self, good_answer, exp, results):
        self.canvas.itemconfig(self.good_answer_id, text=f"Bonne réponse : {good_answer}")
        self.canvas.itemconfig(self.explanation_id, text=f"{exp}")

        self.results_listbox.delete(0, tk.END)

        for player in results:
            pseudo = player['pseudo']
            correct_icon = 'V' if player['correct'] else 'X'
            score = player['totalScore']

            txt = f"{pseudo} - {correct_icon} - Score: {score}"
            
            if "lives" in player:
                lives = player["lives"]
                if lives == 0:
                    txt += f" - Vies: {lives} (ELIMINÉ)"
                else:
                    txt += f" - Vies: {lives}"
            
            self.results_listbox.insert(tk.END, txt)