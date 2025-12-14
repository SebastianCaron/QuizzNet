import tkinter as tk

class CountdownPage(tk.Frame):
    """
    Displays a visual countdown before the game starts.
    
    This page serves as a transition between the Waiting Room and the first Question.
    """
    def __init__(self, app):
        """Initializes the Countdown UI."""
        super().__init__(app)
        self.app = app
        self.value = 0

        # Canvas setup
        self.canvas = tk.Canvas(self, width=800, height=600, highlightthickness=0)
        self.canvas.pack(fill="both", expand=True)

        # Background handling
        if app.images.get("bg"):
            self.canvas.create_image(0, 0, image=app.images["bg"], anchor="nw")
        else:
            self.canvas.configure(bg="#1a1a1a")

        # Static Title Text
        self.canvas.create_text(
            400, 200, 
            text="La partie va commencer...", 
            font=("Comic Sans MS", 28, "bold"), 
            fill="white"
        )

        # Dynamic Countdown Text
        self.countdown_text_id = self.canvas.create_text(
            400, 320, 
            text="", 
            font=("Comic Sans MS", 120, "bold"),
            fill="white"
        )

    def set_countdown(self, value):
        """
        Starts the countdown sequence from a specific number.
        
        :param value: The starting integer
        """
        self.value = value
        self.update_timer()

    def update_timer(self):
        """
        Recursive function to update the displayed number every second.
        
        - If value > 0: Displays the number and schedules the next update.
        - If value <= 0: Displays "GO !" in green to signal the start.
        """
        if self.value <= 0:
            # End of countdown: Show "GO !"
            self.canvas.itemconfig(self.countdown_text_id, text="GO !", fill="#32cd32") 
            return

        # Update text and decrement
        self.canvas.itemconfig(self.countdown_text_id, text=str(self.value), fill="white")
        self.value -= 1
        
        # Schedule the next call in 1 second
        self.after(1000, self.update_timer)