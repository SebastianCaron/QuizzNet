import json
from src.session.session_infos import info_session 
from gui.windows.waiting_room import WaitingRoomPage

def response_session_player_join(message, app):
    try:
        json_message = json.loads(message)
    except:
        return "ERROR JSON"


    info_session.set_new_player(json_message["pseudo"])

    page = app.frames[WaitingRoomPage]
    page.update_players(info_session.players, info_session.is_creator)
