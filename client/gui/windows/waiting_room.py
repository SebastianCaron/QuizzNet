import tkinter as tk
from tkinter import messagebox
from gui.windows.liste_session import SessionListPage

class WaitingRoomPage(tk.Frame):
    def __init__(self, app):
        super().__init__(app)
        self.app = app

        tk.Label(self, text="Salle d'attente", font=("Arial", 20)).pack(pady=15)

        self.players_listbox = tk.Listbox(self, width=40, height=10)
        self.players_listbox.pack(pady=10)

        self.status_label = tk.Label(self, text="", fg="blue", font=("Arial", 12))
        self.status_label.pack(pady=5)

        btn_frame = tk.Frame(self)
        btn_frame.pack(pady=15)

        self.button_start = tk.Button(btn_frame, text="Démarrer la partie", width=18, 
                                      command=self.send_start_request)
        self.button_start.grid(row=0, column=0, padx=10)

        tk.Button(btn_frame, text="Quitter", width=15, 
                  command=self.quit_session).grid(row=0, column=1, padx=10)


    def update_players(self, players, is_creator):
        self.players_listbox.delete(0, tk.END)

        for p in players:
            self.players_listbox.insert(tk.END, p)

        if is_creator:
            self.button_start.grid()
        else:
            self.button_start.grid_remove()

        if is_creator and len(players) < 2:
            self.status_label.config(text="En attente de joueurs...")
        elif is_creator:
            self.status_label.config(text="Prêt à démarrer !")
        else:
            self.status_label.config(text="Seul le créateur peut démarrer la partie")

    def send_start_request(self):
        if self.players_listbox.size() < 2:
            messagebox.showwarning("Erreur", "Il faut au moins 2 joueurs.")
            return

        print("POST session/start envoyé")
        self.app.tcp_client.send("POST session/start")

    def quit_session(self):
        #self.app.tcp_client.send("POST session/left\n{}")
        #TODO : endpoint left session in server AND handeler in client
        self.app.show_page(SessionListPage)
