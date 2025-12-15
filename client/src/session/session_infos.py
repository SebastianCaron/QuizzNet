"""
This class serves as a local cache for the game session's state.
It stores information received from the server to update the GUI
(player list, scores, jokers, lives, etc.).

IMPORTANT:
The server holds the authoritative truth. Modification of data here 
only affects the local display and has no impact on the actual game logic 
enforced by the server.
"""

class InfoSession:
    def __init__(self):
        """Initializes the local session state with default values."""
        self.session_started = False
        self.pseudo = ""
        self.time_limit = 0
        self.players_score = {} # Dictionary 'pseudo' -> 'score'
        
        # Joker tracking
        self.joker_fifty_session = 0
        self.joker_pass_session = 0
        self.joker_fifty = 0 # Current available 50/50 jokers
        self.joker_pass = 0  # Current available Skip jokers
        
        # Battle Mode tracking
        self.lives_session = -1 # Initial lives count (-1 means Classic/Non-battle mode)
        self.lives = -1         # Current lives count
        
        self.nb_questions = 0
        self.is_creator = False

    def is_battle_mode(self):
        """Returns True if the game is in Battle mode ("lives" system active)."""
        return self.lives_session >= 0
    
    def set_time_limit(self, time):
        """Sets the time limit per question (in seconds)."""
        self.time_limit = time
    
    def get_time_limit(self):
        return self.time_limit
    
    def set_pseudo(self, pseudo):
        """Sets the local player's username."""
        self.pseudo = pseudo

    def get_pseudo(self):
        return self.pseudo
    
    def get_players(self):
        """Returns a list of all player usernames in the session."""
        return self.players_score.keys()
    
    def get_players_score(self, pseudo):
        """Returns the score of a specific player."""
        return self.players_score[pseudo]
    
    def set_joker_fifty(self, nb_joker):
        """Sets the initial number of 50/50 jokers."""
        self.joker_fifty = nb_joker
        self.joker_fifty_session = nb_joker

    def set_joker_pass(self, nb_joker):
        """Sets the initial number of Skip jokers."""
        self.joker_pass = nb_joker
        self.joker_pass_session = nb_joker

    def joker_fifty_available(self):
        """Checks if the player still has 50/50 jokers left."""
        return self.joker_fifty > 0
    
    def joker_pass_available(self):
        """Checks if the player still has Skip jokers left."""
        return self.joker_pass > 0
    
    def set_new_player(self, pseudo):
        """Adds a new player to the local scoreboard with a score of 0."""
        self.players_score[pseudo] = 0

    def update_score(self, pseudo, points):
        """Updates the score of a specific player based on server data."""
        self.players_score[pseudo] = points

    def use_joker_fifty(self):
        """Decrements the local count of 50/50 jokers."""
        self.joker_fifty -= 1

    def use_joker_pass(self):
        """Decrements the local count of Skip jokers."""
        self.joker_pass -= 1

    def set_nb_lives(self, nb_lives):
        """Initializes the lives for Battle mode (-1 disables the mode)."""
        self.lives = nb_lives
        self.lives_session = nb_lives

    def set_lives_game(self, nb_lives):
        """Updates the current remaining lives during the game."""
        self.lives = nb_lives

    def get_nb_lives(self):
        return self.lives
    
    def is_eliminated(self):
        """Returns True if the player has 0 lives remaining."""
        return self.lives == 0

    def get_nb_questions(self):
        return self.nb_questions
    
    def set_nb_questions(self, nb_questions):
        self.nb_questions = nb_questions

    def session_has_started(self):
        """Marks the session as active."""
        self.session_started = True

    def is_session_started(self):
        return self.session_started
    
    def unset_player(self, pseudo):
        """
        Handles player disconnection.
        - If game hasn't started: remove from list.
        - If game is running: mark as inactive (score -1).
        """
        if not self.session_started:
            del self.players_score[pseudo]
        else :
            self.update_score(pseudo, -1)

    def is_creator(self):
        """Checks if the local player is the session host."""
        return self.is_creator
    
    def set_creator(self):
        """Grants host statut (e.g., start button) to the local player."""
        self.is_creator = True

    def reset_for_new_game(self):
        """Resets all local game data to prepare for a new session."""
        self.players_score = {}
        self.joker_fifty = 0
        self.joker_pass = 0
        self.lives = -1
        self.lives_session = -1
        self.session_started = False
        self.is_creator = False


# Global instance used throughout the client application
info_session = InfoSession()
