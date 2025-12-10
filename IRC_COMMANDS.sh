#!/bin/bash

# ============================================================================
# Exemples de commandes IRC - Guide de référence rapide
# ============================================================================

cat << 'EOF'
╔═══════════════════════════════════════════════════════════════════╗
║                   COMMANDES IRC - RÉFÉRENCE RAPIDE                ║
╚═══════════════════════════════════════════════════════════════════╝

━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
 🔐 AUTHENTIFICATION
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

PASS <password>                    # Définir le mot de passe
NICK <nickname>                    # Définir le pseudo
USER <username> 0 * :<realname>    # Définir user et nom réel

Exemple de séquence complète:
  PASS mypassword
  NICK Alice
  USER alice 0 * :Alice Smith

━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
 💬 CHANNELS
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

JOIN #channel                      # Rejoindre un channel
JOIN #channel <password>           # Rejoindre avec mot de passe
PART #channel                      # Quitter un channel
PART #channel :Goodbye!            # Quitter avec message

Exemples:
  JOIN #general
  JOIN #vip secret123
  PART #general :See you later!

━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
 📨 MESSAGES
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

PRIVMSG #channel :message          # Message dans un channel
PRIVMSG <nickname> :message        # Message privé

Exemples:
  PRIVMSG #general :Hello everyone!
  PRIVMSG Bob :Hey, how are you?

━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
 🎛️  MODES
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

MODE #channel                      # Afficher les modes actuels

MODE #channel +i                   # Invite only (sur invitation)
MODE #channel -i                   # Retirer invite only

MODE #channel +t                   # Topic restrict (OPs uniquement)
MODE #channel -t                   # Retirer topic restrict

MODE #channel +k <password>        # Définir mot de passe
MODE #channel -k                   # Retirer mot de passe

MODE #channel +l <limit>           # Limite d'utilisateurs
MODE #channel -l                   # Retirer la limite

MODE #channel +o <nickname>        # Donner OP à quelqu'un
MODE #channel -o <nickname>        # Retirer OP

Modes combinés:
  MODE #channel +it               # Activer i et t
  MODE #channel -it               # Désactiver i et t
  MODE #channel +kl secret 50     # Password + limite de 50

Exemples complets:
  MODE #vip                        # Voir les modes
  MODE #vip +i                     # Channel sur invitation
  MODE #vip +k mypass              # Ajouter password
  MODE #vip +l 20                  # Max 20 users
  MODE #vip +o Alice               # Alice devient OP
  MODE #vip +itk secret            # Multiple modes

━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
 👥 GESTION DES UTILISATEURS
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

INVITE <nickname> #channel         # Inviter quelqu'un
KICK #channel <nickname> :reason   # Éjecter quelqu'un

Exemples:
  INVITE Bob #vip
  KICK #general Alice :Spam
  KICK #general Bob :Stop flooding!

━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
 📰 TOPIC
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

TOPIC #channel                     # Voir le topic actuel
TOPIC #channel :new topic          # Changer le topic

Exemples:
  TOPIC #general
  TOPIC #general :Bienvenue sur le serveur!
  TOPIC #general :                 # Effacer le topic

━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
 🤖 COMMANDES BOT
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

!ping                              # Bot répond "pong"
!rules                             # Affiche les règles
!dadjoke                           # Blague aléatoire

⚠️  Mots interdits (kick auto):
  - pute, connard, con, fdp, PTT

Exemples:
  PRIVMSG #chat :!ping
  PRIVMSG #chat :!rules
  PRIVMSG #chat :!dadjoke

━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
 🚪 DÉCONNEXION
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

QUIT                               # Quitter le serveur
QUIT :message                      # Quitter avec message

Exemples:
  QUIT
  QUIT :Goodbye everyone!

━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
 📋 SCÉNARIOS COMPLETS
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

1️⃣  CRÉER UN CHANNEL PRIVÉ:
  PASS mypassword
  NICK Alice
  USER alice 0 * :Alice Smith
  JOIN #private
  MODE #private +i              # Invite only
  MODE #private +k secret123    # Password
  INVITE Bob #private           # Inviter Bob

2️⃣  REJOINDRE UN CHANNEL PROTÉGÉ:
  PASS mypassword
  NICK Bob
  USER bob 0 * :Bob Smith
  JOIN #private secret123       # Avec le password

3️⃣  MODÉRER UN CHANNEL:
  MODE #general +t              # Seuls OPs changent topic
  TOPIC #general :Official topic
  MODE #general +o Alice        # Alice devient OP
  KICK #general Spammer :Stop spamming

4️⃣  CONVERSATION PRIVÉE:
  JOIN #chat
  PRIVMSG #chat :Hello!
  PRIVMSG Bob :Hi, join #private please
  INVITE Bob #private

5️⃣  CONFIGURER UN CHANNEL VIP:
  JOIN #vip
  MODE #vip +i                  # Invite only
  MODE #vip +t                  # Topic protected
  MODE #vip +l 10               # Max 10 users
  TOPIC #vip :VIP Members Only
  INVITE Alice #vip
  MODE #vip +o Alice            # Alice co-admin

━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
 🔢 CODES DE RÉPONSE IMPORTANTS
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

001  Welcome message
324  Channel mode
332  Channel topic
353  Names list
366  End of names
401  No such nick/channel
403  No such channel
421  Unknown command
433  Nickname in use
441  User not on channel
442  You're not on channel
451  Not registered
461  Not enough parameters
464  Password incorrect
473  Invite only channel
475  Bad channel key
482  You're not channel operator

━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
 🧪 TESTER AVEC NETCAT
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

# Se connecter au serveur
nc localhost 6667

# Puis taper les commandes:
PASS mypassword
NICK TestUser
USER testuser 0 * :Test User
JOIN #test
PRIVMSG #test :Hello!
QUIT :Bye

━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

📚 Pour plus d'informations, consultez:
   - TESTING.md (documentation des tests)
   - MODE_REFACTORING.md (détails sur les modes)

🚀 Scripts de test disponibles:
   ./test_irc.sh        # Tests automatisés
   ./manual_test.sh     # Tests manuels
   ./stress_test.sh     # Tests de charge

EOF
