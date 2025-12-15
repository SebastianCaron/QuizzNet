import tkinter as tk
from tkinter import messagebox
from src.network.connection_tcp_server import TCPClient
from gui.windows.login_page import LoginPage
from gui.christmas.christmas_button import ChristmasButton

class SelectServer(tk.Frame):
    """
    Page responsible for displaying the list of discovered servers.
    
    Allows the user to select a server from the list and initiates 
    the TCP connection. If the connection is successful, it transitions 
    the application to the Login Page.
    """
    def __init__(self, app):
        """Initializes the server selection UI."""
        super().__init__(app)
        self.app = app
        self.servers = []

        # Use a Canvas for background image support and custom positioning
        self.canvas = tk.Canvas(self, width=800, height=600, highlightthickness=0)
        self.canvas.pack(fill="both", expand=True)

        if app.images.get("bg"):
            self.canvas.create_image(0, 0, image=app.images["bg"], anchor="nw")
        else:
            self.canvas.configure(bg="#1a1a1a")

        self.canvas.create_text(
            400, 50, 
            text="Serveurs trouvés", 
            font=("Comic Sans MS", 24, "bold"), 
            fill="white"
        )

        # Listbox to display the results of the UDP discovery
        self.listbox = tk.Listbox(
            self.canvas, 
            height=10, 
            width=50, 
            font=("Comic Sans MS", 12),
            bg="#fdf5e6",
            fg="#165b33",
            selectbackground="#ff4a4a",
            selectforeground="white",
            bd=0,
            highlightthickness=0
        )
        self.canvas.create_window(400, 250, window=self.listbox)

        # Connect Button
        self.btn_connect = ChristmasButton(
            canvas=self.canvas,
            x=400, y=450,
            text="Se connecter",
            command=self.connect,
            app=self.app
        )

        # Status text for feedback
        self.status_text_id = self.canvas.create_text(
            400, 520, 
            text="", 
            font=("Comic Sans MS", 12, "bold"), 
            fill="white"
        )

    def update_server_list(self, servers):
        """
        Populates the listbox with the servers found during discovery.
        
        :param servers: A list of tuples (name, ip, port).
        """
        self.servers = servers
        self.listbox.delete(0, tk.END)
        for name, ip, port in servers:
            self.listbox.insert(tk.END, f"{name} — {ip}:{port}")

    def connect(self):
        """
        Handles the connection process.
        
        Checks if a server is selected.
        Creates a TCPClient instance targeting the selected IP/Port.
        Attempts to connect.
        If successful, stores the client in the main App and moves to Login.
        """
        idx = self.listbox.curselection()
        if not idx:
            messagebox.showwarning("Erreur", "Veuillez sélectionner un serveur.")
            return

        name, ip, port = self.servers[idx[0]]
        
        # UI Feedback
        self.canvas.itemconfig(self.status_text_id, text=f"Connexion à {name}...")
        self.canvas.update() 

        # Create and attempt connection
        client = TCPClient(ip=ip, port=port, app=self.app)

        if client.connect():
            self.canvas.itemconfig(self.status_text_id, text="Connecté !")
            
            # Store the active connection in the main controller
            self.app.set_tcp_client(client)
            
            # Proceed to authentication
            self.app.show_page(LoginPage)
        else:
            self.canvas.itemconfig(self.status_text_id, text="Échec de la connexion.")