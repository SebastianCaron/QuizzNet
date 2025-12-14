import tkinter as tk

class ChristmasButton:
    """
    A custom button class drawn directly onto a Tkinter Canvas.
    
    This class uses Canvas items (Images and Text).
    This allows for:
    1. Transparent backgrounds (vital for the Christmas theme).
    2. Custom visual states (Normal, Hover, Click).
    3. Better integration with the overall graphical design.

    This is not possible using a classic Tkinter button.
    """
    def __init__(self, canvas, x, y, text, command, app):
        """
        Initializes the button.

        :param canvas: The parent tk.Canvas instance.
        :param x: The X coordinate (center of the button).
        :param y: The Y coordinate (center of the button).
        :param text: The text to display on the button.
        :param command: The function to call when clicked.
        :param app: Reference to the main App (to access loaded images).
        """
        self.canvas = canvas
        self.app = app
        self.command = command
        self.text = text
        
        self.is_disabled = False
        
        # Retrieve pre-loaded images from the main app instance
        self.img_normal = app.images.get("btn_normal")
        self.img_hover = app.images.get("btn_hover")
        self.img_click = app.images.get("btn_click")
        
        # Canvas item IDs
        self.img_id = None
        self.text_id = None

        self.draw(x, y)

    def draw(self, x, y):
        """
        Creates the visual elements on the canvas and binds mouse events.
        """
        # Draw the background image (or a fallback rectangle if image fails)
        if self.img_normal:
            self.img_id = self.canvas.create_image(x, y, image=self.img_normal)
        else:
            # Fallback for debugging or missing assets
            self.img_id = self.canvas.create_rectangle(x-100, y-30, x+100, y+30, fill="gray")

        # Draw the text overlay
        self.text_id = self.canvas.create_text(
            x, y, 
            text=self.text, 
            font=("Comic Sans MS", 12, "bold"), 
            fill="white"
        )

        # Bind interactions to BOTH the image and the text
        # This ensures the button reacts even if the user clicks exactly on the letters.
        for item in [self.img_id, self.text_id]:
            self.canvas.tag_bind(item, "<Enter>", self.on_enter)
            self.canvas.tag_bind(item, "<Leave>", self.on_leave)
            self.canvas.tag_bind(item, "<Button-1>", self.on_click)
            self.canvas.tag_bind(item, "<ButtonRelease-1>", self.on_release)

    def set_state(self, state):
        """
        Enables or disables the button.
        
        :param state: 'normal' or 'disabled'.
        """
        if state == "disabled":
            self.is_disabled = True
            # Visual feedback: Grey out text, reset cursor and no highlight when hovered
            self.canvas.itemconfig(self.text_id, fill="#aaaaaa")
            if self.img_normal:
                self.canvas.itemconfig(self.img_id, image=self.img_normal)
            self.canvas.config(cursor="")
        else:
            self.is_disabled = False
            # Visual feedback: White text
            self.canvas.itemconfig(self.text_id, fill="white")

    def on_enter(self, event):
        """Mouse Hover: Change image to 'hover' version and change cursor."""
        if self.is_disabled: return
        
        if self.img_hover and self.img_id:
            self.canvas.itemconfig(self.img_id, image=self.img_hover)
        self.canvas.config(cursor="hand2")

    def on_leave(self, event):
        """Mouse Exit: Revert image to normal and reset cursor."""
        if self.is_disabled: return

        if self.img_normal and self.img_id:
            self.canvas.itemconfig(self.img_id, image=self.img_normal)
        self.canvas.config(cursor="")

    def on_click(self, event):
        """Mouse Down: Change image to 'pressed' version."""
        if self.is_disabled: return

        if self.img_click and self.img_id:
            self.canvas.itemconfig(self.img_id, image=self.img_click)

    def on_release(self, event):
        """Mouse Up: Revert image to hover state and execute command."""
        if self.is_disabled: return

        # Visual cleanup
        if self.img_hover and self.img_id:
            self.canvas.itemconfig(self.img_id, image=self.img_hover)
        
        # Execute the actual callback function
        if self.command:
            self.command()