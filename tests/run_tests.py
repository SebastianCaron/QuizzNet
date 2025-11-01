#!/usr/bin/env python3
"""
Script de lancement des tests pour QuizNet
export PYTHONPATH=$PYTHONPATH:`pwd`
"""

import subprocess
import sys
import os
import argparse

from tests.server.utils.server import ServerMaker
from tests.server.udp.udp_server_tester import run_all_udp_tests
from tests.server.tcp.tcp_server_tester import (
    run_all_tcp_tests,
    run_register_tests,
    run_login_tests,
    run_invalid_endpoint_tests
)


def run_tests(local_mode: bool = False, 
              udp: bool = False,
              register: bool = False,
              login: bool = False,
              invalid_ep: bool = False):
    print("=== Lancement des tests QuizNet ===")
    
    run_all = not (udp or register or login or invalid_ep)
    
    server = ServerMaker()
    
    if not local_mode:
        print("\n--- Lancement du server ---")
        if not server.start_server():
            print("❌ Impossible de démarrer le serveur")
            return False
    else:
        print("\n--- Mode local : serveur déjà en cours d'exécution (normalement) ---")
    
    print("\n🧪 Lancement des tests...")
    results = {}
    
    try:
        # Tests UDP
        if run_all or udp:
            print("\n--- Tests UDP ---")
            results['udp'] = run_all_udp_tests(server)
        else:
            results['udp'] = None

        # Tests TCP
        if run_all:
            print("\n--- Tests TCP (tous) ---")
            results['tcp_all'] = run_all_tcp_tests(server)
            results['register'] = None
            results['login'] = None
            results['invalid_ep'] = None
        else:
            results['tcp_all'] = None
            
            if register:
                print("\n--- Tests d'enregistrement ---")
                results['register'] = run_register_tests(server)
            else:
                results['register'] = None

            if login:
                print("\n--- Tests de connexion ---")
                results['login'] = run_login_tests(server)
            else:
                results['login'] = None

            if invalid_ep:
                print("\n--- Tests d'endpoints invalides ---")
                results['invalid_ep'] = run_invalid_endpoint_tests(server)
            else:
                results['invalid_ep'] = None
        
        if not local_mode:
            print("\n--- Arret du server ---")
            server.stop_server()

        print(f"\n=== Résumé ===")
        if results['udp'] is not None:
            print(f"Tests UDP: {'✅ Réussis' if results['udp'] else '❌ Échoués'}")
        if results['register'] is not None:
            print(f"Tests d'enregistrement: {'✅ Réussis' if results['register'] else '❌ Échoués'}")
        if results['login'] is not None:
            print(f"Tests de connexion: {'✅ Réussis' if results['login'] else '❌ Échoués'}")
        if results['invalid_ep'] is not None:
            print(f"Tests d'endpoints invalides: {'✅ Réussis' if results['invalid_ep'] else '❌ Échoués'}")
        if results['tcp_all'] is not None:
            print(f"Tests TCP (tous): {'✅ Réussis' if results['tcp_all'] else '❌ Échoués'}")
        
        all_success = True
        for key, value in results.items():
            if value is not None and not value:
                all_success = False
        
        return all_success
        
    except Exception as e:
        print(f"❌ Erreur lors de l'exécution des tests: {e}")
        if not local_mode:
            server.stop_server()
        return False

def main():
    parser = argparse.ArgumentParser(description='Lancer les tests QuizNet')
    parser.add_argument('--local', action='store_true', 
                        help='Mode local: ne pas compiler, lancer ou arrêter le serveur (serveur déjà en cours d\'exécution)')
    parser.add_argument('-udp', action='store_true',
                        help='Lancer uniquement les tests UDP')
    parser.add_argument('-register', action='store_true',
                        help='Lancer uniquement les tests d\'enregistrement de joueur')
    parser.add_argument('-login', action='store_true',
                        help='Lancer uniquement les tests de connexion de joueur')
    parser.add_argument('-invalid-ep', dest='invalid_ep', action='store_true',
                        help='Lancer uniquement les tests d\'endpoints invalides')
    
    args = parser.parse_args()
    
    success = run_tests(
        local_mode=args.local,
        udp=args.udp,
        register=args.register,
        login=args.login,
        invalid_ep=args.invalid_ep
    )
    
    sys.exit(0 if success else 1)


if __name__ == "__main__":
    main()
