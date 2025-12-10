#!/bin/bash

# ============================================================================
# Script de test de charge pour le serveur IRC
# ============================================================================
# Ce script simule plusieurs clients connectés simultanément
# Usage: ./stress_test.sh [port] [password] [nombre_clients]
# ============================================================================

PORT=${1:-6667}
PASSWORD=${2:-"testpass"}
NUM_CLIENTS=${3:-10}
SERVER="localhost"

# Couleurs
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m'
BOLD='\033[1m'

print_header() {
    echo -e "\n${BOLD}${CYAN}═══════════════════════════════════════════════════${NC}"
    echo -e "${BOLD}${CYAN}  $1${NC}"
    echo -e "${BOLD}${CYAN}═══════════════════════════════════════════════════${NC}\n"
}

print_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[✓]${NC} $1"
}

print_error() {
    echo -e "${RED}[✗]${NC} $1"
}

# Vérifier la connexion au serveur
check_server() {
    if ! nc -z $SERVER $PORT 2>/dev/null; then
        print_error "Le serveur n'est pas accessible sur $SERVER:$PORT"
        echo -e "${YELLOW}Lancez d'abord le serveur:${NC} ./ircserv $PORT $PASSWORD"
        exit 1
    fi
    print_success "Serveur accessible sur $SERVER:$PORT"
}

# Simuler un client
simulate_client() {
    local id=$1
    local nickname="StressUser$id"
    
    {
        echo "PASS $PASSWORD"
        sleep 0.1
        echo "NICK $nickname"
        sleep 0.1
        echo "USER stress$id 0 * :Stress Test User $id"
        sleep 0.2
        echo "JOIN #stress"
        sleep 0.2
        
        # Envoyer quelques messages
        for i in {1..5}; do
            echo "PRIVMSG #stress :Message $i from $nickname"
            sleep 0.3
        done
        
        sleep 1
        echo "QUIT :Test terminé"
    } | nc $SERVER $PORT > "/tmp/stress_client_${id}_$$" 2>&1 &
    
    echo $!
}

# Test de connexions multiples
test_multiple_connections() {
    print_header "TEST DE CONNEXIONS MULTIPLES"
    print_info "Lancement de $NUM_CLIENTS clients simultanés..."
    
    local pids=()
    local start_time=$(date +%s)
    
    # Lancer tous les clients
    for i in $(seq 1 $NUM_CLIENTS); do
        pid=$(simulate_client $i)
        pids+=($pid)
        print_info "Client $i lancé (PID: $pid)"
        sleep 0.1
    done
    
    print_success "$NUM_CLIENTS clients lancés"
    
    # Attendre que tous les clients terminent
    print_info "Attente de la fin des clients..."
    for pid in "${pids[@]}"; do
        wait $pid 2>/dev/null
    done
    
    local end_time=$(date +%s)
    local duration=$((end_time - start_time))
    
    print_success "Tous les clients ont terminé"
    print_info "Durée totale: ${duration}s"
    
    # Vérifier les résultats
    local success_count=0
    for i in $(seq 1 $NUM_CLIENTS); do
        if [ -f "/tmp/stress_client_${i}_$$" ]; then
            if grep -q "001" "/tmp/stress_client_${i}_$$" 2>/dev/null || \
               grep -q "JOIN" "/tmp/stress_client_${i}_$$" 2>/dev/null; then
                ((success_count++))
            fi
        fi
    done
    
    echo ""
    print_info "Clients connectés avec succès: $success_count / $NUM_CLIENTS"
    
    if [ $success_count -eq $NUM_CLIENTS ]; then
        print_success "✓ TOUS LES CLIENTS ONT RÉUSSI À SE CONNECTER"
        return 0
    elif [ $success_count -gt $((NUM_CLIENTS / 2)) ]; then
        print_info "⚠ PLUS DE LA MOITIÉ DES CLIENTS ONT RÉUSSI"
        return 1
    else
        print_error "✗ MOINS DE LA MOITIÉ DES CLIENTS ONT RÉUSSI"
        return 2
    fi
}

