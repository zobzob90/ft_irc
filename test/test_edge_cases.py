#!/usr/bin/env python3
"""
Script de test edge cases pour ft_irc
Teste un maximum de cas limites du serveur IRC
"""

import socket
import time
import sys
import threading
import signal

# Configuration
SERVER_HOST = "127.0.0.1"
SERVER_PORT = 6667
SERVER_PASS = "testpass"

class Colors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKCYAN = '\033[96m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'

class IRCClient:
    def __init__(self, name="TestClient"):
        self.name = name
        self.sock = None
        self.connected = False
        
    def connect(self):
        try:
            self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.sock.settimeout(2)
            self.sock.connect((SERVER_HOST, SERVER_PORT))
            self.connected = True
            time.sleep(0.1)
            return True
        except Exception as e:
            print(f"{Colors.FAIL}[{self.name}] Connection failed: {e}{Colors.ENDC}")
            return False
    
    def send(self, msg):
        if not self.connected:
            return False
        try:
            if not msg.endswith('\r\n'):
                msg += '\r\n'
            self.sock.send(msg.encode())
            time.sleep(0.05)
            return True
        except Exception as e:
            print(f"{Colors.FAIL}[{self.name}] Send failed: {e}{Colors.ENDC}")
            return False
    
    def recv(self, timeout=1):
        if not self.connected:
            return ""
        try:
            self.sock.settimeout(timeout)
            data = self.sock.recv(4096).decode('utf-8', errors='ignore')
            return data
        except socket.timeout:
            return ""
        except Exception as e:
            print(f"{Colors.FAIL}[{self.name}] Recv failed: {e}{Colors.ENDC}")
            return ""
    
    def close(self):
        if self.sock:
            try:
                self.sock.close()
            except:
                pass
        self.connected = False

def print_test(name):
    print(f"\n{Colors.OKBLUE}{'='*80}{Colors.ENDC}")
    print(f"{Colors.BOLD}{Colors.HEADER}TEST: {name}{Colors.ENDC}")
    print(f"{Colors.OKBLUE}{'='*80}{Colors.ENDC}")

def print_success(msg):
    print(f"{Colors.OKGREEN}✓ {msg}{Colors.ENDC}")

def print_fail(msg):
    print(f"{Colors.FAIL}✗ {msg}{Colors.ENDC}")

def print_info(msg):
    print(f"{Colors.OKCYAN}ℹ {msg}{Colors.ENDC}")

def print_warning(msg):
    print(f"{Colors.WARNING}⚠ {msg}{Colors.ENDC}")

# ============================================================================
# TESTS D'AUTHENTIFICATION
# ============================================================================

def test_auth_wrong_password():
    """Test 1: Mauvais mot de passe"""
    print_test("Test 1: Authentification avec mauvais mot de passe")
    client = IRCClient("WrongPassClient")
    
    if not client.connect():
        print_fail("Impossible de se connecter")
        return
    
    client.send("PASS wrongpassword")
    client.send("NICK testuser")
    client.send("USER testuser 0 * :Test User")
    
    response = client.recv()
    print_info(f"Réponse serveur: {response[:200]}")
    
    if "464" in response or "password" in response.lower():
        print_success("Serveur refuse correctement le mauvais mot de passe")
    else:
        print_warning("Vérifier le rejet du mauvais mot de passe")
    
    client.close()

def test_auth_no_password():
    """Test 2: Connexion sans mot de passe"""
    print_test("Test 2: Connexion sans envoyer PASS")
    client = IRCClient("NoPassClient")
    
    if not client.connect():
        print_fail("Impossible de se connecter")
        return
    
    client.send("NICK testuser")
    client.send("USER testuser 0 * :Test User")
    
    response = client.recv()
    print_info(f"Réponse serveur: {response[:200]}")
    
    client.close()

def test_auth_order():
    """Test 3: Ordre incorrect des commandes d'auth"""
    print_test("Test 3: Ordre incorrect (USER avant PASS)")
    client = IRCClient("WrongOrderClient")
    
    if not client.connect():
        return
    
    client.send("USER testuser 0 * :Test User")
    client.send("NICK testuser")
    client.send(f"PASS {SERVER_PASS}")
    
    response = client.recv()
    print_info(f"Réponse serveur: {response[:200]}")
    
    client.close()

def test_auth_duplicate_pass():
    """Test 4: PASS envoyé plusieurs fois"""
    print_test("Test 4: Commande PASS envoyée plusieurs fois")
    client = IRCClient("DuplicatePassClient")
    
    if not client.connect():
        return
    
    client.send(f"PASS {SERVER_PASS}")
    client.send(f"PASS {SERVER_PASS}")
    client.send(f"PASS wrongpass")
    client.send("NICK testuser")
    client.send("USER testuser 0 * :Test User")
    
    response = client.recv()
    print_info(f"Réponse serveur: {response[:200]}")
    
    client.close()

