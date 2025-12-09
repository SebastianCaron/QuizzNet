import json

def response_question_answered(message):
    try :
        json_message = json.loads(message)
    except:
        return "ERROR JSON"
    
    pass #TODO voir si y'a un truc intéressant à faire ici