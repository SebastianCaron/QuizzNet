import tkinter as tk
import json
from src.session.session_infos import info_session  
from gui.christmas.christmas_button import ChristmasButton 

class QuestionPage(tk.Frame):

    def __init__(self, app):
        super().__init__(app)
        self.app = app

        self.canvas = tk.Canvas(self, width=800, height=600, highlightthickness=0)
        self.canvas.pack(fill="both", expand=True)

        if app.images.get("bg"):
            self.canvas.create_image(0, 0, image=app.images["bg"], anchor="nw")
        else:
            self.canvas.configure(bg="#1a1a1a")

        self.nb_questions_id = self.canvas.create_text(
            50, 30, 
            text="Q: --", 
            font=("Comic Sans MS", 16, "bold"), 
            fill="white",
            anchor="w"
        )

        self.timer_id = self.canvas.create_text(
            750, 30, 
            text="Temps : --", 
            font=("Comic Sans MS", 18, "bold"), 
            fill="white",
            anchor="e"
        )

        self.nb_lives_id = self.canvas.create_text(
            400, 30, 
            text="", 
            font=("Comic Sans MS", 16, "bold"), 
            fill="#ff4a4a"
        )

        self.question_id = self.canvas.create_text(
            400, 150, 
            text="En attente...", 
            font=("Comic Sans MS", 22, "bold"), 
            fill="white",
            width=700,
            justify="center"
        )

        self.answers_frame = tk.Frame(self.canvas, bg="#fdf5e6", bd=0) 
        self.answers_window = self.canvas.create_window(400, 350, window=self.answers_frame, width=600, height=300)
        self.canvas.itemconfigure(self.answers_window, state='hidden')

        self.canvas.create_text(400, 520, text="--- Jokers ---", font=("Comic Sans MS", 12), fill="white")

        self.btn_fifty = ChristmasButton(
            canvas=self.canvas,
            x=300, y=560,
            text="50/50",
            command=lambda: self.use_joker("fifty"),
            app=self.app
        )

        self.btn_skip = ChristmasButton(
            canvas=self.canvas,
            x=500, y=560,
            text="Passer",
            command=lambda: self.use_joker("skip"),
            app=self.app
        )
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
            self.canvas.itemconfig(self.nb_lives_id, text=f"Vies : {info_session.get_nb_lives()}")
            self.battle_mode = True
        else:
            self.canvas.itemconfig(self.nb_lives_id, text="")

    def load_question(self, type, question, answers):
        
        self.clear_answers()
        self.nb_question_left = self.nb_question_left - 1
        self.time_left = self.time_limit

        self.canvas.itemconfig(self.question_id, text=question)
        self.canvas.itemconfig(self.nb_questions_id, text=f"Restantes : {self.nb_question_left}")
        
        if info_session.joker_fifty_available() and not info_session.is_eliminated():
            self.btn_fifty.set_state("normal")
        else:
            self.btn_fifty.set_state("disabled")

        if info_session.joker_pass_available() and not info_session.is_eliminated():
            self.btn_skip.set_state("normal")
        else:
            self.btn_skip.set_state("disabled")

        if self.battle_mode:
            if info_session.is_eliminated():
                 self.canvas.itemconfig(self.nb_lives_id, text="ELIMINÉ")
            else:
                 self.canvas.itemconfig(self.nb_lives_id, text=f"Vies : {info_session.get_nb_lives()}")

        self.canvas.itemconfigure(self.answers_window, state='normal')
        
        btn_bg = "#ffffff"
        btn_fg = "#165b33"
        btn_font = ("Comic Sans MS", 14, "bold")

        if type == "qcm":
            for i, ans in enumerate(answers):
                b = tk.Button(
                    self.answers_frame, 
                    text=ans, 
                    width=40,
                    bg=btn_bg, fg=btn_fg, font=btn_font,
                    activebackground="#d42426", activeforeground="white",
                    command=lambda idx=i: self.send_answer(idx)
                )
                b.pack(pady=5)
                if info_session.is_eliminated():
                    b.config(state="disabled")

        elif type == "boolean":
            bv = tk.Button(self.answers_frame, text="Vrai", width=40, bg="#ddffdd", fg="green", font=btn_font,
                      command=lambda: self.send_answer(True))
            bv.pack(pady=10)

            bf = tk.Button(self.answers_frame, text="Faux", width=40, bg="#ffdddd", fg="red", font=btn_font,
                      command=lambda: self.send_answer(False))
            bf.pack(pady=10)
            
            if info_session.is_eliminated():
                    bf.config(state="disabled")
                    bv.config(state="disabled")

        elif type == "text":
            self.answer_entry = tk.Entry(self.answers_frame, width=40, font=("Comic Sans MS", 14))
            self.answer_entry.pack(pady=20)

            b = tk.Button(self.answers_frame, text="Valider", width=20, bg="white", font=btn_font,
                      command=lambda: self.send_answer(self.answer_entry.get()))
            b.pack(pady=10)
            
            if info_session.is_eliminated():
                    b.config(state="disabled")
            

        self.timer_running = True
        self.update_timer()

    def update_question_joker(self, new_answers):
        """Appelé quand le serveur renvoie les réponses filtrées (50/50)"""
        self.clear_answers()
        
        btn_bg = "#ffffff"
        btn_fg = "#165b33"
        btn_font = ("Comic Sans MS", 14, "bold")

        for i, ans in enumerate(new_answers):
            if ans == "": 
                tk.Label(self.answers_frame, text="---", bg="#fdf5e6").pack(pady=5)
            else:
                b = tk.Button(self.answers_frame, text=ans, width=40, bg=btn_bg, fg=btn_fg, font=btn_font,
                            command=lambda idx=i: self.send_answer(idx))
                b.pack(pady=5)


    def clear_answers(self):
        for w in self.answers_frame.winfo_children():
            w.destroy()

    def update_timer(self):
        if not self.timer_running:
            return

        color = "white"
        if self.time_left < 5:
            color = "#ff0000"
        
        self.canvas.itemconfig(self.timer_id, text=f"Temps : {self.time_left:.1f}", fill=color)
        
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
        
        self.canvas.itemconfigure(self.answers_window, state='hidden')
        self.canvas.itemconfig(self.question_id, text="Réponse envoyée ! En attente des autres...")
        self.canvas.itemconfig(self.timer_id, text="")

    def use_joker(self, joker_type):
        if joker_type == "fifty":
            if info_session.joker_fifty_available() and not info_session.is_eliminated():
                
                self.btn_fifty.set_state("disabled") 
                
                self.app.tcp_client.send("POST joker/use\n{\"type\":\"fifty\"}\n")
                info_session.use_joker_fifty()

        elif joker_type == "skip":
            if info_session.joker_pass_available() and not info_session.is_eliminated():
                
                self.btn_skip.set_state("disabled")
                
                self.app.tcp_client.send("POST joker/use\n{\"type\":\"skip\"}\n")
                info_session.use_joker_pass()