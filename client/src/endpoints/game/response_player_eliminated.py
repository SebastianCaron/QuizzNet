import json
from src.session.session_infos import info_session 

def response_player_eliminated(message, app):
    try :
        json_message = json.loads(message)
    except:
        return "ERROR JSON"
    
    info_session.update_score(json_message["pseudo"], -1)