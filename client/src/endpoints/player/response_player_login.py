import json
import time
from src.session.session_infos import info_session 

def response_player_login(message, app):
    """
    Handles the response to a 'POST player/login' request.

    This function processes the server's verification after the user attempts
    to log in.
    - If successful (200): It sets up the local user state, refreshes the UI,
      and automatically requests the list of active game sessions.
    - If failed (e.g., wrong password, user already connected): It extracts
      the error message and displays it via an error banner.

    :param message: The JSON string response from the server.
    :param app: The main Tkinter application instance.
    """
    try :
        json_message = json.loads(message)
    except:
        return "ERROR JSON"

    if json_message["statut"]=="200":
        
        # Initialize the local player session with the pseudo entered in the form
        info_session.set_new_player(info_session.get_pseudo())
        app.update()
        time.sleep(0.1)
        
        # Proceed to the main menu by fetching available sessions
        app.tcp_client.send("GET session/list")
        
    else:        
        # Extract and show the specific reason sent by the server (e.g., "User already logged in")
        error_msg = json_message.get("message", "Erreur inconnue")
        app.show_error_banner(error_msg)