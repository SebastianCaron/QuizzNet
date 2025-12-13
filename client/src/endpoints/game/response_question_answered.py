import json

def response_question_answered(message, app):
    print("--- PASSE PAR QUESTION ANSWERED ---")

    try :
        json_message = json.loads(message)
    except:
        return "ERROR JSON"
    
    pass