import json

def response_session_list(message):
    try :
        json_message = json.loads(message)
    except:
        return "ERROR JSON"
    
    if json_message["nbSessions"] == 0 :
        #pas changer la fenêtre, mettre message "aucune session trouvée"
        return
    liste_session = json_message["sessions"]
    for session in liste_session : #à voir si on fait for each
        # ou si on va aller voir une autre fonction (qui va gérer les
        # choix et l'affichage)
    
        return