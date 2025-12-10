#!/bin/bash

# ============================================================================
# Script de test automatisé pour le serveur IRC
# ============================================================================
# Usage: ./test_irc.sh [port] [password]
# Exemple: ./test_irc.sh 6667 mypassword
# ============================================================================

# Couleurs pour l'affichage
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
MAGENTA='\033[0;35m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color
BOLD='\033[1m'

# Configuration
PORT=${1:-6667}
PASSWORD=${2:-"testpass"}
SERVER="localhost"
SLEEP_TIME=0.5

# Compteurs
TOTAL_TESTS=0
PASSED_TESTS=0
FAILED_TESTS=0

# ============================================================================
# Fonctions utilitaires
# ============================================================================

print_header() {
    echo -e "\n${BOLD}${CYAN}═══════════════════════════════════════════════════${NC}"
    echo -e "${BOLD}${CYAN}  $1${NC}"
    echo -e "${BOLD}${CYAN}═══════════════════════════════════════════════════${NC}\n"
}

print_test() {
    echo -e "${YELLOW}[TEST]${NC} $1"
    ((TOTAL_TESTS++))
}

print_success() {
    echo -e "${GREEN}[✓]${NC} $1"
    ((PASSED_TESTS++))
}

print_error() {
    echo -e "${RED}[✗]${NC} $1"
    ((FAILED_TESTS++))
}

print_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_command() {
    echo -e "${MAGENTA}→${NC} $1"
}

# Fonction pour envoyer une commande et attendre
send_cmd() {
    local fifo=$1
    local cmd=$2
    echo "$cmd" > "$fifo"
    print_command "$cmd"
    sleep $SLEEP_TIME
}

# Créer une connexion client
create_client() {
    local name=$1
    local fifo="/tmp/irc_test_${name}_$$"
    mkfifo "$fifo" 2>/dev/null || true
    
    # Lancer nc en arrière-plan
    tail -f "$fifo" | nc $SERVER $PORT > "/tmp/irc_output_${name}_$$" 2>&1 &
    local pid=$!
    
    sleep 0.3
    
    echo "$fifo:$pid"
}

# Fermer une connexion client
close_client() {
    local fifo=$1
    local pid=$2
    
    echo "QUIT :Bye" > "$fifo" 2>/dev/null
    sleep 0.2
    kill $pid 2>/dev/null
    rm -f "$fifo" 2>/dev/null
}

# Vérifier si le serveur est en cours d'exécution
check_server() {
    if ! nc -z $SERVER $PORT 2>/dev/null; then
        print_error "Le serveur IRC n'est pas accessible sur $SERVER:$PORT"
        echo -e "${YELLOW}Lancez d'abord le serveur avec:${NC} ./ircserv $PORT $PASSWORD"
        exit 1
    fi
    print_success "Serveur IRC détecté sur $SERVER:$PORT"
}

# ============================================================================
# Tests d'authentification
# ============================================================================

test_authentication() {
    print_header "TESTS D'AUTHENTIFICATION"
    
    # Test 1: Mauvais mot de passe
    print_test "Tentative avec mauvais mot de passe"
    client1=$(create_client "badpass")
    IFS=':' read -r fifo1 pid1 <<< "$client1"
    send_cmd "$fifo1" "PASS wrongpassword"
    send_cmd "$fifo1" "NICK TestUser"
    send_cmd "$fifo1" "USER testuser 0 * :Test User"
    sleep 1
    if grep -q "464" "/tmp/irc_output_badpass_$$" 2>/dev/null; then
        print_success "Erreur 464 reçue (mot de passe incorrect)"
    else
        print_error "L'erreur 464 n'a pas été détectée"
    fi
    close_client "$fifo1" "$pid1"
    
    # Test 2: Authentification réussie
    print_test "Authentification complète réussie (PASS + NICK + USER)"
    client2=$(create_client "goodauth")
    IFS=':' read -r fifo2 pid2 <<< "$client2"
    send_cmd "$fifo2" "PASS $PASSWORD"
    send_cmd "$fifo2" "NICK Alice"
    send_cmd "$fifo2" "USER alice 0 * :Alice Smith"
    sleep 1
    if grep -q "001" "/tmp/irc_output_goodauth_$$" 2>/dev/null; then
        print_success "Code 001 reçu (bienvenue)"
    else
        print_error "Le code 001 n'a pas été détecté"
    fi
    close_client "$fifo2" "$pid2"
    
    # Test 3: Nickname déjà utilisé
    print_test "Tentative d'utiliser un nickname déjà pris"
    client3=$(create_client "alice1")
    IFS=':' read -r fifo3 pid3 <<< "$client3"
    send_cmd "$fifo3" "PASS $PASSWORD"
    send_cmd "$fifo3" "NICK Alice"
    send_cmd "$fifo3" "USER alice1 0 * :Alice One"
    
    client4=$(create_client "alice2")
    IFS=':' read -r fifo4 pid4 <<< "$client4"
    send_cmd "$fifo4" "PASS $PASSWORD"
    send_cmd "$fifo4" "NICK Alice"
    sleep 1
    if grep -q "433" "/tmp/irc_output_alice2_$$" 2>/dev/null; then
        print_success "Erreur 433 reçue (nickname déjà utilisé)"
    else
        print_error "L'erreur 433 n'a pas été détectée"
    fi
    close_client "$fifo3" "$pid3"
    close_client "$fifo4" "$pid4"
}