# ============================================================================
# TESTS DE NICKNAMES
# ============================================================================

def test_nick_invalid_chars():
    """Test 5: Nicknames avec caractères invalides"""
    print_test("Test 5: Nicknames avec caractères invalides")
    client = IRCClient("InvalidNickClient")
    
    if not client.connect():
        return
    
    client.send(f"PASS {SERVER_PASS}")
    
    invalid_nicks = [
        "nick@test",
        "nick#test",
        "nick with spaces",
        "nick!test",
        "123nickname",  # Commence par un chiffre
        "nick$money",
        "nick&test",
        "",  # Vide
        "a" * 100,  # Trop long
    ]
    
    for nick in invalid_nicks:
        client.send(f"NICK {nick}")
        response = client.recv(0.5)
        print_info(f"NICK '{nick}' -> {response[:100] if response else 'Pas de réponse'}")
    
    client.close()

def test_nick_duplicate():
    """Test 6: Nicknames en double"""
    print_test("Test 6: Tentative d'utiliser un nickname déjà pris")
    
    # Client 1
    client1 = IRCClient("Client1")
    if not client1.connect():
        return
    
    client1.send(f"PASS {SERVER_PASS}")
    client1.send("NICK samenick")
    client1.send("USER user1 0 * :User One")
    response1 = client1.recv()
    print_info(f"Client1 connecté: {response1[:100]}")
    
    # Client 2 tente d'utiliser le même nick
    client2 = IRCClient("Client2")
    if not client2.connect():
        client1.close()
        return
    
    client2.send(f"PASS {SERVER_PASS}")
    client2.send("NICK samenick")
    response2 = client2.recv()
    
    if "433" in response2 or "in use" in response2.lower():
        print_success("Serveur refuse correctement le nickname en double")
    else:
        print_warning("Vérifier le rejet des nicknames en double")
    
    print_info(f"Réponse: {response2[:200]}")
    
    client1.close()
    client2.close()

def test_nick_change():
    """Test 7: Changement de nickname"""
    print_test("Test 7: Changement de nickname pendant la session")
    client = IRCClient("NickChangeClient")
    
    if not client.connect():
        return
    
    client.send(f"PASS {SERVER_PASS}")
    client.send("NICK originalnick")
    client.send("USER testuser 0 * :Test User")
    response = client.recv()
    
    # Changer de nick
    client.send("NICK newnick")
    response = client.recv()
    print_info(f"Changement de nick: {response[:200]}")
    
    # Essayer de reprendre l'ancien nick
    client.send("NICK originalnick")
    response = client.recv()
    print_info(f"Reprendre ancien nick: {response[:200]}")
    
    client.close()

# ============================================================================
# TESTS DE CHANNELS
# ============================================================================

def test_channel_invalid_names():
    """Test 8: Noms de channels invalides"""
    print_test("Test 8: Tentatives de JOIN avec noms invalides")
    client = IRCClient("InvalidChanClient")
    
    if not client.connect():
        return
    
    client.send(f"PASS {SERVER_PASS}")
    client.send("NICK testuser")
    client.send("USER testuser 0 * :Test User")
    client.recv()
    
    invalid_channels = [
        "notachannel",  # Sans #
        "#",  # Juste #
        "#chan nel",  # Avec espace
        "#ch@nnel",  # Caractères spéciaux
        "#" + "a" * 300,  # Trop long
        "##doublehash",
        "#",
        "",
    ]
    
    for chan in invalid_channels:
        client.send(f"JOIN {chan}")
        response = client.recv(0.5)
        print_info(f"JOIN '{chan}' -> {response[:100] if response else 'Pas de réponse'}")
    
    client.close()

