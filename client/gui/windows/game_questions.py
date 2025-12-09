import tkinter as tk
from src.session.session_infos import info_session  

class QuestionPage(tk.Frame):

    def __init__(self, app):
        super().__init__(app)
        self.app = app

        self.question_label = tk.Label(self, text="", font=("Arial", 18), wraplength=600, justify="center")
        self.question_label.pack(pady=20)

        self.timer_label = tk.Label(self, text="Temps : --", font=("Arial", 16), fg="red")
        self.timer_label.pack(pady=10)

        self.nb_questions_label =  tk.Label(self, text="Questions restantes : --", font=("Arial", 14), fg="black")
        self.timer_label.pack(pady=30)

        self.answers_frame = tk.Frame(self)
        self.answers_frame.pack(pady=20)

        jok_frame = tk.Frame(self)
        jok_frame.pack(pady=10)

        self.btn_fifty = tk.Button(jok_frame, text="50/50", command=lambda: self.use_joker("fifty"))
        self.btn_fifty.grid(row=0, column=0, padx=10)

        self.btn_skip = tk.Button(jok_frame, text="Passer", command=lambda: self.use_joker("skip"))
        self.btn_skip.grid(row=0, column=1, padx=10)

        self.time_limit = info_session.get_time_limit()
        self.nb_question_left = info_session.get_nb_questions()+1
        self.time_left = 0
        self.timer_running = False

    def load_question(self, type, question, answers):

        self.clear_answers()
        self.nb_question_left-=1
        self.time_left = self.time_limit
        self.timer_running = True

        self.question_label.config(text=question)
        self.nb_questions_label.config(text = f"Questions restantes : {self.nb_question_left}")

        if type == "qcm":
            for i, ans in enumerate(answers):
                b = tk.Button(self.answers_frame, text=ans, width=30,
                              command=lambda idx=i: self.send_answer(idx))
                b.pack(pady=5)

        elif type == "boolean":
            tk.Button(self.answers_frame, text="Vrai", width=30,
                      command=lambda: self.send_answer(True)).pack(pady=5)

            tk.Button(self.answers_frame, text="Faux", width=30,
                      command=lambda: self.send_answer(False)).pack(pady=5)

        elif type == "text":
            self.answer_entry = tk.Entry(self.answers_frame, width=40, font=("Arial", 14))
            self.answer_entry.pack(pady=5)

            tk.Button(self.answers_frame, text="Valider", width=20,
                      command=lambda: self.send_answer(self.answer_entry.get())).pack(pady=10)

        self.update_timer()

    def update_question_joker(self, new_answers):
        for i, ans in enumerate(new_answers):
            b = tk.Button(self.answers_frame, text=ans, width=30,
                            command=lambda idx=i: self.send_answer(idx))
            b.pack(pady=5)


    def clear_answers(self):
        for w in self.answers_frame.winfo_children():
            w.destroy()

    def update_timer(self):
        if not self.timer_running:
            return

        self.timer_label.config(text=f"Temps : {self.time_left}")

        self.time_left -= 0.1
        self.after(100, self.update_timer)

    def send_answer(self, value):
        self.timer_running = False


        message = (
            "POST question/answer\n"
            "{\n"
            f'  "answer":{value},\n'
            f'  "responseTime":{self.time_limit - self.time_left}\n'
            "}\n"
        )
        self.app.tcp_client.send(message)

        self.clear_answers()
        self.question_label.config(text="Réponse envoyée !")
        self.timer_label.config(text="X")

    def use_joker(self, joker_type):
        if joker_type == "fifty":
            if info_session.joker_fifty_available():
                self.app.tcp_client.send("POST joker/use\n{\"type\":\"fifty\"}\n")
                info_session.joker_fifty-=1

        elif joker_type == "skip":
            if info_session.joker_pass_available():
                self.app.tcp_client.send("POST joker/use\n{\"type\":\"skip\"}\n")
                info_session.joker_pass-=1