# ============================================================================
# Tests JOIN / PART
# ============================================================================

test_join_part() {
    print_header "TESTS JOIN / PART"
    
    # Test 1: JOIN basique
    print_test "JOIN d'un channel simple"
    client=$(create_client "join1")
    IFS=':' read -r fifo pid <<< "$client"
    send_cmd "$fifo" "PASS $PASSWORD"
    send_cmd "$fifo" "NICK Bob"
    send_cmd "$fifo" "USER bob 0 * :Bob Smith"
    sleep 0.5
    send_cmd "$fifo" "JOIN #test"
    sleep 1
    if grep -q "JOIN #test" "/tmp/irc_output_join1_$$" 2>/dev/null; then
        print_success "JOIN réussi sur #test"
    else
        print_error "JOIN échoué"
    fi
    
    # Test 2: PART
    print_test "PART d'un channel"
    send_cmd "$fifo" "PART #test :Au revoir"
    sleep 1
    if grep -q "PART #test" "/tmp/irc_output_join1_$$" 2>/dev/null; then
        print_success "PART réussi"
    else
        print_error "PART échoué"
    fi
    close_client "$fifo" "$pid"
    
    # Test 3: JOIN avec nom invalide
    print_test "JOIN avec nom de channel invalide (sans #)"
    client2=$(create_client "join2")
    IFS=':' read -r fifo2 pid2 <<< "$client2"
    send_cmd "$fifo2" "PASS $PASSWORD"
    send_cmd "$fifo2" "NICK Charlie"
    send_cmd "$fifo2" "USER charlie 0 * :Charlie"
    sleep 0.5
    send_cmd "$fifo2" "JOIN invalid"
    sleep 1
    if grep -q "403" "/tmp/irc_output_join2_$$" 2>/dev/null; then
        print_success "Erreur 403 reçue (channel invalide)"
    else
        print_error "L'erreur 403 n'a pas été détectée"
    fi
    close_client "$fifo2" "$pid2"
}

# ============================================================================
# Tests PRIVMSG
# ============================================================================

test_privmsg() {
    print_header "TESTS PRIVMSG"
    
    # Créer 2 clients
    client1=$(create_client "priv1")
    IFS=':' read -r fifo1 pid1 <<< "$client1"
    send_cmd "$fifo1" "PASS $PASSWORD"
    send_cmd "$fifo1" "NICK David"
    send_cmd "$fifo1" "USER david 0 * :David"
    sleep 0.5
    send_cmd "$fifo1" "JOIN #chat"
    
    client2=$(create_client "priv2")
    IFS=':' read -r fifo2 pid2 <<< "$client2"
    send_cmd "$fifo2" "PASS $PASSWORD"
    send_cmd "$fifo2" "NICK Eve"
    send_cmd "$fifo2" "USER eve 0 * :Eve"
    sleep 0.5
    send_cmd "$fifo2" "JOIN #chat"
    sleep 1
    
    # Test 1: Message de channel
    print_test "Envoi d'un message dans un channel"
    send_cmd "$fifo1" "PRIVMSG #chat :Hello everyone!"
    sleep 1
    if grep -q "Hello everyone" "/tmp/irc_output_priv2_$$" 2>/dev/null; then
        print_success "Message reçu par l'autre client"
    else
        print_error "Message non reçu"
    fi
    
    # Test 2: Message privé
    print_test "Envoi d'un message privé"
    send_cmd "$fifo1" "PRIVMSG Eve :Hi Eve"
    sleep 1
    if grep -q "Hi Eve" "/tmp/irc_output_priv2_$$" 2>/dev/null; then
        print_success "Message privé reçu"
    else
        print_error "Message privé non reçu"
    fi
    
    close_client "$fifo1" "$pid1"
    close_client "$fifo2" "$pid2"
}

