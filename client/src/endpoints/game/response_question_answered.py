import json

def response_question_answered(message, app):
    """
    Handles the response to a 'POST question/answer' request.

    This function is triggered after the player submits an answer.
    Currently, no specific action is taken here because the UI is updated 
    immediately when the user clicks the answer button 
    
    :param message: The JSON response from the server.
    :param app: The main Tkinter application instance.
    """
    try :
        json_message = json.loads(message)
    except:
        return "ERROR JSON"
    
    # No further action needed.
    pass