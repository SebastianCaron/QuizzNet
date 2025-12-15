import json
from src.session.session_infos import info_session 

def response_player_eliminated(message, app):
    """
    Handles the 'POST session/player/eliminated' notification from the server.

    This function is triggered in Battle Mode when a player loses all their lives.
    It updates the local session state to mark the specific player as eliminated.

    :param message: The JSON body containing the pseudo of the eliminated player.
    :param app: The main Tkinter application instance.
    """
    try :
        json_message = json.loads(message)
    except:
        return "ERROR JSON"
    
    # Mark the player as eliminated in the local scoreboard.
    # Setting the score to -1 is the convention used to indicate 'Eliminated' status.
    info_session.update_score(json_message["pseudo"], -1)