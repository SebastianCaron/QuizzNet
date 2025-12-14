import tkinter as tk

class CountdownPage(tk.Frame):
    def __init__(self, app):
        super().__init__(app)
        self.app = app
        self.value = 0

        self.canvas = tk.Canvas(self, width=800, height=600, highlightthickness=0)
        self.canvas.pack(fill="both", expand=True)

        if app.images.get("bg"):
            self.canvas.create_image(0, 0, image=app.images["bg"], anchor="nw")
        else:
            self.canvas.configure(bg="#1a1a1a")

        self.canvas.create_text(
            400, 200, 
            text="La partie va commencer...", 
            font=("Comic Sans MS", 28, "bold"), 
            fill="white"
        )

        self.countdown_text_id = self.canvas.create_text(
            400, 320, 
            text="", 
            font=("Comic Sans MS", 120, "bold"),
            fill="white"
        )

    def set_countdown(self, value):
        self.value = value
        self.update_timer()

    def update_timer(self):
        if self.value <= 0:
            self.canvas.itemconfig(self.countdown_text_id, text="GO !", fill="#32cd32") # Vert
            return

        self.canvas.itemconfig(self.countdown_text_id, text=str(self.value), fill="white")
        self.value -= 1
        self.after(1000, self.update_timer)