def test_channel_modes():
    """Test 9: Modes de channels"""
    print_test("Test 9: Tests des modes de channels")
    
    # Client opérateur
    op = IRCClient("OpClient")
    if not op.connect():
        return
    
    op.send(f"PASS {SERVER_PASS}")
    op.send("NICK operator")
    op.send("USER operator 0 * :Operator")
    op.recv()
    
    # Créer et rejoindre un channel
    op.send("JOIN #testmode")
    response = op.recv()
    print_info(f"Channel créé: {response[:200]}")
    
    # Test MODE +i (invite-only)
    op.send("MODE #testmode +i")
    response = op.recv()
    print_info(f"MODE +i: {response[:100]}")
    
    # Test MODE +t (topic protection)
    op.send("MODE #testmode +t")
    response = op.recv()
    print_info(f"MODE +t: {response[:100]}")
    
    # Test MODE +k (key/password)
    op.send("MODE #testmode +k secretkey")
    response = op.recv()
    print_info(f"MODE +k: {response[:100]}")
    
    # Test MODE +l (user limit)
    op.send("MODE #testmode +l 5")
    response = op.recv()
    print_info(f"MODE +l: {response[:100]}")
    
    # Test MODE +o (operator)
    op.send("MODE #testmode +o operator")
    response = op.recv()
    print_info(f"MODE +o: {response[:100]}")
    
    # Tests de modes invalides
    op.send("MODE #testmode +xyz")
    response = op.recv()
    print_info(f"MODE invalide: {response[:100]}")
    
    # Test MODE sans channel
    op.send("MODE +i")
    response = op.recv()
    print_info(f"MODE sans channel: {response[:100]}")
    
    op.close()

def test_channel_key():
    """Test 10: Protection par clé de channel"""
    print_test("Test 10: Channel protégé par mot de passe")
    
    # Client 1 crée le channel avec clé
    client1 = IRCClient("KeyCreator")
    if not client1.connect():
        return
    
    client1.send(f"PASS {SERVER_PASS}")
    client1.send("NICK creator")
    client1.send("USER creator 0 * :Creator")
    client1.recv()
    
    client1.send("JOIN #keychan")
    client1.recv()
    client1.send("MODE #keychan +k secret123")
    response = client1.recv()
    print_info(f"Clé définie: {response[:100]}")
    
    # Client 2 essaie sans clé
    client2 = IRCClient("NoKeyClient")
    if not client2.connect():
        client1.close()
        return
    
    client2.send(f"PASS {SERVER_PASS}")
    client2.send("NICK joiner1")
    client2.send("USER joiner1 0 * :Joiner One")
    client2.recv()
    
    client2.send("JOIN #keychan")
    response = client2.recv()
    
    if "475" in response or "key" in response.lower():
        print_success("Serveur refuse l'accès sans clé")
    else:
        print_warning("Vérifier la protection par clé")
    print_info(f"Sans clé: {response[:100]}")
    
    # Client 2 essaie avec mauvaise clé
    client2.send("JOIN #keychan wrongkey")
    response = client2.recv()
    print_info(f"Mauvaise clé: {response[:100]}")
    
    # Client 2 essaie avec bonne clé
    client2.send("JOIN #keychan secret123")
    response = client2.recv()
    
    if "JOIN" in response:
        print_success("Accès autorisé avec bonne clé")
    print_info(f"Bonne clé: {response[:100]}")
    
    client1.close()
    client2.close()

def test_channel_limit():
    """Test 11: Limite d'utilisateurs"""
    print_test("Test 11: Limite d'utilisateurs dans un channel")
    
    clients = []
    
    # Client 1 crée le channel avec limite de 3
    client1 = IRCClient("LimitCreator")
    if not client1.connect():
        return
    
    client1.send(f"PASS {SERVER_PASS}")
    client1.send("NICK limitop")
    client1.send("USER limitop 0 * :Limit Op")
    client1.recv()
    
    client1.send("JOIN #limitchan")
    client1.recv()
    client1.send("MODE #limitchan +l 3")
    response = client1.recv()
    print_info(f"Limite définie à 3: {response[:100]}")
    
    clients.append(client1)
    
    # Clients 2 et 3 rejoignent (total: 3)
    for i in range(2, 4):
        client = IRCClient(f"LimitClient{i}")
        if not client.connect():
            continue
        
        client.send(f"PASS {SERVER_PASS}")
        client.send(f"NICK limituser{i}")
        client.send(f"USER limituser{i} 0 * :User {i}")
        client.recv()
        
        client.send("JOIN #limitchan")
        response = client.recv()
        print_info(f"Client {i} join: {response[:100]}")
        clients.append(client)
        time.sleep(0.2)
    
    # Client 4 devrait être refusé (limite atteinte)
    client4 = IRCClient("LimitClient4")
    if client4.connect():
        client4.send(f"PASS {SERVER_PASS}")
        client4.send("NICK limituser4")
        client4.send("USER limituser4 0 * :User 4")
        client4.recv()
        
        client4.send("JOIN #limitchan")
        response = client4.recv()
        
        if "471" in response or "full" in response.lower():
            print_success("Serveur refuse l'accès (channel plein)")
        else:
            print_warning("Vérifier la limite d'utilisateurs")
        print_info(f"Client 4 (devrait être refusé): {response[:100]}")
        
        clients.append(client4)
    
    for client in clients:
        client.close()

