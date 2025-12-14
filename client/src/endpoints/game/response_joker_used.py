import json
from gui.windows.game_questions import QuestionPage

def response_joker_used(message, app):
    """
    Handles the response to a 'POST joker/use' request.

    This function is triggered when the server confirms the activation of a joker.
    It is particularly important for the '50/50' joker, as the server sends back
    the list of remaining answers to display.

    :param message: The JSON body containing the joker status and data.
    :param app: The main Tkinter application instance.
    """

    try :
        json_message = json.loads(message)
    except:
        return "ERROR JSON"
    
    # Check if the server confirmed the activation
    if json_message["message"] == "joker activated" :
        # Retrieve the current question page
        page = app.frames[QuestionPage]
        
        # Update the UI to remove incorrect answers (for 50/50 joker)
        page.update_question_joker(json_message["remainingAnswers"])
    
    else :
        # Case: Joker failed or another type of joker response that requires no UI update
        pass