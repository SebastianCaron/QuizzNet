import json

def response_question_answered(message):
    try :
        json_message = json.loads(message)
    except:
        return "ERROR JSON"
    
    #afficher le json_message["message"] sur la fenêtre actuelle