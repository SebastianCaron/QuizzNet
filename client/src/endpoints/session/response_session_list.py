import json
from gui.windows.liste_session import SessionListPage

def response_session_list(message, app):
    try :
        json_message = json.loads(message)
    except:
        return "ERROR JSON"
    
    liste_session = json_message["sessions"]
    page = app.frames[SessionListPage]
    page.update_sessions(liste_session)

