import json
from src.session.session_infos import info_session 

def response_session_started(message):
    try :
        json_message = json.loads(message)
    except:
        return "ERROR JSON"
    
    info_session.session_has_started()

    #passer à la fenetre de début de jeu avec countdown dans le message json