import json
import time
from src.session.session_infos import info_session 

def response_player_register(message, app):
    """
    Handles the response to a 'POST player/register' request.

    This function processes the server's confirmation after the user attempts
    to register a new account.
    - If successful (201): It logs the user in locally (by setting the pseudo),
      forces a UI update, and immediately requests the list of available game sessions.
    - If failed: It extracts the error message from the server and displays it via a banner.

    :param message: The JSON response string from the server.
    :param app: The main Tkinter application instance.
    """
    try :
        json_message = json.loads(message)
    except:
        return "ERROR JSON"

    if json_message["statut"]=="201":
        
        # Register the player in the local session state using the stored pseudo
        info_session.set_new_player(info_session.get_pseudo())
        app.update()
        time.sleep(0.1)
        
        # Automatically transition to the next step: Fetching the session list
        app.tcp_client.send("GET session/list")

    else:
        
        # Extract and show the specific error message sent by the server
        error_msg = json_message.get("message", "Erreur inconnue")        
        app.show_error_banner(error_msg)