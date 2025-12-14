import tkinter as tk
from tkinter import messagebox
from gui.windows.liste_session import SessionListPage
from gui.christmas.christmas_button import ChristmasButton 

class WaitingRoomPage(tk.Frame):
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
            text="Salle d'attente", 
            font=("Comic Sans MS", 28, "bold"), 
            fill="white"
        )

        self.players_listbox = tk.Listbox(
            self.canvas, 
            width=40, 
            height=10,
            font=("Comic Sans MS", 14),
            bg="#fdf5e6",
            fg="#165b33",
            selectbackground="#ff4a4a",
            selectforeground="white",
            bd=0,
            highlightthickness=0
        )
        self.canvas.create_window(400, 250, window=self.players_listbox)

        self.status_text_id = self.canvas.create_text(
            400, 420, 
            text="", 
            font=("Comic Sans MS", 14, "bold"), 
            fill="white"
        )

        self.button_start = ChristmasButton(
            canvas=self.canvas,
            x=400, y=500,
            text="Démarrer la partie",
            command=self.send_start_request,
            app=self.app
        )
        
        self._set_button_visibility(False)

    def update_players(self, players, is_creator):
        self.players_listbox.delete(0, tk.END)
        for p in players:
            self.players_listbox.insert(tk.END, p)
        self._set_button_visibility(is_creator)

        if is_creator and len(players) < 2:
            self.canvas.itemconfig(self.status_text_id, text="En attente de joueurs...")
        elif is_creator:
            self.canvas.itemconfig(self.status_text_id, text="Prêt à démarrer !")
        else:
            self.canvas.itemconfig(self.status_text_id, text="Seul le créateur peut démarrer la partie")

    def _set_button_visibility(self, visible):
        """Helper pour cacher/montrer le bouton de Noël sur le Canvas"""
        state = "normal" if visible else "hidden"
        self.canvas.itemconfig(self.button_start.img_id, state=state)
        self.canvas.itemconfig(self.button_start.text_id, state=state)

    def send_start_request(self):
        if self.players_listbox.size() < 2:
            messagebox.showwarning("Erreur", "Il faut au moins 2 joueurs.")
            return

        print("POST session/start envoyé")
        self.app.tcp_client.send("POST session/start")