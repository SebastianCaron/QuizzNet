import json
from src.session.session_infos import info_session 
from gui.windows.waiting_room import WaitingRoomPage

def response_session_join(message, app):
    """
    Handles the response to a 'POST session/join' request.

    This function processes the server's decision when the user tries to join 
    an existing session.
    - If successful (201): It initializes local game state (jokers, lives, 
      list of existing players) and moves the user to the Waiting Room.
    - If failed (400/403): It displays an appropriate error banner.

    :param message: The JSON response from the server.
    :param app: The main Tkinter application instance.
    """
    try:
        json_message = json.loads(message)
    except:
        return "ERROR JSON"

    if json_message["statut"] == "201":

        # Setup Battle Mode specific settings (lives)
        if json_message["mode"] == "battle":
            info_session.set_nb_lives(json_message["lives"])

        # Sync the list of players already present in the session
        for player in json_message["players"]:
            info_session.set_new_player(player)

        # Initialize available jokers for this session
        dict_nb_joker = json_message["jokers"]
        info_session.set_joker_fifty(dict_nb_joker["fifty"])
        info_session.set_joker_pass(dict_nb_joker["skip"])

        # Update and show the Waiting Room
        page = app.frames[WaitingRoomPage]
        page.update_players(info_session.get_players(), is_creator=False)

        app.show_page(WaitingRoomPage)

    elif json_message["statut"] == "403":
        # Case: The session has reached its maximum player capacity
        app.show_error_banner("Session full")
    
    elif json_message["statut"] == "400":
        # Case: Logic error (e.g., user is already connected in this session)
        app.show_error_banner("Compte déjà dans cette session")

    else :
        # Case: Unknown server error
        app.show_error_banner("Impossible de rejoindre la session")
