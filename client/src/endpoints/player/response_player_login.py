import json

def response_player_login(message):
    try :
        json_message = json.loads(message)
    except:
        return "ERROR JSON"
    #afficher message de la réponse 2 puis wait 2sec, sans changer niveau graphique
    if json_message["statut"]=="200":
        #passer niveau graphique au choix de session
        return
    return