# ============================================================================
# Tests MODE
# ============================================================================

test_mode() {
    print_header "TESTS MODE"
    
    client=$(create_client "mode1")
    IFS=':' read -r fifo pid <<< "$client"
    send_cmd "$fifo" "PASS $PASSWORD"
    send_cmd "$fifo" "NICK Frank"
    send_cmd "$fifo" "USER frank 0 * :Frank"
    sleep 0.5
    send_cmd "$fifo" "JOIN #modetest"
    sleep 1
    
    # Test 1: Afficher les modes
    print_test "Affichage des modes du channel"
    send_cmd "$fifo" "MODE #modetest"
    sleep 1
    if grep -q "324" "/tmp/irc_output_mode1_$$" 2>/dev/null; then
        print_success "Code 324 reçu (modes du channel)"
    else
        print_error "Modes non affichés"
    fi
    
    # Test 2: Mode +i (invite only)
    print_test "Activation du mode +i (invite only)"
    send_cmd "$fifo" "MODE #modetest +i"
    sleep 1
    if grep -q "MODE #modetest +i" "/tmp/irc_output_mode1_$$" 2>/dev/null; then
        print_success "Mode +i activé"
    else
        print_error "Mode +i non activé"
    fi
    
    # Test 3: Mode +t (topic restrict)
    print_test "Activation du mode +t (topic restrict)"
    send_cmd "$fifo" "MODE #modetest +t"
    sleep 1
    if grep -q "MODE #modetest +t" "/tmp/irc_output_mode1_$$" 2>/dev/null; then
        print_success "Mode +t activé"
    else
        print_error "Mode +t non activé"
    fi
    
    # Test 4: Mode +k (password)
    print_test "Activation du mode +k (password)"
    send_cmd "$fifo" "MODE #modetest +k secret123"
    sleep 1
    if grep -q "MODE #modetest +k" "/tmp/irc_output_mode1_$$" 2>/dev/null; then
        print_success "Mode +k activé avec mot de passe"
    else
        print_error "Mode +k non activé"
    fi
    
    # Test 5: Mode +l (user limit)
    print_test "Activation du mode +l (user limit)"
    send_cmd "$fifo" "MODE #modetest +l 10"
    sleep 1
    if grep -q "MODE #modetest +l" "/tmp/irc_output_mode1_$$" 2>/dev/null; then
        print_success "Mode +l activé avec limite de 10"
    else
        print_error "Mode +l non activé"
    fi
    
    # Test 6: Modes combinés
    print_test "Désactivation de plusieurs modes (-it)"
    send_cmd "$fifo" "MODE #modetest -it"
    sleep 1
    if grep -q "MODE #modetest -it" "/tmp/irc_output_mode1_$$" 2>/dev/null; then
        print_success "Modes -it désactivés"
    else
        print_error "Modes -it non désactivés"
    fi
    
    close_client "$fifo" "$pid"
}

# ============================================================================
# Tests INVITE
# ============================================================================

