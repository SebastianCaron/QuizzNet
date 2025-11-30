import json
from gui.windows.liste_session import SessionListPage

def response_session_list(message, app):
    try :
        json_message = json.loads(message)
    except:
        return "ERROR JSON"
    
    if json_message["nbSessions"] == 0:
        page = app.frames[SessionListPage]
        page.update_sessions([])
    else:
        liste_session = json_message["sessions"]
        page = app.frames[SessionListPage]
        page.update_sessions(liste_session)

    app.show_page(SessionListPage)
