import json
import time
from tests.server.tcp.tcp_base import TCPServerTester


class ThemesListTester(TCPServerTester):
    
    def test_themes_list(self) -> bool:
        register_request = """GET themes/list"""
        
        success, response = self.send_tcp_request(register_request)
        if not success:
            print("Échec de récupération des themes disponibles")
            return False
        
        try:
            response_json = json.loads(response)
            
            if response_json.get("action") != "themes/list":
                print(f"Action incorrecte. Attendu: 'themes/list', Reçu: {response_json.get('action')}")
                return False
            
            if response_json.get("statut") != "200":
                print(f"Statut incorrect. Attendu: '200', Reçu: {response_json.get('statut')}")
                print(f"Réponse complète: {response}")
                return False
            
            if response_json.get("message") != "ok":
                print(f"Message incorrect. Attendu: 'ok', Reçu: {response_json.get('message')}")
                return False
            
            return True
            
        except json.JSONDecodeError as e:
            print(f"Erreur de parsing JSON: {e}")
            print(f"Réponse reçue: {response}")
            return False
    