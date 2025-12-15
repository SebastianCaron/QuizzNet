import json
from gui.windows.game_questions import QuestionPage

def response_question_new(message, app):
    """
    Handles the 'POST question/new' notification from the server.

    This function is triggered at the start of a new round. It:
    Parses the question data (text, type, answers).
    Loads this data into the QuestionPage UI.
    Navigates the user to the question screen to start answering.

    :param message: The JSON body containing the question details.
    :param app: The main Tkinter application instance.
    """

    try :
        json_message = json.loads(message)
    except:
        return "ERROR JSON"
    
    # Retrieve the Question Page instance
    page = app.frames[QuestionPage]

    # Load the question data into the UI
    # if QCM, it needs the list answers
    if json_message["type"] == "qcm" :
        page.load_question(json_message["type"], json_message["question"], json_message["answers"])
    else :
        # For boolean or text questions, we don't need a list of answer choices
        page.load_question(json_message["type"], json_message["question"], [])
    
    app.show_page(QuestionPage)