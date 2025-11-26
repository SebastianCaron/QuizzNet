import json
from src.session.session_infos import info_session 

def response_player_eliminated(message):
    try :
        json_message = json.loads(message)
    except:
        return "ERROR JSON"
    
    info_session.update_score(json_message["pseudo"], -1)
    #Ici, ce sera la fenetre de l'après question, montrant les résultats tempo. Mettre à jour la page