def test_channel_invite_only():
    """Test 12: Mode invite-only"""
    print_test("Test 12: Channel en mode invite-only")
    
    # Client 1 crée le channel +i
    op = IRCClient("InviteOp")
    if not op.connect():
        return
    
    op.send(f"PASS {SERVER_PASS}")
    op.send("NICK inviteop")
    op.send("USER inviteop 0 * :Invite Op")
    op.recv()
    
    op.send("JOIN #invitechan")
    op.recv()
    op.send("MODE #invitechan +i")
    response = op.recv()
    print_info(f"Mode +i activé: {response[:100]}")
    
    # Client 2 essaie de rejoindre sans invitation
    client2 = IRCClient("NoInviteClient")
    if not client2.connect():
        op.close()
        return
    
    client2.send(f"PASS {SERVER_PASS}")
    client2.send("NICK noninvited")
    client2.send("USER noninvited 0 * :Non Invited")
    client2.recv()
    
    client2.send("JOIN #invitechan")
    response = client2.recv()
    
    if "473" in response or "invite" in response.lower():
        print_success("Serveur refuse l'accès sans invitation")
    else:
        print_warning("Vérifier le mode invite-only")
    print_info(f"Sans invitation: {response[:100]}")
    
    # Client 1 invite Client 2
    op.send("INVITE noninvited #invitechan")
    response = op.recv()
    print_info(f"INVITE envoyée: {response[:100]}")
    
    # Client 2 essaie de rejoindre après invitation
    time.sleep(0.2)
    client2.send("JOIN #invitechan")
    response = client2.recv()
    
    if "JOIN" in response:
        print_success("Accès autorisé après invitation")
    print_info(f"Après invitation: {response[:100]}")
    
    op.close()
    client2.close()

# ============================================================================
# TESTS PRIVMSG
# ============================================================================

def test_privmsg_edge_cases():
    """Test 13: PRIVMSG edge cases"""
    print_test("Test 13: PRIVMSG - cas limites")
    
    client1 = IRCClient("PrivmsgClient1")
    if not client1.connect():
        return
    
    client1.send(f"PASS {SERVER_PASS}")
    client1.send("NICK sender")
    client1.send("USER sender 0 * :Sender")
    client1.recv()
    
    # PRIVMSG sans destinataire
    client1.send("PRIVMSG")
    response = client1.recv()
    print_info(f"Sans destinataire: {response[:100]}")
    
    # PRIVMSG sans message
    client1.send("PRIVMSG receiver")
    response = client1.recv()
    print_info(f"Sans message: {response[:100]}")
    
    # PRIVMSG vers utilisateur inexistant
    client1.send("PRIVMSG nonexistent :Hello!")
    response = client1.recv()
    print_info(f"User inexistant: {response[:100]}")
    
    # PRIVMSG vers channel inexistant
    client1.send("PRIVMSG #nonexistent :Hello!")
    response = client1.recv()
    print_info(f"Channel inexistant: {response[:100]}")
    
    # PRIVMSG très long
    long_msg = "A" * 500
    client1.send(f"PRIVMSG #test :{long_msg}")
    response = client1.recv()
    print_info(f"Message très long: {response[:100]}")
    
    # PRIVMSG avec caractères spéciaux
    client1.send("PRIVMSG #test :Message avec émojis 🔥 et caractères spéciaux @#$%")
    response = client1.recv()
    print_info(f"Caractères spéciaux: {response[:100]}")
    
    client1.close()

def test_privmsg_to_channel_not_member():
    """Test 14: PRIVMSG vers channel dont on n'est pas membre"""
    print_test("Test 14: PRIVMSG vers un channel sans être membre")
    
    # Client 1 crée un channel
    client1 = IRCClient("ChannelCreator")
    if not client1.connect():
        return
    
    client1.send(f"PASS {SERVER_PASS}")
    client1.send("NICK creator2")
    client1.send("USER creator2 0 * :Creator 2")
    client1.recv()
    
    client1.send("JOIN #privatechan")
    client1.recv()
    
    # Client 2 essaie d'envoyer un message sans être membre
    client2 = IRCClient("NonMember")
    if not client2.connect():
        client1.close()
        return
    
    client2.send(f"PASS {SERVER_PASS}")
    client2.send("NICK outsider")
    client2.send("USER outsider 0 * :Outsider")
    client2.recv()
    
    client2.send("PRIVMSG #privatechan :I'm not a member!")
    response = client2.recv()
    print_info(f"Message sans être membre: {response[:100]}")
    
    client1.close()
    client2.close()

# ============================================================================
# TESTS KICK
# ============================================================================

