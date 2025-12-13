import tkinter as tk

class CountdownPage(tk.Frame):
    def __init__(self, app):
        super().__init__(app)
        self.app = app
        self.value = 0

        self.label = tk.Label(self, text="", font=("Arial", 30))
        self.label.pack(pady=40)

    def set_countdown(self, value):
        self.value = value
        self.update_timer()

    def update_timer(self):
        if self.value <= 0:
            return

        self.label.config(text=str(self.value))
        self.value -= 1
        self.after(1000, self.update_timer)