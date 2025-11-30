import json
from gui.windows.create_session import SessionCreatePage

def response_themes_list(message, app):
    try :
        json_message = json.loads(message)
    except:
        return "ERROR JSON"
    
    if json_message["nbThemes"] == 0:
        page = app.frames[SessionCreatePage]
        page.update_theme_list([])
    else:
        liste_theme = json_message["themes"]
        page = app.frames[SessionCreatePage]
        page.update_theme_list(liste_theme)

    app.show_page(SessionCreatePage)
    