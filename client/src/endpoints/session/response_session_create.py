import json
from src.session.session_infos import info_session 
from gui.windows.waiting_room import WaitingRoomPage

def response_session_create(message, app):
    """
    Handles the response to a 'POST session/create' request.

    This function processes the server's confirmation after the user attempts
    to create a new game session.
    - If successful (201): It sets the local player as the 'Creator' (Host), 
      initializes game settings (lives, jokers), and enters the Waiting Room.
    - If failed: Displays an error message.

    :param message: The JSON response from the server.
    :param app: The main Tkinter application instance.
    """
    try:
        json_message = json.loads(message)
    except:
        return "ERROR JSON"

    if json_message["statut"] == "201":

        # Reset local session state for the new game
        info_session.reset_for_new_game()

        # Battle Mode setup: Set lives if applicable
        if "lives" in json_message:
            info_session.set_nb_lives(json_message["lives"])

        # Store game configuration (Jokers)
        dict_nb_joker = json_message["jokers"]
        info_session.set_joker_fifty(dict_nb_joker["fifty"])
        info_session.set_joker_pass(dict_nb_joker["skip"])

        # Mark local player as the Host (Grants "Start Game" button access)
        info_session.set_creator()
        
        # Add the creator to the player list
        info_session.set_new_player(info_session.get_pseudo())

        # Prepare and display the Waiting Room
        page = app.frames[WaitingRoomPage]
        # Update player list (initially just the host) and enable admin controls
        page.update_players(info_session.get_players(), is_creator=True)

        app.show_page(WaitingRoomPage)

    else:
        # Failure: show error banner
        app.show_error_banner("Impossible de créer la session")