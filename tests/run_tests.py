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
from tests.server.tcp.tcp_server_tester import run_all_tcp_tests


def run_tests(local_mode: bool = False):
    print("=== Lancement des tests QuizNet ===")
    
    server = ServerMaker()
    
    if not local_mode:
        print("\n--- Lancement du server ---")
        if not server.start_server():
            print("❌ Impossible de démarrer le serveur")
            return False
    else:
        print("\n--- Mode local : serveur déjà en cours d'exécution (normalement) ---")
    
    print("\n🧪 Lancement des tests...")
    try:
        # Tests UDP
        print("\n--- Tests UDP ---")
        result_udp = run_all_udp_tests(server)

        # Tests TCP
        print("\n--- Tests TCP ---")
        result_tcp = run_all_tcp_tests(server)
        
        if not local_mode:
            print("\n--- Arret du server ---")
            server.stop_server()

        print(f"\n=== Résumé ===")
        print(f"Tests UDP: {'✅ Réussis' if result_udp else '❌ Échoués'}")
        print(f"Tests TCP: {'✅ Réussis' if result_tcp else '❌ Échoués'}")
        
        return result_udp and result_tcp
        
    except Exception as e:
        print(f"❌ Erreur lors de l'exécution des tests: {e}")
        if not local_mode:
            server.stop_server()
        return False

def main():
    parser = argparse.ArgumentParser(description='Lancer les tests QuizNet')
    parser.add_argument('--local', action='store_true', 
                        help='Mode local: ne pas compiler, lancer ou arrêter le serveur (serveur déjà en cours d\'exécution)')
    
    args = parser.parse_args()
    
    success = run_tests(local_mode=args.local)
    
    sys.exit(0 if success else 1)


if __name__ == "__main__":
    main()