test_invite() {
    print_header "TESTS INVITE"
    
    # Créer 2 clients
    client1=$(create_client "inv1")
    IFS=':' read -r fifo1 pid1 <<< "$client1"
    send_cmd "$fifo1" "PASS $PASSWORD"
    send_cmd "$fifo1" "NICK Grace"
    send_cmd "$fifo1" "USER grace 0 * :Grace"
    sleep 0.5
    send_cmd "$fifo1" "JOIN #vip"
    sleep 1
    
    client2=$(create_client "inv2")
    IFS=':' read -r fifo2 pid2 <<< "$client2"
    send_cmd "$fifo2" "PASS $PASSWORD"
    send_cmd "$fifo2" "NICK Hank"
    send_cmd "$fifo2" "USER hank 0 * :Hank"
    sleep 1
    
    # Test 1: INVITE basique
    print_test "Invitation d'un utilisateur"
    send_cmd "$fifo1" "INVITE Hank #vip"
    sleep 1
    if grep -q "INVITE Hank #vip" "/tmp/irc_output_inv2_$$" 2>/dev/null; then
        print_success "Invitation reçue par Hank"
    else
        print_error "Invitation non reçue"
    fi
    
    # Test 2: JOIN avec invitation
    print_test "JOIN après invitation"
    send_cmd "$fifo2" "JOIN #vip"
    sleep 1
    if grep -q "JOIN #vip" "/tmp/irc_output_inv2_$$" 2>/dev/null; then
        print_success "JOIN réussi après invitation"
    else
        print_error "JOIN échoué"
    fi
    
    # Test 3: Channel +i sans invitation
    print_test "Tentative de JOIN sur channel +i sans invitation"
    send_cmd "$fifo1" "MODE #vip +i"
    sleep 0.5
    
    client3=$(create_client "inv3")
    IFS=':' read -r fifo3 pid3 <<< "$client3"
    send_cmd "$fifo3" "PASS $PASSWORD"
    send_cmd "$fifo3" "NICK Ivan"
    send_cmd "$fifo3" "USER ivan 0 * :Ivan"
    sleep 0.5
    send_cmd "$fifo3" "JOIN #vip"
    sleep 1
    if grep -q "473" "/tmp/irc_output_inv3_$$" 2>/dev/null; then
        print_success "Erreur 473 reçue (invite only)"
    else
        print_error "L'erreur 473 n'a pas été détectée"
    fi
    
    close_client "$fifo1" "$pid1"
    close_client "$fifo2" "$pid2"
    close_client "$fifo3" "$pid3"
}

# ============================================================================
# Tests TOPIC
# ============================================================================

test_topic() {
    print_header "TESTS TOPIC"
    
    client=$(create_client "topic1")
    IFS=':' read -r fifo pid <<< "$client"
    send_cmd "$fifo" "PASS $PASSWORD"
    send_cmd "$fifo" "NICK Julia"
    send_cmd "$fifo" "USER julia 0 * :Julia"
    sleep 0.5
    send_cmd "$fifo" "JOIN #topictest"
    sleep 1
    
    # Test 1: Définir un topic
    print_test "Définir un topic"
    send_cmd "$fifo" "TOPIC #topictest :Bienvenue sur notre channel!"
    sleep 1
    if grep -q "TOPIC #topictest" "/tmp/irc_output_topic1_$$" 2>/dev/null; then
        print_success "Topic défini"
    else
        print_error "Topic non défini"
    fi
    
    # Test 2: Afficher le topic
    print_test "Afficher le topic"
    send_cmd "$fifo" "TOPIC #topictest"
    sleep 1
    if grep -q "332" "/tmp/irc_output_topic1_$$" 2>/dev/null; then
        print_success "Code 332 reçu (topic affiché)"
    else
        print_error "Topic non affiché"
    fi
    
    # Test 3: Mode +t (seuls les OPs changent le topic)
    print_test "Mode +t et tentative de changement par non-OP"
    send_cmd "$fifo" "MODE #topictest +t"
    sleep 0.5
    
    client2=$(create_client "topic2")
    IFS=':' read -r fifo2 pid2 <<< "$client2"
    send_cmd "$fifo2" "PASS $PASSWORD"
    send_cmd "$fifo2" "NICK Kevin"
    send_cmd "$fifo2" "USER kevin 0 * :Kevin"
    sleep 0.5
    send_cmd "$fifo2" "JOIN #topictest"
    sleep 1
    send_cmd "$fifo2" "TOPIC #topictest :Nouveau topic"
    sleep 1
    if grep -q "482" "/tmp/irc_output_topic2_$$" 2>/dev/null; then
        print_success "Erreur 482 reçue (pas opérateur)"
    else
        print_error "L'erreur 482 n'a pas été détectée"
    fi
    
    close_client "$fifo" "$pid"
    close_client "$fifo2" "$pid2"
}

# ============================================================================
# Tests KICK
# ============================================================================

