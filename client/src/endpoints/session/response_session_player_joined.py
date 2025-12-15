import json
from src.session.session_infos import info_session 
from gui.windows.waiting_room import WaitingRoomPage

def response_session_player_join(message, app):
    """
    Handles the 'POST session/player/joined' notification from the server.
    
    This function is triggered when a new player connects to the current session.
    It adds the player to the local tracking and updates the Waiting Room UI.

    :param message: The JSON body string containing the new player's pseudo.
    :param app: The main Tkinter application instance.
    """
    try:
        json_message = json.loads(message)
    except:
        return "ERROR JSON"

    # Add the new player to the local session state
    info_session.set_new_player(json_message["pseudo"])

    # Refresh the Waiting Room listbox to show the new arrival
    page = app.frames[WaitingRoomPage]
    page.update_players(info_session.get_players(), info_session.is_creator)
