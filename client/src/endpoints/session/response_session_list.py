import json
from gui.windows.liste_session import SessionListPage

def response_session_list(message, app):
    """
    Handles the response to a 'GET session/list' request.

    It parses the list of available game sessions sent by the server,
    updates the Session List Page UI with this data, and navigates 
    the user to that page.

    :param message: The JSON body containing the list of sessions.
    :param app: The main Tkinter application instance.
    """
    try :
        json_message = json.loads(message)
    except:
        return "ERROR JSON"
    
    # Check if there are any active sessions
    if json_message["nbSessions"] == 0:
        # Case: No sessions found. Update UI with an empty list.
        page = app.frames[SessionListPage]
        page.update_sessions([])
    else:
        # Case: Sessions found. Extract data and populate the UI list.
        liste_session = json_message["sessions"]
        page = app.frames[SessionListPage]
        page.update_sessions(liste_session)

    # Switch the current view to the Session List page
    app.show_page(SessionListPage)