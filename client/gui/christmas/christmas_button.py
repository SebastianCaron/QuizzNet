import tkinter as tk

class ChristmasButton:
    def __init__(self, canvas, x, y, text, command, app):
        self.canvas = canvas
        self.app = app
        self.command = command
        self.text = text
        
        self.is_disabled = False
        self.img_normal = app.images.get("btn_normal")
        self.img_hover = app.images.get("btn_hover")
        self.img_click = app.images.get("btn_click")
        self.img_id = None
        self.text_id = None

        self.draw(x, y)

    def draw(self, x, y):
        if self.img_normal:
            self.img_id = self.canvas.create_image(x, y, image=self.img_normal)
        else:
            self.img_id = self.canvas.create_rectangle(x-100, y-30, x+100, y+30, fill="gray")

        self.text_id = self.canvas.create_text(
            x, y, 
            text=self.text, 
            font=("Comic Sans MS", 12, "bold"), 
            fill="white"
        )

        for item in [self.img_id, self.text_id]:
            self.canvas.tag_bind(item, "<Enter>", self.on_enter)
            self.canvas.tag_bind(item, "<Leave>", self.on_leave)
            self.canvas.tag_bind(item, "<Button-1>", self.on_click)
            self.canvas.tag_bind(item, "<ButtonRelease-1>", self.on_release)

    def set_state(self, state):
        #state can be 'normal' or 'disabled'

        if state == "disabled":
            self.is_disabled = True
            self.canvas.itemconfig(self.text_id, fill="#aaaaaa")
            if self.img_normal:
                self.canvas.itemconfig(self.img_id, image=self.img_normal)
            self.canvas.config(cursor="")
        else:
            self.is_disabled = False
            self.canvas.itemconfig(self.text_id, fill="white")

    def on_enter(self, event):
        if self.is_disabled: return
        
        if self.img_hover and self.img_id:
            self.canvas.itemconfig(self.img_id, image=self.img_hover)
        self.canvas.config(cursor="hand2")

    def on_leave(self, event):
        if self.is_disabled: return

        if self.img_normal and self.img_id:
            self.canvas.itemconfig(self.img_id, image=self.img_normal)
        self.canvas.config(cursor="")

    def on_click(self, event):
        if self.is_disabled: return

        if self.img_click and self.img_id:
            self.canvas.itemconfig(self.img_id, image=self.img_click)

    def on_release(self, event):
        if self.is_disabled: return

        if self.img_hover and self.img_id:
            self.canvas.itemconfig(self.img_id, image=self.img_hover)
        
        if self.command:
            self.command()