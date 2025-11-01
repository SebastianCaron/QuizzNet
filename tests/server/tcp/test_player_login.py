import json
import time
from tests.server.tcp.tcp_base import TCPServerTester


class PlayerLoginTester(TCPServerTester):
    """Tests pour l'endpoint player/login"""
    
    def test_player_login_success(self) -> bool:
        """Test de connexion de joueur avec succès (200)"""
        register_request = """POST player/register
{
    "pseudo":"LoginPlayer1",
    "password":"mypassword123"
}"""
        
        success, _ = self.send_tcp_request(register_request)
        if not success:
            print("Échec de l'enregistrement nécessaire pour le test de connexion")
            return False
        
        time.sleep(0.5)

        login_request = """POST player/login
{
    "pseudo":"LoginPlayer1",
    "password":"mypassword123"
}"""
        
        success, response = self.send_tcp_request(login_request)
        
        if not success:
            print(f"Échec de la requête de connexion: {response}")
            return False
        
        try:
            response_json = json.loads(response)
            
            if response_json.get("action") != "player/login":
                print(f"Action incorrecte. Attendu: 'player/login', Reçu: {response_json.get('action')}")
                return False
            
            if response_json.get("statut") != "200":
                print(f"Statut incorrect. Attendu: '200', Reçu: {response_json.get('statut')}")
                print(f"Réponse complète: {response}")
                return False
            
            if response_json.get("message") != "login successful":
                print(f"Message incorrect. Attendu: 'login successful', Reçu: {response_json.get('message')}")
                return False
            
            return True
            
        except json.JSONDecodeError as e:
            print(f"Erreur de parsing JSON: {e}")
            print(f"Réponse reçue: {response}")
            return False
    
    def test_player_login_invalid_credentials(self) -> bool:
        """Test de connexion avec mauvais identifiants (401)"""
        login_request = """POST player/login
{
    "pseudo":"NonexistentPlayer",
    "password":"wrongpassword"
}"""
        
        success, response = self.send_tcp_request(login_request)
        
        if not success:
            print(f"Échec de la requête de connexion: {response}")
            return False
        
        try:
            response_json = json.loads(response)
            
            if response_json.get("action") != "player/login":
                print(f"Action incorrecte. Attendu: 'player/login', Reçu: {response_json.get('action')}")
                return False
            
            if response_json.get("statut") != "401":
                print(f"Statut incorrect. Attendu: '401', Reçu: {response_json.get('statut')}")
                print(f"Réponse complète: {response}")
                return False
            
            if response_json.get("message") != "invalid credentials":
                print(f"Message incorrect. Attendu: 'invalid credentials', Reçu: {response_json.get('message')}")
                return False
            
            return True
            
        except json.JSONDecodeError as e:
            print(f"Erreur de parsing JSON: {e}")
            print(f"Réponse reçue: {response}")
            return False