def test_kick_edge_cases():
    """Test 15: KICK edge cases"""
    print_test("Test 15: KICK - cas limites")
    
    # Setup: Op et membre dans un channel
    op = IRCClient("KickOp")
    if not op.connect():
        return
    
    op.send(f"PASS {SERVER_PASS}")
    op.send("NICK kickop")
    op.send("USER kickop 0 * :Kick Op")
    op.recv()
    
    op.send("JOIN #kicktest")
    op.recv()
    
    member = IRCClient("KickMember")
    if not member.connect():
        op.close()
        return
    
    member.send(f"PASS {SERVER_PASS}")
    member.send("NICK kickmember")
    member.send("USER kickmember 0 * :Kick Member")
    member.recv()
    
    member.send("JOIN #kicktest")
    member.recv()
    time.sleep(0.2)
    
    # KICK sans raison
    op.send("KICK #kicktest kickmember")
    response = op.recv()
    print_info(f"KICK sans raison: {response[:100]}")
    
    # Réinviter le membre
    time.sleep(0.2)
    member.send("JOIN #kicktest")
    member.recv()
    time.sleep(0.2)
    
    # KICK avec raison
    op.send("KICK #kicktest kickmember :Violating rules")
    response = op.recv()
    print_info(f"KICK avec raison: {response[:100]}")
    
    # KICK utilisateur inexistant
    op.send("KICK #kicktest nonexistent :You don't exist")
    response = op.recv()
    print_info(f"KICK user inexistant: {response[:100]}")
    
    # KICK de soi-même
    op.send("KICK #kicktest kickop :Kicking myself")
    response = op.recv()
    print_info(f"KICK soi-même: {response[:100]}")
    
    # Non-op essaie de KICK
    member2 = IRCClient("NonOpMember")
    if member2.connect():
        member2.send(f"PASS {SERVER_PASS}")
        member2.send("NICK nonop")
        member2.send("USER nonop 0 * :Non Op")
        member2.recv()
        
        member2.send("JOIN #kicktest")
        member2.recv()
        time.sleep(0.2)
        
        member2.send("KICK #kicktest kickop :You're gone")
        response = member2.recv()
        
        if "482" in response or "privilege" in response.lower():
            print_success("Non-op ne peut pas KICK")
        else:
            print_warning("Vérifier les privilèges KICK")
        print_info(f"Non-op KICK: {response[:100]}")
        
        member2.close()
    
    op.close()
    member.close()

# ============================================================================
# TESTS TOPIC
# ============================================================================

def test_topic_edge_cases():
    """Test 16: TOPIC edge cases"""
    print_test("Test 16: TOPIC - cas limites")
    
    op = IRCClient("TopicOp")
    if not op.connect():
        return
    
    op.send(f"PASS {SERVER_PASS}")
    op.send("NICK topicop")
    op.send("USER topicop 0 * :Topic Op")
    op.recv()
    
    op.send("JOIN #topictest")
    op.recv()
    
    # Définir un topic
    op.send("TOPIC #topictest :This is the topic")
    response = op.recv()
    print_info(f"Set topic: {response[:100]}")
    
    # Consulter le topic
    op.send("TOPIC #topictest")
    response = op.recv()
    print_info(f"Get topic: {response[:100]}")
    
    # Topic très long
    long_topic = "A" * 400
    op.send(f"TOPIC #topictest :{long_topic}")
    response = op.recv()
    print_info(f"Topic très long: {response[:100]}")
    
    # Activer mode +t (topic protection)
    op.send("MODE #topictest +t")
    response = op.recv()
    print_info(f"MODE +t: {response[:100]}")
    
    # Non-op essaie de changer le topic
    member = IRCClient("TopicMember")
    if member.connect():
        member.send(f"PASS {SERVER_PASS}")
        member.send("NICK topicmember")
        member.send("USER topicmember 0 * :Topic Member")
        member.recv()
        
        member.send("JOIN #topictest")
        member.recv()
        time.sleep(0.2)
        
        member.send("TOPIC #topictest :New topic by member")
        response = member.recv()
        
        if "482" in response or "privilege" in response.lower():
            print_success("Non-op ne peut pas changer le topic avec +t")
        else:
            print_warning("Vérifier la protection du topic")
        print_info(f"Non-op change topic: {response[:100]}")
        
        member.close()
    
    # Supprimer le topic
    op.send("TOPIC #topictest :")
    response = op.recv()
    print_info(f"Supprimer topic: {response[:100]}")
    
    op.close()

# ============================================================================
# TESTS DE STRESS ET LIMITES
# ============================================================================

