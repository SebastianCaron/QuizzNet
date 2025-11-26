import json

def response_player_register(message, app):
    try :
        json_message = json.loads(message)
    except:
        return "ERROR JSON"
    #afficher message de la réponse 2 puis wait 2sec, sans changer niveau graphique
    if json_message["statut"]=="201":
        app.tcp_client.send("GET sessions/list")
    return
    