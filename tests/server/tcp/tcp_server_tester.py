from tests.server.utils.server import ServerMaker
from tests.server.tcp.test_player_register import PlayerRegisterTester
from tests.server.tcp.test_player_login import PlayerLoginTester
from tests.server.tcp.test_invalid_endpoint import InvalidEndpointTester


def run_all_tcp_tests(server_class: ServerMaker):
    """Lance tous les tests TCP"""
    print("=== Tests du serveur TCP QuizNet ===")
    tests_passed = 0
    total_tests = 5
    
    # Tests d'enregistrement
    register_tester = PlayerRegisterTester(server_class)
    
    print("\n1. Test d'enregistrement de joueur (succès - 201)...")
    if register_tester.test_player_register_success():
        print("✅ Test d'enregistrement réussi")
        tests_passed += 1
    else:
        print("❌ Test d'enregistrement échoué")
    
    print("\n2. Test d'enregistrement avec pseudo déjà pris (409)...")
    if register_tester.test_player_register_duplicate():
        print("✅ Test de duplication réussi")
        tests_passed += 1
    else:
        print("❌ Test de duplication échoué")
    
    # Test d'endpoint invalide
    invalid_endpoint_tester = InvalidEndpointTester(server_class)
    
    print("\n3. Test d'endpoint inexistant (400)...")
    if invalid_endpoint_tester.test_invalid_endpoint():
        print("✅ Test d'endpoint invalide réussi")
        tests_passed += 1
    else:
        print("❌ Test d'endpoint invalide échoué")
    
    # Tests de connexion
    login_tester = PlayerLoginTester(server_class)
    
    print("\n4. Test de connexion de joueur (succès - 200)...")
    if login_tester.test_player_login_success():
        print("✅ Test de connexion réussi")
        tests_passed += 1
    else:
        print("❌ Test de connexion échoué")
    
    print("\n5. Test de connexion avec mauvais identifiants (401)...")
    if login_tester.test_player_login_invalid_credentials():
        print("✅ Test de connexion échouée réussi")
        tests_passed += 1
    else:
        print("❌ Test de connexion échouée échoué")
    
    print(f"\n=== Résultats: {tests_passed}/{total_tests} tests réussis ===")
    return tests_passed >= total_tests

