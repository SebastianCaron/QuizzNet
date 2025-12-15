import json
from src.session.session_infos import info_session 
from gui.windows.countdown import CountdownPage
from gui.windows.game_questions import QuestionPage

def response_session_started(message, app):
    """
    Handles the 'POST session/started' notification from the server.
    
    This function is triggered when the host starts the game. It:
    Prepares the Question Page with session settings (time limit, lives...).
    Switches the UI to the Countdown Page.
    Starts the visual countdown before the first question.
    
    :param message: The JSON body string containing the countdown duration.
    :param app: The main Tkinter application instance.
    """
    try :
        json_message = json.loads(message)
    except:
        return "ERROR JSON"
    
    # Pre-configure the Question Page so it's ready when the countdown ends
    page_question = app.frames[QuestionPage]
    page_question.set_timer_and_questions()
    
    # Get the Countdown Page instance and display it
    page = app.frames[CountdownPage]
    app.show_page(CountdownPage)
    
    # Start the visual countdown using the value sent by the server
    page.set_countdown(json_message["countdown"])
    
    # Mark the session as active in the local state
    info_session.session_has_started()

