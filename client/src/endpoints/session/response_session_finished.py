import json
from gui.windows.end_game import EndGamePage

def response_session_finished(message, app):
    try :
        json_message = json.loads(message)
    except:
        print("ERREUR JSON")
        return "ERROR JSON"
    
    mode = json_message["mode"]
    ranking = json_message["ranking"]
    if mode == "battle" :
        winner = json_message["winner"]
    else :
        winner = ""

    page = app.frames[EndGamePage]
    page.set_ranking(mode, ranking, winner)
    app.show_page(EndGamePage)