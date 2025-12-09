import json
from src.session.session_infos import info_session 
from gui.windows.countdown import CountdownPage

def response_session_started(message, app):
    try :
        json_message = json.loads(message)
    except:
        return "ERROR JSON"
    
    info_session.session_has_started()
    page = app.frames[CountdownPage]
    page.set_countdown(json_message["countdown"])
    app.show_page(CountdownPage)
