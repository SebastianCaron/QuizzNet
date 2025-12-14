import json
from src.session.session_infos import info_session 
from gui.windows.waiting_room import WaitingRoomPage

def response_session_player_left(message, app):
    """
    Handles the 'POST session/player/left' notification from the server.
    
    This function is triggered when another player disconnects or leaves the session.
    It updates the local data and refreshes the Waiting Room UI if the game
    has not started yet.

    :param message: The JSON body string containing the leaver's pseudo.
    :param app: The main Tkinter application instance.
    """
    try:
        json_message = json.loads(message)
    except:
        return "ERROR JSON"

    # Update the local session state (remove player or mark as disconnected)
    info_session.unset_player(json_message["pseudo"])

    # If the game is still in the lobby phase (Waiting Room), refresh the UI
    if not info_session.session_started :
        page = app.frames[WaitingRoomPage]
        # Refresh the listbox with the updated list of players
        page.update_players(info_session.players, info_session.is_creator)