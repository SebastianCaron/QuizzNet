import json
from src.session.session_infos import info_session 

def response_joker_used(message):
    try :
        json_message = json.loads(message)
    except:
        return "ERROR JSON"
    
    #TODO voir si pour chaque handler (d'ailleur rename en handler_x_y serait mieux je pense)
    # -- il faut check si status == 200 au cas où erreur 504 ou 400 ? ou forcément 200 ?

    for (key, value) in json_message["jokers"]:
        if key == "fifty":
            info_session.set_joker_fifty(value)
        else :
            info_session.set_joker_pass(value)
    
    #afficher message activé sur la page actuelle et mettre à jour l'affichage joker car maj

    if json_message["message"] == "joker activated" :
        #page qcm forcément, supprimer (ou barrer) les 2 options
        # du qcm qui ne sont PAS données dans le json_message["remainingAnswers"]
        return
    else :
        #pass la question, joueur a répondu bon pour le serveur, le joueur ne peut plus répondre
        # et attend la fin du tour de question
        return

