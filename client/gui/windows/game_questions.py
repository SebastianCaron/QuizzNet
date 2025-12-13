import tkinter as tk
import json
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
        self.nb_questions_label.pack(pady=30)

        self.nb_lives_label =  tk.Label(self, text="Vies restantes : --", font=("Arial", 16), fg="red")
        self.nb_lives_label.pack(pady=20)
        self.nb_lives_label.pack_forget()

        self.answers_frame = tk.Frame(self)
        self.answers_frame.pack(pady=20)

        jok_frame = tk.Frame(self)
        jok_frame.pack(pady=10)

        self.btn_fifty = tk.Button(jok_frame, text="50/50", command=lambda: self.use_joker("fifty"))
        self.btn_fifty.grid(row=0, column=0, padx=10)

        self.btn_skip = tk.Button(jok_frame, text="Passer", command=lambda: self.use_joker("skip"))
        self.btn_skip.grid(row=0, column=1, padx=10)

        self.time_limit = 0
        self.nb_question_left = 0
        self.time_left = 0
        self.timer_running = False
        self.after_id = None
        self.battle_mode = False

    def set_timer_and_questions(self):
        self.time_limit = info_session.get_time_limit()
        self.nb_question_left = info_session.get_nb_questions()+1
        print("--- Time Limit : ", self.time_limit, " --- Nombre Questions :", self.nb_question_left)
        if info_session.is_battle_mode():
            self.nb_lives_label.pack(pady=10)
            self.battle_mode = True

    def load_question(self, type, question, answers):
        self.btn_skip.config(state="disabled")
        self.btn_fifty.config(state="disabled")
        if info_session.joker_pass_available():
            self.btn_skip.config(state="normal")
        self.clear_answers()
        self.nb_question_left = self.nb_question_left - 1
        self.time_left = self.time_limit

        self.question_label.config(text=question)
        self.nb_questions_label.config(text = f"Questions restantes : {self.nb_question_left}")
        if self.battle_mode and info_session.is_eliminated() :
            self.nb_lives_label.config(text="Vies restantes : 0 - ELIMINE")
        elif self.battle_mode :
            self.nb_lives_label.config(text=f"Vies restantes : {info_session.get_nb_lives()}")

        if type == "qcm":
            if info_session.joker_fifty_available() and (not info_session.is_eliminated()):
                self.btn_fifty.config(state="normal")
            for i, ans in enumerate(answers):
                b = tk.Button(self.answers_frame, text=ans, width=30,
                              command=lambda idx=i: self.send_answer(idx))
                b.pack(pady=5)
                if info_session.is_eliminated():
                    b.config(state="disabled")

        elif type == "boolean":
            bv = tk.Button(self.answers_frame, text="Vrai", width=30,
                      command=lambda: self.send_answer(True))
            bv.pack(pady=5)

            bf = tk.Button(self.answers_frame, text="Faux", width=30,
                      command=lambda: self.send_answer(False))
            bf.pack(pady=5)
            
            if info_session.is_eliminated():
                    bf.config(state="disabled")
                    bv.config(state="disabled")

        elif type == "text":
            self.answer_entry = tk.Entry(self.answers_frame, width=40, font=("Arial", 14))
            self.answer_entry.pack(pady=5)

            b = tk.Button(self.answers_frame, text="Valider", width=20,
                      command=lambda: self.send_answer(self.answer_entry.get()))
            b.pack(pady=10)
            if info_session.is_eliminated():
                    b.config(state="disabled")
            

        self.timer_running = True
        self.update_timer()

    def update_question_joker(self, new_answers):
        self.clear_answers()
        for i, ans in enumerate(new_answers):
            b = tk.Button(self.answers_frame, text=ans, width=30,
                            command=lambda idx=i: self.send_answer(idx))
            b.pack(pady=5)


    def clear_answers(self):
        for w in self.answers_frame.winfo_children():
            w.destroy()
        self.time_left = self.time_limit

    def update_timer(self):
        if not self.timer_running:
            return

        self.timer_label.config(text=f"Temps : {self.time_left:.1f}")
        self.time_left = self.time_left - 0.1
        self.after_id = self.after(100, self.update_timer)

    def stop_timer(self):
        self.timer_running = False
        if self.after_id :
            self.after_cancel(self.after_id)
            self.after_id = None

    def send_answer(self, value):
        self.stop_timer()

        payload = {
            "answer": value,
            "responseTime": self.time_limit - self.time_left
        }
        message = f"POST question/answer\n{json.dumps(payload)}\n"
        
        self.app.tcp_client.send(message)
        print("Envoyé au serveur : ", message)
        self.clear_answers()
        self.question_label.config(text="Réponse envoyée !")
        self.timer_label.config(text="")

    def use_joker(self, joker_type):
        if joker_type == "fifty":
            if info_session.joker_fifty_available():
                self.app.tcp_client.send("POST joker/use\n{\"type\":\"fifty\"}\n")
                info_session.use_joker_fifty()
                self.btn_fifty.config(state="disabled")

        elif joker_type == "skip":
            if info_session.joker_pass_available():
                self.app.tcp_client.send("POST joker/use\n{\"type\":\"skip\"}\n")
                info_session.use_joker_pass()
                self.btn_skip.config(state="disabled")

        else :
            print ("Erreur utilisation Joker")
