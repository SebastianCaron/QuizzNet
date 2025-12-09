import json
#GAME
from src.endpoints.game.respone_themes_list import response_themes_list
from src.endpoints.game.response_question_new import response_question_new
from src.endpoints.game.response_joker_used import response_joker_used
from src.endpoints.game.response_question_answered import response_question_answered
from src.endpoints.game.response_player_eliminated import response_player_eliminated

# PLAYER
from src.endpoints.player.response_player_login import response_player_login
from src.endpoints.player.response_player_register import response_player_register

# SESSION
from src.endpoints.session.response_session_list import response_session_list
from src.endpoints.session.response_session_create import response_session_create
from src.endpoints.session.response_session_join import response_session_join
from src.endpoints.session.response_session_player_joined import response_session_player_join
from src.endpoints.session.response_session_started import response_session_started


def message_route(message, app):
    print("------PASSE PAR LA DEBUT MESSAGE : \n", message, "\nFIN MESSAGE------\n")
    
    if message.startswith('POST question/results'):
        response_question_new("\n".join(message.splitlines()[1:]), app)
    if message.startswith('POST session/player/eliminated'):
        response_player_eliminated("\n".join(message.splitlines()[1:]), app)
    if message.startswith('POST session/finished'):
        return
    if message.startswith('POST session/player/left'):
        return
    if message.startswith('POST session/started'):
        response_session_started("\n".join(message.splitlines()[1:]), app)
    if message.startswith('POST question/new'):
        response_question_new("\n".join(message.splitlines()[1:]), app)
    if message.startswith('POST session/player/joined'):
        response_session_player_join("\n".join(message.splitlines()[1:]), app)
    if message.startswith('POST'):
        return "MESSAGE INCONNU AU BATAILLON"
    
    #Sinon, à priori c'est du json
    try :
        json_message = json.loads(message)
    except:
        return "ERROR JSON"
    
    if json_message["message"] == "Bad request" or json_message["message"] == "Unknown Error" :
        print("Message d'erreur")
        return
    
    match json_message["action"]:
        case "player/register":
            response_player_register(message, app)
        case "player/login":
            response_player_login(message, app)
        case "themes/list":
            response_themes_list(message, app)
        case "sessions/list":
            response_session_list(message, app)
        case "session/create":
            response_session_create(message, app)
        case "session/join":
            response_session_join(message, app)
        case "joker/use":
            response_joker_used(message, app)
        case "question/answer":
            response_question_answered(message, app)
        case _:
            print("INCONNU")
