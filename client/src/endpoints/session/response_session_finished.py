import json
from gui.windows.end_game import EndGamePage

def response_session_finished(message, app):
    """
    Handles the 'POST session/finished' notification from the server.

    This function is triggered when the game ends (either all questions are answered
    or only one player remains in Battle mode). It parses the final leaderboard 
    and displays the End Game screen.

    :param message: The JSON body containing rankings and winner info.
    :param app: The main Tkinter application instance.
    """
    try :
        json_message = json.loads(message)
    except:
        return "ERROR JSON"
    
    # Extract general game information
    mode = json_message["mode"]
    ranking = json_message["ranking"]

    # Extract specific Battle Mode information (Winner)
    if mode == "battle" :
        winner = json_message["winner"]
    else :
        winner = ""

    # Configure and display the final Scoreboard Page
    page = app.frames[EndGamePage]
    page.set_ranking(mode, ranking, winner)
    app.show_page(EndGamePage)