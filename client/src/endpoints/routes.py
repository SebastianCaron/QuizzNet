import json

# GAME HANDLERS
from src.endpoints.game.respone_themes_list import response_themes_list
from src.endpoints.game.response_question_new import response_question_new
from src.endpoints.game.response_question_result import response_question_result
from src.endpoints.game.response_joker_used import response_joker_used
from src.endpoints.game.response_question_answered import response_question_answered
from src.endpoints.game.response_player_eliminated import response_player_eliminated

# PLAYER HANDLERS
from src.endpoints.player.response_player_login import response_player_login
from src.endpoints.player.response_player_register import response_player_register

# SESSION HANDLERS
from src.endpoints.session.response_session_list import response_session_list
from src.endpoints.session.response_session_create import response_session_create
from src.endpoints.session.response_session_join import response_session_join
from src.endpoints.session.response_session_player_joined import response_session_player_join
from src.endpoints.session.response_session_player_left import response_session_player_left
from src.endpoints.session.response_session_started import response_session_started
from src.endpoints.session.response_session_finished import response_session_finished


def message_route(message, app):
    """
    Central dispatcher for all incoming server messages.
    
    It analyzes the content of the message (Header or JSON 'action')
    and calls the appropriate function to update the GUI.

    :param message: The raw string message received from the TCP client.
    :param app: The main Tkinter application instance.
    """
    
    # These messages start with a specific header followed by a body.
    # We strip the first line (header) and pass the body to the handler.
    
    if message.startswith('POST question/results'):
        response_question_result("\n".join(message.splitlines()[1:]), app)
    if message.startswith('POST session/player/eliminated'):
        response_player_eliminated("\n".join(message.splitlines()[1:]), app)
    if message.startswith('POST session/finished'):
        response_session_finished("\n".join(message.splitlines()[1:]), app)
    if message.startswith('POST session/player/left'):
        response_session_player_left("\n".join(message.splitlines()[1:]), app)
    if message.startswith('POST session/started'):
        response_session_started("\n".join(message.splitlines()[1:]), app)
    if message.startswith('POST question/new'):
        response_question_new("\n".join(message.splitlines()[1:]), app)
    if message.startswith('POST session/player/joined'):
        response_session_player_join("\n".join(message.splitlines()[1:]), app)
    
    # If it starts with POST but wasn't caught above, it's an unknown command
    if message.startswith('POST'):
        return "MESSAGE INCONNU AU BATAILLON"
    
    # These messages are usually direct responses to a client request.
    # They are pure JSON objects containing an "action" field.

    try :
        json_message = json.loads(message)
    except:
        return "ERROR JSON"
    
    # Check for global server errors - no need to do anything 
    if json_message["message"] == "Bad request" or json_message["message"] == "Unknown Error" :
        print("Error global message recieved from serveur")
        return
    
    # Route based on the 'action' field in the JSON
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
        case "session/start":
            return #Only errors that does not have to be handled
        case _:
            print("Route unknown")