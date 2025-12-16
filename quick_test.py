#!/usr/bin/env python3
"""
Quick test - Test rapide pour vérifier que le serveur fonctionne
"""

import socket
import sys

SERVER_HOST = "127.0.0.1"
SERVER_PORT = 6667
SERVER_PASS = "testpass"

def quick_test():
    print("🧪 Test rapide de connexion...")
    
    try:
        # Connexion
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.settimeout(3)
        sock.connect((SERVER_HOST, SERVER_PORT))
        print("✓ Connexion établie")
        
        # Authentification
        sock.send(f"PASS {SERVER_PASS}\r\n".encode())
        sock.send("NICK quicktest\r\n".encode())
        sock.send("USER quicktest 0 * :Quick Test\r\n".encode())
        
        # Recevoir la réponse
        response = sock.recv(4096).decode('utf-8', errors='ignore')
        
        if "001" in response or "Welcome" in response:
            print("✓ Authentification réussie")
            print(f"\n📨 Réponse du serveur:\n{response}")
            
            # Test JOIN
            sock.send("JOIN #quicktest\r\n".encode())
            response = sock.recv(4096).decode('utf-8', errors='ignore')
            
            if "JOIN" in response:
                print("\n✓ JOIN réussi")
            
            # Test PRIVMSG
            sock.send("PRIVMSG #quicktest :Hello World!\r\n".encode())
            print("✓ PRIVMSG envoyé")
            
            # QUIT
            sock.send("QUIT :Goodbye\r\n".encode())
            
            print("\n✅ Tous les tests rapides réussis!")
            sock.close()
            return True
        else:
            print("✗ Authentification échouée")
            print(f"Réponse: {response}")
            sock.close()
            return False
            
    except Exception as e:
        print(f"✗ Erreur: {e}")
        return False

if __name__ == "__main__":
    print("\n" + "="*50)
    print("TEST RAPIDE FT_IRC")
    print("="*50 + "\n")
    
    if quick_test():
        sys.exit(0)
    else:
        print("\n⚠️  Assurez-vous que le serveur est démarré:")
        print("   ./ircserv 6667 testpass")
        sys.exit(1)
