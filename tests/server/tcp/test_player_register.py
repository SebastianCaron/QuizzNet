import json
import time
from tests.server.tcp.tcp_base import TCPServerTester


class PlayerRegisterTester(TCPServerTester):
    
    def test_player_register_success(self) -> bool:
        request = """POST player/register
{
    "pseudo":"Player1",
    "password":"mypassword123"
}"""
        
        success, response = self.send_tcp_request(request)
        
        if not success:
            print(f"Échec de la requête d'enregistrement: {response}")
            return False
        
        try:
            response_json = json.loads(response)
            
            if response_json.get("action") != "player/register":
                print(f"Action incorrecte. Attendu: 'player/register', Reçu: {response_json.get('action')}")
                return False
            
            if response_json.get("statut") != "201":
                print(f"Statut incorrect. Attendu: '201', Reçu: {response_json.get('statut')}")
                return False
            
            if response_json.get("message") != "player registered successfully":
                print(f"Message incorrect. Attendu: 'player registered successfully', Reçu: {response_json.get('message')}")
                return False
            
            return True
            
        except json.JSONDecodeError as e:
            print(f"Erreur de parsing JSON: {e}")
            print(f"Réponse reçue: {response}")
            return False
    
    def test_player_register_duplicate(self) -> bool:
        first_request = """POST player/register
{
    "pseudo":"Player2",
    "password":"mypassword123"
}"""
        
        success, _ = self.send_tcp_request(first_request)
        if not success:
            print("Échec du premier enregistrement nécessaire pour le test de duplication")
            return False
        
        time.sleep(0.5)
        
        duplicate_request = """POST player/register
{
    "pseudo":"Player2",
    "password":"mypassword123"
}"""
        
        success, response = self.send_tcp_request(duplicate_request)
        
        if not success:
            print(f"Échec de la requête de duplication: {response}")
            return False
        
        try:
            response_json = json.loads(response)
            
            if response_json.get("statut") != "409":
                print(f"Statut incorrect. Attendu: '409', Reçu: {response_json.get('statut')}")
                print(f"Réponse complète: {response}")
                return False
            
            if response_json.get("message") != "pseudo already exists":
                print(f"Message incorrect. Attendu: 'pseudo already exists', Reçu: {response_json.get('message')}")
                return False
            
            return True
            
        except json.JSONDecodeError as e:
            print(f"Erreur de parsing JSON: {e}")
            print(f"Réponse reçue: {response}")
            return False

