import json
from client.src.session.session_infos import info_session 

def response_session_player_join(message):
    try :
        json_message = json.loads(message)
    except:
        return "ERROR JSON"
    
    info_session.set_new_player(json_message["pseudo"])

    #update la fenêtre