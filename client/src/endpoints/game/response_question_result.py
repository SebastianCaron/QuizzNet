import json
from gui.windows.game_result_question import ResultsPage
from src.session.session_infos import info_session 


def response_question_new(message, app):
    try :
        json_message = json.loads(message)
    except:
        return "ERROR JSON"
    
    for player in json_message["results"]:
        info_session.update_score(player["pseudo"], player["totalScore"])
    page = app.frames[ResultsPage]
    page.set_result(json_message["correctAnswer"], json_message["explanation"], json_message["results"])
    app.show_page(ResultsPage)