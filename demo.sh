#!/bin/bash

# Script de démonstration et menu interactif

clear

cat << 'EOF'
╔═══════════════════════════════════════════════════════════════════╗
║                                                                   ║
║                  🎉 SUITE DE TESTS IRC 🎉                        ║
║                                                                   ║
║               Votre serveur IRC est prêt à être testé !          ║
║                                                                   ║
╚═══════════════════════════════════════════════════════════════════╝

📁 Fichiers créés:
   ✓ test_irc.sh        (23 KB) - Tests automatisés complets
   ✓ manual_test.sh     (1.4 KB) - Tests manuels interactifs  
   ✓ stress_test.sh     (8.1 KB) - Tests de charge
   ✓ IRC_COMMANDS.sh    (11 KB) - Guide de référence
   ✓ TESTING.md         (6.5 KB) - Documentation
   ✓ TESTS_README.md    (8.1 KB) - Guide complet

━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

🚀 DÉMARRAGE RAPIDE:

1️⃣  Lancer le serveur:
   make re && ./ircserv 6667 testpass

2️⃣  Lancer les tests (dans un autre terminal):
   ./test_irc.sh 6667 testpass

3️⃣  Voir le guide de commandes:
   ./IRC_COMMANDS.sh

━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

📊 TESTS DISPONIBLES:

🤖 Tests automatisés (~45 tests):
   • Authentification (PASS, NICK, USER)
   • Channels (JOIN, PART)
   • Messages (PRIVMSG)
   • Modes (i, t, k, l, o)
   • Gestion users (INVITE, KICK, TOPIC)
   • Bot (bienvenue, commandes, modération)
   • Cas limites et erreurs

🔥 Tests de charge:
   • Connexions multiples simultanées
   • Spam de messages
   • Changements de modes rapides
   • Déconnexions brutales

🎮 Tests manuels:
   • Session interactive
   • Saisie libre de commandes

━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

🎯 MENU INTERACTIF:

EOF

echo -e "\033[1;36mQue voulez-vous faire ?\033[0m\n"
echo "  1) Lancer les tests automatisés"
echo "  2) Lancer les tests de charge"
echo "  3) Tester manuellement (interactif)"
echo "  4) Voir le guide des commandes IRC"
echo "  5) Lire la documentation"
echo "  6) Vérifier que le serveur est lancé"
echo "  7) Afficher l'état des fichiers"
echo "  0) Quitter"
echo ""

read -p "Votre choix [0-7]: " choice

case $choice in
    1)
        echo -e "\n\033[1;32m→ Lancement des tests automatisés...\033[0m\n"
        read -p "Port [6667]: " port
        port=${port:-6667}
        read -p "Password [testpass]: " pass
        pass=${pass:-testpass}
        ./test_irc.sh $port $pass
        ;;
    2)
        echo -e "\n\033[1;32m→ Lancement des tests de charge...\033[0m\n"
        read -p "Port [6667]: " port
        port=${port:-6667}
        read -p "Password [testpass]: " pass
        pass=${pass:-testpass}
        read -p "Nombre de clients [10]: " clients
        clients=${clients:-10}
        ./stress_test.sh $port $pass $clients
        ;;
    3)
        echo -e "\n\033[1;32m→ Lancement du test manuel...\033[0m\n"
        read -p "Port [6667]: " port
        port=${port:-6667}
        read -p "Password [testpass]: " pass
        pass=${pass:-testpass}
        ./manual_test.sh $port $pass
        ;;
    4)
        echo -e "\n\033[1;32m→ Affichage du guide des commandes...\033[0m\n"
        ./IRC_COMMANDS.sh | less
        ;;
    5)
        echo -e "\n\033[1;32m→ Documentation disponible:\033[0m\n"
        echo "  • TESTS_README.md - Guide complet des tests"
        echo "  • TESTING.md - Documentation détaillée"
        echo "  • MODE_REFACTORING.md - Détails sur MODE"
        echo ""
        read -p "Quel fichier voulez-vous lire ? [TESTS_README.md]: " doc
        doc=${doc:-TESTS_README.md}
        if [ -f "$doc" ]; then
            less "$doc"
        else
            echo "Fichier non trouvé: $doc"
        fi
        ;;
    6)
        echo -e "\n\033[1;32m→ Vérification du serveur...\033[0m\n"
        read -p "Port [6667]: " port
        port=${port:-6667}
        if nc -z localhost $port 2>/dev/null; then
            echo -e "\033[1;32m✓ Le serveur est accessible sur localhost:$port\033[0m"
        else
            echo -e "\033[1;31m✗ Le serveur n'est PAS accessible sur localhost:$port\033[0m"
            echo -e "\nLancez-le avec: \033[1;33m./ircserv $port testpass\033[0m"
        fi
        ;;
    7)
        echo -e "\n\033[1;32m→ État des fichiers:\033[0m\n"
        ls -lh *.sh *.md 2>/dev/null | grep -E '\.(sh|md)$'
        echo ""
        echo "Scripts exécutables:"
        ls -lh *.sh | awk '{print "  " $9 " (" $5 ")"}'
        ;;
    0)
        echo -e "\n\033[1;36mAu revoir ! Bon courage pour votre projet IRC ! 🚀\033[0m\n"
        exit 0
        ;;
    *)
        echo -e "\n\033[1;31mChoix invalide.\033[0m\n"
        exit 1
        ;;
esac

echo ""
echo -e "\033[1;36m━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\033[0m"
echo -e "\033[1;36mPour relancer ce menu: ./demo.sh\033[0m"
echo -e "\033[1;36m━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\033[0m"
echo ""
