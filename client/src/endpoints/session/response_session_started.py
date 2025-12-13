import json
from src.session.session_infos import info_session 
from gui.windows.countdown import CountdownPage
from gui.windows.game_questions import QuestionPage

def response_session_started(message, app):
    print("--- PASSE PAR SESSION STARTED ---")
    print("APP DANS RESPONSE:", app)
    print("type(app):", type(app))
    try :
        json_message = json.loads(message)
    except:
        print("ERREUR JSON")
        return "ERROR JSON"
    
    page_question = app.frames[QuestionPage]
    page_question.set_timer_and_questions()
    page = app.frames[CountdownPage]
    app.show_page(CountdownPage)
    page.set_countdown(json_message["countdown"])
    info_session.session_has_started()

