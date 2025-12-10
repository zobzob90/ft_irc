#!/bin/bash

# ============================================================================
# Script de test manuel interactif pour le serveur IRC
# ============================================================================
# Usage: ./manual_test.sh [port] [password]
# Exemple: ./manual_test.sh 6667 mypassword
# ============================================================================

PORT=${1:-6667}
PASSWORD=${2:-"testpass"}
SERVER="localhost"

# Couleurs
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

echo -e "${BLUE}═══════════════════════════════════════════════════${NC}"
echo -e "${BLUE}     TEST MANUEL INTERACTIF - SERVEUR IRC${NC}"
echo -e "${BLUE}═══════════════════════════════════════════════════${NC}\n"

echo -e "${YELLOW}Connexion au serveur $SERVER:$PORT...${NC}\n"

# Créer une session temporaire avec les commandes pré-remplies
{
    echo "PASS $PASSWORD"
    sleep 0.2
    echo "NICK TestUser"
    sleep 0.2
    echo "USER testuser 0 * :Test User"
    sleep 0.5
    
    # Garder la connexion ouverte pour les commandes interactives
    cat
} | nc $SERVER $PORT

echo -e "\n${GREEN}Connexion fermée.${NC}"
