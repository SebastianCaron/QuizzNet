import json
from src.session.session_infos import info_session 
from gui.windows.waiting_room import WaitingRoomPage

def response_session_join(message, app):
    try:
        json_message = json.loads(message)
    except:
        return "ERROR JSON"

    if json_message["mode"] == "battle":
        info_session.set_nb_lives(json_message["lives"])

    for player in json_message["players"]:
        info_session.set_new_player(player)

    dict_nb_joker = json_message["jokers"]
    info_session.set_joker_fifty(dict_nb_joker["fifty"])
    info_session.set_joker_pass(dict_nb_joker["skip"])

    page = app.frames[WaitingRoomPage]
    page.update_players(info_session.get_players(), is_creator=False)

    app.show_page(WaitingRoomPage)