test_kick() {
    print_header "TESTS KICK"
    
    # Créer 2 clients
    client1=$(create_client "kick1")
    IFS=':' read -r fifo1 pid1 <<< "$client1"
    send_cmd "$fifo1" "PASS $PASSWORD"
    send_cmd "$fifo1" "NICK Laura"
    send_cmd "$fifo1" "USER laura 0 * :Laura"
    sleep 0.5
    send_cmd "$fifo1" "JOIN #kicktest"
    sleep 1
    
    client2=$(create_client "kick2")
    IFS=':' read -r fifo2 pid2 <<< "$client2"
    send_cmd "$fifo2" "PASS $PASSWORD"
    send_cmd "$fifo2" "NICK Mike"
    send_cmd "$fifo2" "USER mike 0 * :Mike"
    sleep 0.5
    send_cmd "$fifo2" "JOIN #kicktest"
    sleep 1
    
    # Test 1: KICK par un opérateur
    print_test "KICK d'un utilisateur par un opérateur"
    send_cmd "$fifo1" "KICK #kicktest Mike :Spam"
    sleep 1
    if grep -q "KICK #kicktest Mike" "/tmp/irc_output_kick2_$$" 2>/dev/null; then
        print_success "KICK reçu par Mike"
    else
        print_error "KICK non reçu"
    fi
    
    # Test 2: KICK par un non-opérateur
    print_test "Tentative de KICK par un non-opérateur"
    client3=$(create_client "kick3")
    IFS=':' read -r fifo3 pid3 <<< "$client3"
    send_cmd "$fifo3" "PASS $PASSWORD"
    send_cmd "$fifo3" "NICK Nancy"
    send_cmd "$fifo3" "USER nancy 0 * :Nancy"
    sleep 0.5
    send_cmd "$fifo3" "JOIN #kicktest"
    sleep 1
    send_cmd "$fifo3" "KICK #kicktest Laura :Test"
    sleep 1
    if grep -q "482" "/tmp/irc_output_kick3_$$" 2>/dev/null; then
        print_success "Erreur 482 reçue (pas opérateur)"
    else
        print_error "L'erreur 482 n'a pas été détectée"
    fi
    
    close_client "$fifo1" "$pid1"
    close_client "$fifo2" "$pid2"
    close_client "$fifo3" "$pid3"
}

# ============================================================================
# Tests BOT
# ============================================================================

test_bot() {
    print_header "TESTS BOT"
    
    client=$(create_client "bot1")
    IFS=':' read -r fifo pid <<< "$client"
    send_cmd "$fifo" "PASS $PASSWORD"
    send_cmd "$fifo" "NICK Oscar"
    send_cmd "$fifo" "USER oscar 0 * :Oscar"
    sleep 0.5
    send_cmd "$fifo" "JOIN #bottest"
    sleep 1
    
    # Test 1: Message de bienvenue du bot
    print_test "Message de bienvenue du bot"
    if grep -q "BotCop" "/tmp/irc_output_bot1_$$" 2>/dev/null && grep -q "Welcome" "/tmp/irc_output_bot1_$$" 2>/dev/null; then
        print_success "Bot a envoyé un message de bienvenue"
    else
        print_error "Pas de message de bienvenue du bot"
    fi
    
    # Test 2: Commande !ping
    print_test "Commande !ping du bot"
    send_cmd "$fifo" "PRIVMSG #bottest :!ping"
    sleep 1
    if grep -q "pong" "/tmp/irc_output_bot1_$$" 2>/dev/null; then
        print_success "Bot a répondu 'pong'"
    else
        print_error "Bot n'a pas répondu"
    fi
    
    # Test 3: Commande !rules
    print_test "Commande !rules du bot"
    send_cmd "$fifo" "PRIVMSG #bottest :!rules"
    sleep 1
    if grep -q "Regle" "/tmp/irc_output_bot1_$$" 2>/dev/null; then
        print_success "Bot a affiché les règles"
    else
        print_error "Bot n'a pas affiché les règles"
    fi
    
    # Test 4: Mot interdit (kick automatique)
    print_test "Détection de mot interdit et kick automatique"
    send_cmd "$fifo" "PRIVMSG #bottest :connard"
    sleep 1
    if grep -q "KICK" "/tmp/irc_output_bot1_$$" 2>/dev/null; then
        print_success "Bot a kick l'utilisateur pour mot interdit"
    else
        print_error "Bot n'a pas kick l'utilisateur"
    fi
    
    close_client "$fifo" "$pid"
}

# ============================================================================
# Tests de cas limites
# ============================================================================

