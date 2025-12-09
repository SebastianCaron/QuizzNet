import json
from gui.windows.game_questions import QuestionPage

def response_joker_used(message, app):
    try :
        json_message = json.loads(message)
    except:
        return "ERROR JSON"
    
    if json_message["message"] == "joker activated" :
        page = app.frames[QuestionPage]
        page.update_question_joker(json_message["remainingAnswers"])
        #app.show_page(QuestionPage)
    
    else :
        pass #TODO voir si truc à faire

