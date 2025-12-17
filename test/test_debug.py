#!/usr/bin/env python3
"""Test debug pour isoler le problème du test #10-11"""

import socket
import time

SERVER_HOST = "127.0.0.1"
SERVER_PORT = 6667
SERVER_PASS = "testpass"

class IRCClient:
    def __init__(self, name):
        self.name = name
        self.sock = None
        
    def connect(self):
        try:
            self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.sock.settimeout(5)
            self.sock.connect((SERVER_HOST, SERVER_PORT))
            print(f"[{self.name}] ✓ Connecté")
            return True
        except Exception as e:
            print(f"[{self.name}] ✗ Erreur connexion: {e}")
            return False
    
    def send(self, msg):
        try:
            self.sock.send((msg + "\r\n").encode())
            print(f"[{self.name}] → {msg}")
        except Exception as e:
            print(f"[{self.name}] ✗ Erreur envoi: {e}")
    
    def recv(self):
        try:
            data = self.sock.recv(4096).decode('utf-8', errors='ignore')
            if data:
                print(f"[{self.name}] ← {data[:200]}")
            return data
        except Exception as e:
            print(f"[{self.name}] ✗ Erreur réception: {e}")
            return ""
    
    def close(self):
        if self.sock:
            print(f"[{self.name}] Fermeture connexion...")
            self.sock.close()

print("=" * 80)
print("TEST #10: Channel protégé par mot de passe")
print("=" * 80)

# Client 1 crée le channel
client1 = IRCClient("creator")
if not client1.connect():
    exit(1)

client1.send(f"PASS {SERVER_PASS}")
client1.send("NICK creator")
client1.send("USER creator 0 * :Creator")
client1.recv()

client1.send("JOIN #keychan")
client1.recv()
client1.send("MODE #keychan +k secret123")
client1.recv()

# Client 2 tente de rejoindre
client2 = IRCClient("joiner1")
if not client2.connect():
    client1.close()
    exit(1)

client2.send(f"PASS {SERVER_PASS}")
client2.send("NICK joiner1")
client2.send("USER joiner1 0 * :Joiner One")
client2.recv()

client2.send("JOIN #keychan")
client2.recv()

client2.send("JOIN #keychan wrongkey")
client2.recv()

client2.send("JOIN #keychan secret123")
client2.recv()

print("\n>>> Fermeture des clients...")
client1.close()
time.sleep(0.5)
print(">>> Client 1 fermé")

client2.close()
time.sleep(0.5)
print(">>> Client 2 fermé")

print("\n>>> Attente 2 secondes...")
time.sleep(2)

print("\n" + "=" * 80)
print("TEST #11: Tentative de connexion après test #10")
print("=" * 80)

# Client 3 tente de se connecter
client3 = IRCClient("limitop")
if not client3.connect():
    print("✗ CRASH DÉTECTÉ: Impossible de se connecter")
    exit(1)

print("✓ Connexion réussie après test #10!")

client3.send(f"PASS {SERVER_PASS}")
client3.send("NICK limitop")
client3.send("USER limitop 0 * :Limit Op")
client3.recv()

client3.close()

print("\n✓ Tests terminés avec succès!")
