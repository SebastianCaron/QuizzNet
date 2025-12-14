import json
from gui.windows.game_result_question import ResultsPage
from gui.windows.game_questions import QuestionPage
from src.session.session_infos import info_session 


def response_question_result(message, app):
    """
    Handles the 'POST question/results' notification from the server.

    This function is triggered after all players have answered (or time ran out).
    It processes the results for the current round, updates player scores and lives
    (in Battle Mode), and displays the Results Page with the correct answer.

    :param message: The JSON body containing round results and explanations.
    :param app: The main Tkinter application instance.
    """
    try :
        json_message = json.loads(message)
    except:
        return "ERROR JSON"
    
    # Ensure the timer on the Question Page is stopped to prevent background ticking
    page_quest = app.frames[QuestionPage]
    page_quest.stop_timer()
    
    # Update local game state based on the received results
    if info_session.is_battle_mode():
        # Battle Mode: Update scores AND lives
        for player in json_message["results"]:
            info_session.update_score(player["pseudo"], player["totalScore"])
            
            # If this entry corresponds to the local player, update their lives count
            if player["pseudo"] == info_session.pseudo :
                info_session.set_lives_game(player["lives"])

    else:
        # Classic Mode: Only update scores
        for player in json_message["results"]:
            info_session.update_score(player["pseudo"], player["totalScore"])

    # Configure and display the Results Page
    page = app.frames[ResultsPage]
    page.set_result(
        json_message["correctAnswer"], 
        json_message["explanation"], 
        json_message["results"]
    )
    app.show_page(ResultsPage)