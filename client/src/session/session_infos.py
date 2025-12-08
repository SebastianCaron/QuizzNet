
#Will recieve and keep all the usefull informations given by the server
#About the session
#All these informations are kept in the server. This is used only for the
#graphic part of the client (display the pseudos, scores, etc)
#If you change sth here, it will not have any impact on the real situation

class InfoSession:
    def __init__(self):
        self.session_started = False
        self.pseudo = ""
        self.players_score = {} # dico pseudo:score
        self.joker_fifty = 0 #just info to display the button
        self.joker_pass = 0 #same
        self.lives = -1 #if -1 -> not displayed not used, classic mode
        self.is_creator = False

    def set_pseudo(self, pseudo):
        self.pseudo = pseudo

    def get_pseudo(self):
        return self.pseudo
    
    def get_players(self):
        return self.players_score.keys()
    
    def get_players_score(self, pseudo):
        return self.players_score[pseudo]
    
    def set_joker_fifty(self, nb_joker):
        self.joker_fifty = nb_joker

    def set_joker_pass(self, nb_joker):
        self.joker_pass = nb_joker

    def joker_fifty_available(self):
        return self.joker_fifty >= 0
    
    def joker_pass_available(self):
        return self.joker_pass >= 0
    
    def set_new_player(self, pseudo):
        self.players_score[pseudo] = 0

    def update_score(self, pseudo, points):
        self.players_score[pseudo] += points

    def use_joker_fifty(self):
        self.joker_fifty -= 1

    def use_joker_pass(self):
        self.joker_pass -= 1

    def set_nb_lives(self, nb_lives):
        self.lives = nb_lives
    
    def lost_a_life(self):
        self.lives -=1 

    def session_has_started(self):
        self.session_started = True

    def is_session_started(self):
        return self.session_started
    
    def unset_player(self, pseudo):
        if not self.session_started:
            del self.players_score[pseudo]
        else :
            self.update_score(pseudo, -1)

    def is_creator(self):
        return self.is_creator
    
    def set_creator(self):
        self.is_creator = True

info_session = InfoSession()