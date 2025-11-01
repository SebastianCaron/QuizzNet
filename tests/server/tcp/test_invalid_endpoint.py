import json
from tests.server.tcp.tcp_base import TCPServerTester


class InvalidEndpointTester(TCPServerTester):
    def test_invalid_endpoint(self) -> bool:
        request = """POST player/nonexistent
{
    "pseudo":"Player1",
    "password":"mypassword123"
}"""
        
        success, response = self.send_tcp_request(request)
        
        if not success:
            print(f"Échec de la requête invalide: {response}")
            return False
        
        try:
            response_json = json.loads(response)
            
            if response_json.get("statut") != "400":
                print(f"Statut incorrect. Attendu: '400', Reçu: {response_json.get('statut')}")
                print(f"Réponse complète: {response}")
                return False
            
            if response_json.get("message") != "Bad request":
                print(f"Message incorrect. Attendu: 'Bad request', Reçu: {response_json.get('message')}")
                return False
            
            return True
            
        except json.JSONDecodeError as e:
            print(f"Erreur de parsing JSON: {e}")
            print(f"Réponse reçue: {response}")
            return False

