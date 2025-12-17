#!/usr/bin/env python3
"""
Test rapide des nouveaux correctifs
"""

import socket
import time

SERVER_HOST = "127.0.0.1"
SERVER_PORT = 6667
SERVER_PASS = "testpass"

def test_channel_validation():
    print("🧪 Test des validations de channels...")
    
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.settimeout(2)
    sock.connect((SERVER_HOST, SERVER_PORT))
    
    # Auth
    sock.send(f"PASS {SERVER_PASS}\r\n".encode())
    sock.send("NICK validator\r\n".encode())
    sock.send("USER validator 0 * :Validator\r\n".encode())
    time.sleep(0.2)
    sock.recv(4096)
    
    print("\n📋 Tests de validation:")
    
    tests = [
        ("#",           "❌ Devrait être refusé"),
        ("#chan nel",   "❌ Devrait être refusé (espace)"),
        ("#ch@nnel",    "❌ Devrait être refusé (@)"),
        ("##doublehash","❌ Devrait être refusé (##)"),
        ("#test",       "✅ Devrait être accepté"),
        ("#valid-123",  "✅ Devrait être accepté"),
    ]
    
    for channel, expected in tests:
        sock.send(f"JOIN {channel}\r\n".encode())
        time.sleep(0.2)
        response = sock.recv(4096).decode('utf-8', errors='ignore')
        
        if "403" in response or "Invalid" in response:
            result = "❌ REFUSÉ"
        elif "JOIN" in response:
            result = "✅ ACCEPTÉ"
        else:
            result = "⚠️  INCONNU"
        
        print(f"  {channel:20s} → {result:15s} ({expected})")
        
        if "JOIN" in response:
            # Quitter le channel
            sock.send(f"PART {channel}\r\n".encode())
            time.sleep(0.1)
            sock.recv(4096)
    
    sock.close()
    print("\n✅ Test de validation terminé\n")

if __name__ == "__main__":
    print("\n" + "="*60)
    print("TEST RAPIDE DES NOUVEAUX CORRECTIFS")
    print("="*60 + "\n")
    
    try:
        test_channel_validation()
        print("✅ Tous les tests ont été exécutés avec succès!")
    except Exception as e:
        print(f"❌ Erreur: {e}")
        print("\n⚠️  Assurez-vous que le serveur est démarré:")
        print("   ./ircserv 6667 testpass")