def test_multiple_channels():
    """Test 17: Rejoindre plusieurs channels"""
    print_test("Test 17: Client rejoint plusieurs channels")
    
    client = IRCClient("MultiChanClient")
    if not client.connect():
        return
    
    client.send(f"PASS {SERVER_PASS}")
    client.send("NICK multichan")
    client.send("USER multichan 0 * :Multi Chan")
    client.recv()
    
    # Rejoindre plusieurs channels
    for i in range(10):
        client.send(f"JOIN #channel{i}")
        response = client.recv()
        print_info(f"JOIN #channel{i}: {response[:50]}")
        time.sleep(0.1)
    
    # Envoyer des messages à chaque channel
    for i in range(10):
        client.send(f"PRIVMSG #channel{i} :Message to channel {i}")
        time.sleep(0.05)
    
    client.close()

def test_rapid_fire_commands():
    """Test 18: Commandes rapides en rafale"""
    print_test("Test 18: Envoi rapide de commandes")
    
    client = IRCClient("RapidFireClient")
    if not client.connect():
        return
    
    client.send(f"PASS {SERVER_PASS}")
    client.send("NICK rapidfire")
    client.send("USER rapidfire 0 * :Rapid Fire")
    client.recv()
    
    # Envoyer beaucoup de commandes rapidement
    for i in range(20):
        client.send(f"JOIN #rapid{i}")
    
    time.sleep(0.5)
    response = client.recv(timeout=2)
    print_info(f"Réponses reçues: {len(response)} caractères")
    
    client.close()

def test_buffer_overflow():
    """Test 19: Tests de buffer overflow"""
    print_test("Test 19: Tentatives de buffer overflow")
    
    client = IRCClient("BufferClient")
    if not client.connect():
        return
    
    client.send(f"PASS {SERVER_PASS}")
    client.send("NICK buffer")
    client.send("USER buffer 0 * :Buffer Test")
    client.recv()
    
    # Message énorme
    huge_msg = "A" * 10000
    client.send(f"PRIVMSG #test :{huge_msg}")
    response = client.recv()
    print_info(f"Message 10000 chars: {response[:100]}")
    
    # Commande énorme
    huge_cmd = "JOIN " + "#" + "A" * 5000
    client.send(huge_cmd)
    response = client.recv()
    print_info(f"Commande énorme: {response[:100]}")
    
    # Paramètres nombreux
    many_params = "PRIVMSG " + " ".join([f"param{i}" for i in range(100)])
    client.send(many_params)
    response = client.recv()
    print_info(f"Nombreux paramètres: {response[:100]}")
    
    client.close()

def test_special_characters():
    """Test 20: Caractères spéciaux"""
    print_test("Test 20: Gestion des caractères spéciaux")
    
    client = IRCClient("SpecialCharClient")
    if not client.connect():
        return
    
    client.send(f"PASS {SERVER_PASS}")
    client.send("NICK special")
    client.send("USER special 0 * :Special Char")
    client.recv()
    
    # Messages avec différents caractères
    test_msgs = [
        "Message avec \r\n embeded",
        "Message avec \x00 null byte",
        "Message avec émojis 😀🚀🔥",
        "Message avec UTF-8 éàçù",
        "Message avec \t tabulations",
        "Message avec \\ backslash",
        "Message avec ' quotes '",
        'Message avec " double quotes "',
    ]
    
    for msg in test_msgs:
        try:
            client.send(f"PRIVMSG #test :{msg}")
            response = client.recv(0.3)
            print_info(f"Test: {msg[:30]} -> OK")
        except Exception as e:
            print_info(f"Test: {msg[:30]} -> Exception: {e}")
    
    client.close()

def test_disconnection_scenarios():
    """Test 21: Scénarios de déconnexion"""
    print_test("Test 21: Différents scénarios de déconnexion")
    
    # Déconnexion brutale
    client1 = IRCClient("BrutalDisconnect")
    if client1.connect():
        client1.send(f"PASS {SERVER_PASS}")
        client1.send("NICK brutal")
        client1.send("USER brutal 0 * :Brutal")
        client1.recv()
        client1.send("JOIN #disconnect")
        client1.recv()
        # Déconnexion sans QUIT
        client1.close()
        print_success("Déconnexion brutale sans QUIT")
    
    time.sleep(0.5)
    
    # Déconnexion avec QUIT
    client2 = IRCClient("GracefulDisconnect")
    if client2.connect():
        client2.send(f"PASS {SERVER_PASS}")
        client2.send("NICK graceful")
        client2.send("USER graceful 0 * :Graceful")
        client2.recv()
        client2.send("JOIN #disconnect")
        client2.recv()
        client2.send("QUIT :Goodbye!")
        response = client2.recv()
        print_info(f"QUIT response: {response[:100]}")
        client2.close()
        print_success("Déconnexion avec QUIT")
    
    time.sleep(0.5)
    
    # QUIT avec message long
    client3 = IRCClient("LongQuitMsg")
    if client3.connect():
        client3.send(f"PASS {SERVER_PASS}")
        client3.send("NICK longquit")
        client3.send("USER longquit 0 * :Long Quit")
        client3.recv()
        long_quit = "A" * 500
        client3.send(f"QUIT :{long_quit}")
        response = client3.recv()
        print_info(f"QUIT long message: {response[:100]}")
        client3.close()

