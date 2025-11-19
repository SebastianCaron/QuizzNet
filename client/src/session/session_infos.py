
#Will recieve and keep all the usefull informations given by the server
#About the session
#All these informations are kept in the server. This is used only for the
#graphic part of the client (display the pseudos, scores, etc)
#If you change sth here, it will not have any impact on the real situation

class InfoSession:
    def __init__(self):
        self.session_started = False
        self.players_score = {} # dico pseudo:score, First is the client player
        self.joker_fifty = True #just info to display the button
        self.joker_pass = True #same
        self.lives = -1 #if -1 -> not displayed not used, classic mode

    def get_players_score(self, pseudo):
        return self.players_score[pseudo]
    
    def get_joker_fifty(self):
        return self.joker_fifty
    
    def get_joker_pass(self):
        return self.joker_pass
    
    def set_new_player(self, pseudo):
        self.players_score[pseudo] = 0

    def update_score(self, pseudo, points):
        self.players_score[pseudo] += points

    def use_joker_fifty(self):
        self.joker_fifty = False

    def use_joker_pass(self):
        self.joker_pass = False

    def set_nb_lives(self, nb_lives):
        self.lives = nb_lives
    
    def lost_a_life(self):
        self.lives -=1 

    def session_started(self):
        self.session_started = True

    def is_session_started(self):
        return self.session_started
    
info_session = InfoSession()