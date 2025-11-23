#!/usr/bin/env python3
"""
Module utilitaire pour tester le serveur UDP QuizNet
"""

import socket
import threading
import time
import subprocess
import os
import signal
import sys
from typing import Optional, Tuple
from tests.server.utils.server import ServerMaker


class UDPServerTester:    
    def __init__(self, server_class: ServerMaker):
        self.discovery_message = "looking for quiznet servers"
        self.expected_response_prefix = "hello i'm a quiznet server:"
        self.server = server_class
    
    def send_discovery_request(self, timeout: float = 5.0) -> Tuple[bool, str]:
        try:
            sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
            sock.settimeout(timeout)
            
            server_address = ('localhost', self.server.udp_port)
            sock.sendto(self.discovery_message.encode('utf-8'), server_address)
            
            response, _ = sock.recvfrom(1024)
            response_str = response.decode('utf-8')
            
            sock.close()
            return True, response_str
            
        except socket.timeout:
            return False, "Timeout - aucune réponse reçue"
        except Exception as e:
            return False, f"Erreur: {e}"
    
    def test_discovery_response(self) -> bool:
        success, response = self.send_discovery_request()
        
        if not success:
            print(f"Échec de la requête de découverte: {response}")
            return False
        
        if not response.startswith(self.expected_response_prefix):
            print(f"Format de réponse incorrect. Attendu: {self.expected_response_prefix}, Reçu: {response}")
            return False
        
        expected_content = f"{self.server.server_name}:{self.server.tcp_port}"
        if expected_content not in response:
            print(f"Contenu de réponse incorrect. Attendu: {expected_content}, Reçu: {response}")
            return False
        
        print(f"Réponse de découverte correcte: {response}")
        return True
    
    def test_multiple_discovery_requests(self, count: int = 3) -> bool:
        print(f"Test de {count} requêtes de découverte consécutives...")
        
        for i in range(count):
            success, response = self.send_discovery_request()
            if not success:
                print(f"Échec de la requête {i+1}: {response}")
                return False
            
            if not self.test_discovery_response():
                print(f"Réponse incorrecte pour la requête {i+1}")
                return False
            
            print(f"Requête {i+1} réussie")
            time.sleep(0.5)
        
        return True
    
    def test_invalid_message(self) -> bool:
        try:
            sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
            sock.settimeout(2.0)
            
            invalid_message = "invalid message"
            server_address = ('localhost', self.server.udp_port)
            sock.sendto(invalid_message.encode('utf-8'), server_address)
            
            try:
                response, _ = sock.recvfrom(1024)
                print(f"Le serveur a répondu à un message invalide: {response.decode('utf-8')}")
                sock.close()
                return False
            except socket.timeout:
                print("Le serveur ignore correctement les messages invalides")
                sock.close()
                return True
                
        except Exception as e:
            print(f"Erreur lors du test de message invalide: {e}")
            return False


def run_all_udp_tests(server_class : ServerMaker):
    tester = UDPServerTester(server_class)
    
    print("=== Tests du serveur UDP QuizNet ===")
    tests_passed = 0
    total_tests = 4
    
    print("\n1. Test de la réponse de découverte...")
    if tester.test_discovery_response():
        print("🐙 Test de découverte réussi")
        tests_passed += 1
    else:
        print("❌ Test de découverte échoué")
    
    print("\n2. Test de requêtes multiples...")
    if tester.test_multiple_discovery_requests():
        print("🐙 Test de requêtes multiples réussi")
        tests_passed += 1
    else:
        print("❌ Test de requêtes multiples échoué")
    
    print("\n3. Test de message invalide...")
    if tester.test_invalid_message():
        print("🐙 Test de message invalide réussi")
        tests_passed += 1
    else:
        print("❌ Test de message invalide échoué")
    
    print("\n4. Test de disponibilité du port UDP...")
    if tester.send_discovery_request()[0]:
        print("🐙 Port UDP accessible")
        tests_passed += 1
    else:
        print("❌ Port UDP inaccessible")
    
    print(f"\n=== Résultats: {tests_passed}/{total_tests} tests réussis ===")
    return tests_passed == total_tests
