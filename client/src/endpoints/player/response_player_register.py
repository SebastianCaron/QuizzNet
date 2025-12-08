import json
import time
from src.session.session_infos import info_session 

def response_player_register(message, app):
    try :
        json_message = json.loads(message)
    except:
        return "ERROR JSON"

    if json_message["statut"]=="201":
        info_session.set_new_player(info_session.get_pseudo())
        app.update()
        time.sleep(0.1)
        app.tcp_client.send("GET session/list")
    else:
        error_msg = json_message.get("message", "Erreur inconnue")
        print(f"Login échoué : {error_msg}")
        app.show_error_banner(error_msg)
    