def test_part_edge_cases():
    """Test 22: PART edge cases"""
    print_test("Test 22: PART - cas limites")
    
    client = IRCClient("PartClient")
    if not client.connect():
        return
    
    client.send(f"PASS {SERVER_PASS}")
    client.send("NICK parter")
    client.send("USER parter 0 * :Parter")
    client.recv()
    
    # PART d'un channel qu'on n'a pas rejoint
    client.send("PART #notjoined")
    response = client.recv()
    print_info(f"PART not joined: {response[:100]}")
    
    # JOIN puis PART
    client.send("JOIN #parttest")
    client.recv()
    client.send("PART #parttest :Leaving now")
    response = client.recv()
    print_info(f"PART with reason: {response[:100]}")
    
    # PART sans raison
    client.send("JOIN #parttest2")
    client.recv()
    client.send("PART #parttest2")
    response = client.recv()
    print_info(f"PART without reason: {response[:100]}")
    
    # PART plusieurs channels à la fois
    client.send("JOIN #part1,#part2,#part3")
    client.recv()
    client.send("PART #part1,#part2,#part3")
    response = client.recv()
    print_info(f"PART multiple channels: {response[:100]}")
    
    client.close()

def test_invite_edge_cases():
    """Test 23: INVITE edge cases"""
    print_test("Test 23: INVITE - cas limites")
    
    op = IRCClient("InviteOp2")
    if not op.connect():
        return
    
    op.send(f"PASS {SERVER_PASS}")
    op.send("NICK inviteop2")
    op.send("USER inviteop2 0 * :Invite Op 2")
    op.recv()
    
    op.send("JOIN #invitetest")
    op.recv()
    
    # INVITE utilisateur inexistant
    op.send("INVITE nonexistent #invitetest")
    response = op.recv()
    print_info(f"INVITE user inexistant: {response[:100]}")
    
    # INVITE vers channel inexistant
    op.send("INVITE someuser #nonexistent")
    response = op.recv()
    print_info(f"INVITE channel inexistant: {response[:100]}")
    
    # INVITE sans être op
    member = IRCClient("NonOpInviter")
    if member.connect():
        member.send(f"PASS {SERVER_PASS}")
        member.send("NICK noopinvite")
        member.send("USER noopinvite 0 * :No Op Invite")
        member.recv()
        
        member.send("JOIN #invitetest")
        member.recv()
        time.sleep(0.2)
        
        member.send("INVITE someuser #invitetest")
        response = member.recv()
        print_info(f"INVITE sans être op: {response[:100]}")
        
        member.close()
    
    # INVITE utilisateur déjà dans le channel
    op.send("INVITE inviteop2 #invitetest")
    response = op.recv()
    print_info(f"INVITE user déjà présent: {response[:100]}")
    
    op.close()

def test_mode_combinations():
    """Test 24: Combinaisons de modes"""
    print_test("Test 24: Combinaisons de modes de channel")
    
    op = IRCClient("ModeCombOp")
    if not op.connect():
        return
    
    op.send(f"PASS {SERVER_PASS}")
    op.send("NICK modecomb")
    op.send("USER modecomb 0 * :Mode Comb")
    op.recv()
    
    op.send("JOIN #modecomb")
    op.recv()
    
    # Plusieurs modes en même temps
    op.send("MODE #modecomb +it")
    response = op.recv()
    print_info(f"MODE +it: {response[:100]}")
    
    # Ajouter et retirer des modes
    op.send("MODE #modecomb +l-t 10")
    response = op.recv()
    print_info(f"MODE +l-t: {response[:100]}")
    
    # Modes avec paramètres multiples
    op.send("MODE #modecomb +kl secret 5")
    response = op.recv()
    print_info(f"MODE +kl avec params: {response[:100]}")
    
    # Retirer mode avec clé
    op.send("MODE #modecomb -k secret")
    response = op.recv()
    print_info(f"MODE -k: {response[:100]}")
    
    # MODE sur channel inexistant
    op.send("MODE #nonexistent +i")
    response = op.recv()
    print_info(f"MODE channel inexistant: {response[:100]}")
    
    op.close()

