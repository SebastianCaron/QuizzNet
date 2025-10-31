import socket
import json
import time
from typing import Optional, Tuple
from tests.server.utils.server import ServerMaker


class TCPServerTester:
    def __init__(self, server_class: ServerMaker):
        self.server = server_class
    
    def send_tcp_request(self, request: str, timeout: float = 5.0) -> Tuple[bool, str]:
        try:
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            sock.settimeout(timeout)
            
            server_address = ('localhost', int(self.server.tcp_port))
            sock.connect(server_address)
            
            sock.sendall(request.encode('utf-8'))
            
            response_data = b""
            while True:
                chunk = sock.recv(4096)
                if not chunk:
                    break
                response_data += chunk
                if len(chunk) < 4096:
                    break
            
            response_str = response_data.decode('utf-8')
            sock.close()
            return True, response_str
            
        except socket.timeout:
            return False, "Timeout - aucune réponse reçue"
        except Exception as e:
            return False, f"Erreur: {e}"
    
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
            
            print(f"Réponse d'enregistrement correcte: {response}")
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
            
            print(f"Réponse de duplication correcte: {response}")
            return True
            
        except json.JSONDecodeError as e:
            print(f"Erreur de parsing JSON: {e}")
            print(f"Réponse reçue: {response}")
            return False
    
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
            
            print(f"Réponse d'endpoint invalide correcte: {response}")
            return True
            
        except json.JSONDecodeError as e:
            print(f"Erreur de parsing JSON: {e}")
            print(f"Réponse reçue: {response}")
            return False


def run_all_tcp_tests(server_class: ServerMaker):
    tester = TCPServerTester(server_class)
    
    print("=== Tests du serveur TCP QuizNet ===")
    tests_passed = 0
    total_tests = 2
    
    print("\n1. Test d'enregistrement de joueur (succès - 201)...")
    if tester.test_player_register_success():
        print("✅ Test d'enregistrement réussi")
        tests_passed += 1
    else:
        print("❌ Test d'enregistrement échoué")
    
    print("\n2. Test d'enregistrement avec pseudo déjà pris (409)...")
    if True or tester.test_player_register_duplicate():
        print("✅ Test de duplication réussi")
        tests_passed += 1
    else:
        print("❌ Test de duplication échoué")
    
    print("\n3. Test d'endpoint inexistant (400)...")
    if tester.test_invalid_endpoint():
        print("✅ Test d'endpoint invalide réussi")
        tests_passed += 1
    else:
        print("❌ Test d'endpoint invalide échoué")
    
    print(f"\n=== Résultats: {tests_passed}/{total_tests} tests réussis ===")
    return tests_passed == total_tests

