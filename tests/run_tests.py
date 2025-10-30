#!/usr/bin/env python3
"""
Script de lancement des tests pour QuizNet
export PYTHONPATH=$PYTHONPATH:`pwd`
"""

import subprocess
import sys
import os

from tests.server.udp.udp_server_tester import run_all_udp_tests


def run_tests():
    print("=== Lancement des tests QuizNet ===")
    
    print("\n🧪 Lancement des tests...")
    try:
        # Tests UDP
        print("\n--- Tests UDP ---")
        result_udp = run_all_udp_tests()

        # Tests TCP
        
        print(f"\n=== Résumé ===")
        print(f"Tests UDP: {'✅ Réussis' if result_udp else '❌ Échoués'}")
        
        return result_udp
        
    except Exception as e:
        print(f"❌ Erreur lors de l'exécution des tests: {e}")
        return False

def main():
    success = run_tests()
    
    sys.exit(0 if success else 1)


if __name__ == "__main__":
    main()
