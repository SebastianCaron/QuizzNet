import json

def response_question_new(message):
    try :
        json_message = json.loads(message)
    except:
        return "ERROR JSON"
    
    #Fenetre différente affichée selon le type de question. L'envoie des réponses aux question au serveur se fera directement
    # dans les fonctions question_qcm, question_bool et question_text. 
    match json_message["type"]:
        case "qcm":
            # afficher fenetre qcm, en faire une fonction question_qcm avec :
            # int num_question = le numéro de la question
            # int nb_total = le nombre total de questions
            # string difficulty = la difficulté de la question
            # string question = l'énoncé de la question
            # tableau[string] de taille 4 list_answers = les 4 choix du qcm possibles
            return
        case "boolean":
            # afficher fenetre question vrai faux, en faire une fonction question_bool avec :
            # int num_question = le numéro de la question
            # int nb_total = le nombre total de questions
            # string difficulty = la difficulté de la question
            # string question = l'énoncé de la question
            return
        case "text":
            # afficher fenetre question input text, en faire une fonction question_text avec :
            # int num_question = le numéro de la question
            # int nb_total = le nombre total de questions
            # string difficulty = la difficulté de la question
            # string question = l'énoncé de la question
            return
        case _:
             return "INCONNU"