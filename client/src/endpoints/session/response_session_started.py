import json
from src.session.session_infos import info_session 
from gui.windows.countdown import CountdownPage

def response_session_started(message, app):
    print("--- PASSE PAR SESSION STARTED ---")
    print("APP DANS RESPONSE:", app)
    print("type(app):", type(app))
    try :
        json_message = json.loads(message)
    except:
        print("ERREUR JSON")
        return "ERROR JSON"
        
    page = app.frames[CountdownPage]
    app.show_page(CountdownPage)
    page.set_countdown(json_message["countdown"])
    info_session.session_has_started()

