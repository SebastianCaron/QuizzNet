import json
#GAME
from game.respone_themes_list import response_themes_list

# PLAYER
from player.response_player_login import response_player_login
from player.response_player_register import response_player_register

# SESSION
from session.response_session_list import response_session_list
from session.response_session_create import response_session_create
from session.response_session_join import response_session_join
from session.response_session_player_joined import response_session_player_join
from session.response_session_started import response_session_started


def message_route(message):
    if message.startswith('POST question/results'):
        return
    if message.startswith('POST session/player/eliminated'):
        return
    if message.startswith('POST session/finished'):
        return
    if message.startswith('POST session/player/left'):
        return
    if message.startswith('POST session/started'):
        response_session_started(message)
    if message.startswith('POST question/new'):
        return
    if message.startswith('POST session/player/joined'):
        response_session_player_join(message)
    if message.startswith('POST'):
        return "MESSAGE INCONNU AU BATAILLON"
    
    #Sinon, à priori c'est du json
    try :
        json_message = json.loads(message)
    except:
        return "ERROR JSON"
    
    match json_message["action"]:
        case "player/register":
            response_player_register(message)
        case "player/login":
            response_player_login(message)
        case "themes/list":
            response_themes_list(message)
        case "sessions/list":
            response_session_list(message)
        case "session/create":
            response_session_create(message)
        case "session/join":
            response_session_join(message)
        case "joker/use":
            return
        case "question/answer":
            return
        case _:
             return "INCONNU"
