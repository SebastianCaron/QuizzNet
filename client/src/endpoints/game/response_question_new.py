import json
from gui.windows.game_questions import QuestionPage

def response_question_new(message, app):
    try :
        json_message = json.loads(message)
    except:
        return "ERROR JSON"
    

    page = app.frames[QuestionPage]
    if json_message["type"] == "qcm" :
        page.load_question(json_message["type"], json_message["question"], json_message["answers"])
    else :
        page.load_question(json_message["type"], json_message["question"], [])
    app.show_page(QuestionPage)