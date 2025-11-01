from tests.server.utils.server import ServerMaker
from tests.server.tcp.test_player_register import PlayerRegisterTester
from tests.server.tcp.test_player_login import PlayerLoginTester
from tests.server.tcp.test_invalid_endpoint import InvalidEndpointTester


def run_register_tests(server_class: ServerMaker):
    print("=== Tests d'enregistrement de joueur ===")
    register_tester = PlayerRegisterTester(server_class)
    tests_passed = 0
    total_tests = 2
    
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
    
    print(f"\n=== Résultats: {tests_passed}/{total_tests} tests réussis ===")
    return tests_passed == total_tests


def run_login_tests(server_class: ServerMaker):
    print("=== Tests de connexion de joueur ===")
    login_tester = PlayerLoginTester(server_class)
    tests_passed = 0
    total_tests = 2
    
    print("\n1. Test de connexion de joueur (succès - 200)...")
    if login_tester.test_player_login_success():
        print("✅ Test de connexion réussi")
        tests_passed += 1
    else:
        print("❌ Test de connexion échoué")
    
    print("\n2. Test de connexion avec mauvais identifiants (401)...")
    if login_tester.test_player_login_invalid_credentials():
        print("✅ Test de connexion échouée réussi")
        tests_passed += 1
    else:
        print("❌ Test de connexion échouée échoué")
    
    print(f"\n=== Résultats: {tests_passed}/{total_tests} tests réussis ===")
    return tests_passed == total_tests


def run_invalid_endpoint_tests(server_class: ServerMaker):
    print("=== Tests d'endpoints invalides ===")
    invalid_endpoint_tester = InvalidEndpointTester(server_class)
    tests_passed = 0
    total_tests = 1
    
    print("\n1. Test d'endpoint inexistant (400)...")
    if invalid_endpoint_tester.test_invalid_endpoint():
        print("✅ Test d'endpoint invalide réussi")
        tests_passed += 1
    else:
        print("❌ Test d'endpoint invalide échoué")
    
    print(f"\n=== Résultats: {tests_passed}/{total_tests} tests réussis ===")
    return tests_passed == total_tests


def run_all_tcp_tests(server_class: ServerMaker):
    print("=== Tests du serveur TCP QuizNet ===")
    all_passed = True
    
    result_register = run_register_tests(server_class)
    all_passed = all_passed and result_register
    
    result_invalid = run_invalid_endpoint_tests(server_class)
    all_passed = all_passed and result_invalid
    
    result_login = run_login_tests(server_class)
    all_passed = all_passed and result_login
    
    return all_passed

