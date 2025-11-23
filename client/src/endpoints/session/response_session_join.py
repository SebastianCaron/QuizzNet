import json
from client.src.session.session_infos import info_session 

def response_session_join(message):
    try :
        json_message = json.loads(message)
    except:
        return "ERROR JSON"
    
    if json_message["mode"] == "battle":
        info_session.set_nb_lives(json_message["lives"])

    for player in json_message["players"]:
        info_session.set_new_player(player)

    dict_nb_joker = json_message["jokers"]
    info_session.set_joker_fifty = dict_nb_joker["fifty"]
    info_session.set_joker_pass = dict_nb_joker["skip"]

    #passer à la salle d'attente de la session