test_edge_cases() {
    print_header "TESTS DE CAS LIMITES"
    
    client=$(create_client "edge1")
    IFS=':' read -r fifo pid <<< "$client"
    send_cmd "$fifo" "PASS $PASSWORD"
    send_cmd "$fifo" "NICK Paula"
    send_cmd "$fifo" "USER paula 0 * :Paula"
    sleep 0.5
    
    # Test 1: Commande inconnue
    print_test "Envoi d'une commande inconnue"
    send_cmd "$fifo" "UNKNOWN command"
    sleep 1
    if grep -q "421" "/tmp/irc_output_edge1_$$" 2>/dev/null; then
        print_success "Erreur 421 reçue (commande inconnue)"
    else
        print_error "L'erreur 421 n'a pas été détectée"
    fi
    
    # Test 2: PRIVMSG sans destinataire
    print_test "PRIVMSG sans destinataire"
    send_cmd "$fifo" "PRIVMSG"
    sleep 1
    if grep -q "411" "/tmp/irc_output_edge1_$$" 2>/dev/null; then
        print_success "Erreur 411 reçue (pas de destinataire)"
    else
        print_error "L'erreur 411 n'a pas été détectée"
    fi
    
    # Test 3: PRIVMSG sans texte
    print_test "PRIVMSG sans texte"
    send_cmd "$fifo" "PRIVMSG #test"
    sleep 1
    if grep -q "412" "/tmp/irc_output_edge1_$$" 2>/dev/null; then
        print_success "Erreur 412 reçue (pas de texte)"
    else
        print_error "L'erreur 412 n'a pas été détectée"
    fi
    
    # Test 4: MODE sans paramètres suffisants
    print_test "MODE +k sans mot de passe"
    send_cmd "$fifo" "JOIN #edge"
    sleep 0.5
    send_cmd "$fifo" "MODE #edge +k"
    sleep 1
    if grep -q "461" "/tmp/irc_output_edge1_$$" 2>/dev/null; then
        print_success "Erreur 461 reçue (pas assez de paramètres)"
    else
        print_error "L'erreur 461 n'a pas été détectée"
    fi
    
    close_client "$fifo" "$pid"
}

# ============================================================================
# Nettoyage
# ============================================================================

cleanup() {
    print_info "Nettoyage des fichiers temporaires..."
    rm -f /tmp/irc_test_*_$$ /tmp/irc_output_*_$$ 2>/dev/null
    print_success "Nettoyage terminé"
}

# ============================================================================
# Rapport final
# ============================================================================

print_report() {
    echo ""
    print_header "RAPPORT FINAL"
    
    echo -e "${BOLD}Total de tests:${NC} $TOTAL_TESTS"
    echo -e "${GREEN}${BOLD}Tests réussis:${NC} $PASSED_TESTS"
    echo -e "${RED}${BOLD}Tests échoués:${NC} $FAILED_TESTS"
    
    if [ $FAILED_TESTS -eq 0 ]; then
        echo -e "\n${GREEN}${BOLD}🎉 TOUS LES TESTS SONT PASSÉS ! 🎉${NC}\n"
        return 0
    else
        echo -e "\n${RED}${BOLD}⚠️  CERTAINS TESTS ONT ÉCHOUÉ ⚠️${NC}\n"
        return 1
    fi
}

# ============================================================================
# Main
# ============================================================================

main() {
    clear
    echo -e "${BOLD}${CYAN}"
    echo "╔═══════════════════════════════════════════════════════════╗"
    echo "║                                                           ║"
    echo "║         SCRIPT DE TEST AUTOMATISÉ - SERVEUR IRC          ║"
    echo "║                                                           ║"
    echo "╚═══════════════════════════════════════════════════════════╝"
    echo -e "${NC}\n"
    
    print_info "Serveur: $SERVER:$PORT"
    print_info "Mot de passe: $PASSWORD"
    echo ""
    
    # Vérifier que le serveur est accessible
    check_server
    
    # Lancer les tests
    test_authentication
    test_join_part
    test_privmsg
    test_mode
    test_invite
    test_topic
    test_kick
    test_bot
    test_edge_cases
    
    # Nettoyage
    cleanup
    
    # Rapport final
    print_report
}

# Trap pour nettoyer en cas d'interruption
trap cleanup EXIT INT TERM

# Lancer le main
main

exit $?
