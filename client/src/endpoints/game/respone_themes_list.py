import json
from gui.windows.create_session import SessionCreatePage

def response_themes_list(message, app):
    """
    Handles the response to a 'GET themes/list' request.

    This function is triggered when the user wants to create a session.
    It receives the list of available quiz themes from the server,
    populates the selection menu in the Create Session page, and navigates there.

    :param message: The JSON body containing the list of themes.
    :param app: The main Tkinter application instance.
    """
    try :
        json_message = json.loads(message)
    except:
        return "ERROR JSON"
    
    # Check if the server returned any themes
    if json_message["nbThemes"] == 0:
        # Case: No themes available. Initialize UI with an empty list.
        page = app.frames[SessionCreatePage]
        page.update_theme_list([])
    else:
        # Case: Themes found. Extract them and populate the UI.
        liste_theme = json_message["themes"]
        page = app.frames[SessionCreatePage]
        page.update_theme_list(liste_theme)

    # Once the data is loaded, switch the view to the Create Session page
    app.show_page(SessionCreatePage)