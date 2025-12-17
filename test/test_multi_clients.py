#!/usr/bin/env python3
"""
Test de stress avec plusieurs clients simultanés
Simule 5 clients IRC connectés en même temps
"""

import socket
import time
import threading
import sys

SERVER_HOST = "127.0.0.1"
SERVER_PORT = 6667
SERVER_PASS = "testpass"
NUM_CLIENTS = 5

class Colors:
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    CYAN = '\033[96m'

def client_thread(client_id):
    """Thread pour simuler un client IRC"""
    try:
        print(f"{Colors.CYAN}[Client {client_id}] Connexion...{Colors.ENDC}")
        
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.settimeout(5)
        sock.connect((SERVER_HOST, SERVER_PORT))
        
        time.sleep(0.1)
        
        # Authentification
        sock.send(f"PASS {SERVER_PASS}\r\n".encode())
        time.sleep(0.1)
        
        sock.send(f"NICK client{client_id}\r\n".encode())
        time.sleep(0.1)
        
        sock.send(f"USER client{client_id} 0 * :Client {client_id}\r\n".encode())
        time.sleep(0.2)
        
        # Lire la réponse
        try:
            response = sock.recv(4096).decode('utf-8', errors='ignore')
            if "001" in response or "Welcome" in response:
                print(f"{Colors.OKGREEN}[Client {client_id}] ✓ Authentifié{Colors.ENDC}")
            else:
                print(f"{Colors.WARNING}[Client {client_id}] Réponse: {response[:100]}{Colors.ENDC}")
        except:
            pass
        
        # Rejoindre un channel commun
        sock.send(f"JOIN #stress\r\n".encode())
        time.sleep(0.2)
        
        try:
            response = sock.recv(4096).decode('utf-8', errors='ignore')
            print(f"{Colors.OKGREEN}[Client {client_id}] ✓ Rejoint #stress{Colors.ENDC}")
        except:
            pass
        
        # Envoyer des messages
        for i in range(5):
            msg = f"PRIVMSG #stress :Message {i} from client {client_id}\r\n"
            sock.send(msg.encode())
            time.sleep(0.3)
            
            # Lire les réponses
            try:
                sock.settimeout(0.5)
                response = sock.recv(4096).decode('utf-8', errors='ignore')
                if response:
                    lines = response.split('\r\n')
                    for line in lines[:3]:  # Afficher max 3 lignes
                        if line.strip():
                            print(f"{Colors.CYAN}[Client {client_id}] << {line[:80]}{Colors.ENDC}")
            except socket.timeout:
                pass
            except:
                pass
        
        # Changer de topic (client 1 seulement)
        if client_id == 1:
            time.sleep(1)
            sock.send(f"TOPIC #stress :Topic set by client {client_id}\r\n".encode())
            time.sleep(0.2)
            try:
                response = sock.recv(4096).decode('utf-8', errors='ignore')
                print(f"{Colors.OKGREEN}[Client {client_id}] ✓ Topic changé{Colors.ENDC}")
            except:
                pass
        
        # Tester les modes (client 1 seulement)
        if client_id == 1:
            time.sleep(0.5)
            sock.send(f"MODE #stress +it\r\n".encode())
            time.sleep(0.2)
            try:
                response = sock.recv(4096).decode('utf-8', errors='ignore')
                print(f"{Colors.OKGREEN}[Client {client_id}] ✓ Mode +it activé{Colors.ENDC}")
            except:
                pass
        
        # Attendre un peu
        time.sleep(2)
        
        # Quitter proprement
        sock.send(f"QUIT :Client {client_id} leaving\r\n".encode())
        time.sleep(0.2)
        
        try:
            response = sock.recv(4096).decode('utf-8', errors='ignore')
        except:
            pass
        
        sock.close()
        print(f"{Colors.OKGREEN}[Client {client_id}] ✓ Déconnecté{Colors.ENDC}")
        
    except Exception as e:
        print(f"{Colors.FAIL}[Client {client_id}] ✗ Erreur: {e}{Colors.ENDC}")

def main():
    print(f"\n{Colors.BOLD}{'='*60}")
    print(f"TEST MULTI-CLIENTS - ft_irc")
    print(f"{'='*60}{Colors.ENDC}\n")
    
    print(f"{Colors.CYAN}Configuration:")
    print(f"  Serveur: {SERVER_HOST}:{SERVER_PORT}")
    print(f"  Password: {SERVER_PASS}")
    print(f"  Nombre de clients: {NUM_CLIENTS}{Colors.ENDC}\n")
    
    print(f"{Colors.WARNING}Assurez-vous que le serveur IRC est démarré!{Colors.ENDC}")
    
    input(f"\n{Colors.BOLD}Appuyez sur Entrée pour commencer...{Colors.ENDC}\n")
    
    # Créer les threads pour chaque client
    threads = []
    
    print(f"{Colors.BOLD}Lancement de {NUM_CLIENTS} clients...{Colors.ENDC}\n")
    
    for i in range(1, NUM_CLIENTS + 1):
        thread = threading.Thread(target=client_thread, args=(i,))
        thread.start()
        threads.append(thread)
        time.sleep(0.5)  # Petit délai entre chaque client
    
    # Attendre que tous les threads terminent
    for thread in threads:
        thread.join()
    
    print(f"\n{Colors.BOLD}{Colors.OKGREEN}{'='*60}")
    print(f"TEST TERMINÉ - Tous les clients ont terminé")
    print(f"{'='*60}{Colors.ENDC}\n")
    
    print(f"{Colors.CYAN}Vérifications recommandées:")
    print(f"  1. Vérifier les logs du serveur")
    print(f"  2. Tester avec Valgrind: ./test_valgrind.sh")
    print(f"  3. Vérifier qu'aucun client n'est resté bloqué{Colors.ENDC}\n")

if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        print(f"\n{Colors.WARNING}Test interrompu par l'utilisateur{Colors.ENDC}")
        sys.exit(0)
