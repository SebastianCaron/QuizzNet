import json
from gui.windows.game_result_question import ResultsPage
from gui.windows.game_questions import QuestionPage
from src.session.session_infos import info_session 


def response_question_result(message, app):
    print("--- PASSE PAR QUESTION RESULT ---")
    try :
        json_message = json.loads(message)
    except:
        return "ERROR JSON"
    
    page_quest = app.frames[QuestionPage]
    page_quest.stop_timer()
    
    for player in json_message["results"]:
        info_session.update_score(player["pseudo"], player["totalScore"])
    page = app.frames[ResultsPage]
    page.set_result(json_message["correctAnswer"], json_message["explanation"], json_message["results"])
    app.show_page(ResultsPage)