def test_empty_and_null():
    """Test 25: Commandes vides et nulles"""
    print_test("Test 25: Commandes vides et caractères nuls")
    
    client = IRCClient("EmptyClient")
    if not client.connect():
        return
    
    client.send(f"PASS {SERVER_PASS}")
    client.send("NICK empty")
    client.send("USER empty 0 * :Empty")
    client.recv()
    
    # Ligne vide
    client.send("\r\n")
    time.sleep(0.2)
    
    # Plusieurs lignes vides
    client.send("\r\n\r\n\r\n")
    time.sleep(0.2)
    
    # Espaces seulement
    client.send("     \r\n")
    time.sleep(0.2)
    
    # Commande sans paramètres requis
    client.send("JOIN\r\n")
    response = client.recv()
    print_info(f"JOIN sans param: {response[:100]}")
    
    client.send("KICK\r\n")
    response = client.recv()
    print_info(f"KICK sans param: {response[:100]}")
    
    client.send("MODE\r\n")
    response = client.recv()
    print_info(f"MODE sans param: {response[:100]}")
    
    client.close()

# ============================================================================
# TESTS BOT
# ============================================================================

def test_bot_interaction():
    """Test 26: Interaction avec le bot"""
    print_test("Test 26: Interaction avec le bot (BadWord Filter)")
    
    client = IRCClient("BotTester")
    if not client.connect():
        return
    
    client.send(f"PASS {SERVER_PASS}")
    client.send("NICK bottester")
    client.send("USER bottester 0 * :Bot Tester")
    client.recv()
    
    # Rejoindre un channel
    client.send("JOIN #bottest")
    response = client.recv()
    print_info(f"JOIN channel: {response[:100]}")
    
    # Envoyer un message normal
    client.send("PRIVMSG #bottest :Hello everyone!")
    time.sleep(0.3)
    response = client.recv()
    print_info(f"Message normal: {response[:100]}")
    
    # Envoyer des messages avec des mots potentiellement interdits
    bad_words = ["fuck", "shit", "damn", "stupid", "idiot"]
    for word in bad_words:
        client.send(f"PRIVMSG #bottest :This is {word} message")
        time.sleep(0.3)
        response = client.recv()
        print_info(f"Message avec '{word}': {response[:150]}")
    
    client.close()

# ============================================================================
# MAIN RUNNER
# ============================================================================

def run_all_tests():
    """Exécute tous les tests"""
    print(f"\n{Colors.BOLD}{Colors.HEADER}")
    print("=" * 80)
    print("TEST SUITE FT_IRC - EDGE CASES")
    print("=" * 80)
    print(f"{Colors.ENDC}\n")
    
    print_info(f"Serveur: {SERVER_HOST}:{SERVER_PORT}")
    print_info(f"Mot de passe: {SERVER_PASS}")
    print_warning("Assurez-vous que le serveur IRC est démarré!")
    
    input(f"\n{Colors.BOLD}Appuyez sur Entrée pour commencer les tests...{Colors.ENDC}")
    
    tests = [
        # Authentification
        test_auth_wrong_password,
        test_auth_no_password,
        test_auth_order,
        test_auth_duplicate_pass,
        
        # Nicknames
        test_nick_invalid_chars,
        test_nick_duplicate,
        test_nick_change,
        
        # Channels
        test_channel_invalid_names,
        test_channel_modes,
        test_channel_key,
        test_channel_limit,
        test_channel_invite_only,
        
        # Messages
        test_privmsg_edge_cases,
        test_privmsg_to_channel_not_member,
        
        # Opérations
        test_kick_edge_cases,
        test_topic_edge_cases,
        test_part_edge_cases,
        test_invite_edge_cases,
        
        # Modes
        test_mode_combinations,
        
        # Stress
        test_multiple_channels,
        test_rapid_fire_commands,
        test_buffer_overflow,
        test_special_characters,
        test_disconnection_scenarios,
        test_empty_and_null,
        
        # Bot
        test_bot_interaction,
    ]
    
    total = len(tests)
    for i, test in enumerate(tests, 1):
        try:
            print(f"\n{Colors.OKCYAN}[{i}/{total}] Exécution...{Colors.ENDC}")
            test()
            time.sleep(0.5)
        except Exception as e:
            print_fail(f"Exception pendant le test: {e}")
            import traceback
            traceback.print_exc()
        
        if i < total:
            time.sleep(1)
    
    print(f"\n{Colors.BOLD}{Colors.OKGREEN}")
    print("=" * 80)
    print(f"TESTS TERMINÉS - {total} tests exécutés")
    print("=" * 80)
    print(f"{Colors.ENDC}\n")

if __name__ == "__main__":
    try:
        run_all_tests()
    except KeyboardInterrupt:
        print(f"\n{Colors.WARNING}Tests interrompus par l'utilisateur{Colors.ENDC}")
        sys.exit(0)
