import json


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
        return
    if message.startswith('POST question/new'):
        return
    if message.startswith('POST'):
        return "MESSAGE INCONNU AU BATAILLON"
    
    #Sinon, à priori c'est du json
    try :
        json_message = json.loads(message)
    except:
        return "ERROR JSON"
    
    match json_message["action"]:
        case "player/register":
            return
        case "player/login":
            return
        case "themes/list":
            return
        case "sessions/list":
            return
        case "session/create":
            return
        case "session/join":
            return
        case "joker/use":
            return
        case "question/answer":
            return
        case _:
             return "INCONNU"
