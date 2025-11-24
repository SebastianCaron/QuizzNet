import json
from src.session.session_infos import info_session 
#voir si info_session utile

def response_themes_list(message):
    try :
        json_message = json.loads(message)
    except:
        return "ERROR JSON"
    
    liste_theme = json_message["themes"]
    for theme in liste_theme : #à voir si on fait for each
        # ou si on va aller voir une autre fonction (qui va gérer les
        # choix lors de création session par exemple et l'affichage)
        return