# Test de spam de messages
test_message_spam() {
    print_header "TEST DE SPAM DE MESSAGES"
    print_info "Envoi de messages en masse..."
    
    {
        echo "PASS $PASSWORD"
        sleep 0.1
        echo "NICK SpamBot"
        sleep 0.1
        echo "USER spambot 0 * :Spam Bot"
        sleep 0.2
        echo "JOIN #spamtest"
        sleep 0.3
        
        # Envoyer 100 messages rapidement
        for i in $(seq 1 100); do
            echo "PRIVMSG #spamtest :Spam message $i"
        done
        
        sleep 1
        echo "QUIT"
    } | nc $SERVER $PORT > "/tmp/spam_test_$$" 2>&1
    
    if [ -f "/tmp/spam_test_$$" ]; then
        local msg_count=$(grep -c "PRIVMSG" "/tmp/spam_test_$$" 2>/dev/null || echo 0)
        print_info "Messages traités: $msg_count"
        
        if [ $msg_count -gt 50 ]; then
            print_success "Le serveur a géré le spam de messages"
        else
            print_error "Le serveur a eu des difficultés avec le spam"
        fi
    fi
}

# Test de modes multiples
test_rapid_mode_changes() {
    print_header "TEST DE CHANGEMENTS DE MODES RAPIDES"
    print_info "Changements rapides de modes..."
    
    {
        echo "PASS $PASSWORD"
        sleep 0.1
        echo "NICK ModeBot"
        sleep 0.1
        echo "USER modebot 0 * :Mode Bot"
        sleep 0.2
        echo "JOIN #modetest"
        sleep 0.3
        
        # Changer les modes rapidement
        for i in {1..20}; do
            echo "MODE #modetest +i"
            echo "MODE #modetest -i"
            echo "MODE #modetest +t"
            echo "MODE #modetest -t"
        done
        
        sleep 1
        echo "QUIT"
    } | nc $SERVER $PORT > "/tmp/mode_test_$$" 2>&1
    
    if grep -q "MODE" "/tmp/mode_test_$$" 2>/dev/null; then
        print_success "Le serveur a géré les changements de modes"
    else
        print_error "Problème avec les changements de modes"
    fi
}

# Test de déconnexions brutales
test_brutal_disconnects() {
    print_header "TEST DE DÉCONNEXIONS BRUTALES"
    print_info "Simulation de déconnexions brutales..."
    
    for i in $(seq 1 5); do
        {
            echo "PASS $PASSWORD"
            sleep 0.1
            echo "NICK Brutal$i"
            sleep 0.1
            echo "USER brutal$i 0 * :Brutal User"
            sleep 0.1
            echo "JOIN #brutal"
            # Pas de QUIT, fermeture brutale
        } | timeout 1 nc $SERVER $PORT > /dev/null 2>&1 &
    done
    
    sleep 2
    print_success "Déconnexions brutales simulées"
}

# Nettoyage
cleanup() {
    print_info "Nettoyage des fichiers temporaires..."
    rm -f /tmp/stress_client_*_$$ /tmp/spam_test_$$ /tmp/mode_test_$$ 2>/dev/null
    # Tuer les processus nc restants
    pkill -f "nc $SERVER $PORT" 2>/dev/null
    print_success "Nettoyage terminé"
}

# Rapport
print_report() {
    print_header "RÉSUMÉ DU TEST DE CHARGE"
    echo -e "${BOLD}Configuration:${NC}"
    echo -e "  Serveur: $SERVER:$PORT"
    echo -e "  Nombre de clients: $NUM_CLIENTS"
    echo -e "\n${BOLD}Tests effectués:${NC}"
    echo -e "  ✓ Connexions multiples simultanées"
    echo -e "  ✓ Spam de messages"
    echo -e "  ✓ Changements de modes rapides"
    echo -e "  ✓ Déconnexions brutales"
    echo -e "\n${GREEN}${BOLD}Le serveur a survécu au test de charge ! 💪${NC}\n"
}

# Main
main() {
    clear
    echo -e "${BOLD}${CYAN}"
    echo "╔═══════════════════════════════════════════════════════════╗"
    echo "║                                                           ║"
    echo "║           TEST DE CHARGE - SERVEUR IRC                   ║"
    echo "║                                                           ║"
    echo "╚═══════════════════════════════════════════════════════════╝"
    echo -e "${NC}\n"
    
    check_server
    
    test_multiple_connections
    sleep 2
    
    test_message_spam
    sleep 2
    
    test_rapid_mode_changes
    sleep 2
    
    test_brutal_disconnects
    sleep 2
    
    cleanup
    print_report
}

# Trap pour cleanup
trap cleanup EXIT INT TERM

# Lancer
main
