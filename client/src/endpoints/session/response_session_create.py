import json
from src.session.session_infos import info_session 
from gui.windows.liste_session import SessionListPage

def response_session_create(message, app):
    try :
        json_message = json.loads(message)
    except:
        return "ERROR JSON"

    if json_message["statut"]=="201":
        if "lives" in json_message:
            info_session.set_nb_lives(json_message["lives"])

        dict_nb_joker = json_message["jokers"]
        info_session.set_joker_fifty = dict_nb_joker["fifty"]
        info_session.set_joker_pass = dict_nb_joker["skip"]
        info_session.set_creator()
        app.tcp_client.send("GET sessions/list")

        #passer à la salle d'attente. Comme il est créateur, seul lui peut démarrer
        # VOIR COMMENT GERER SI CREATEUR LEFT AVANT DEBUT DE LA SESSION
    else:
        app.show_error_banner("Impossible de créer la session")
    