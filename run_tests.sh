#!/bin/bash

# Script pour lancer le serveur et les tests ft_irc

PORT=6667
PASSWORD="testpass"

# Couleurs
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${BLUE}================================${NC}"
echo -e "${BLUE}FT_IRC - Test Runner${NC}"
echo -e "${BLUE}================================${NC}\n"

# Vérifier si le serveur est compilé
if [ ! -f "./ircserv" ]; then
    echo -e "${YELLOW}⚠ Le serveur n'est pas compilé${NC}"
    echo -e "${GREEN}Compilation en cours...${NC}"
    make
    if [ $? -ne 0 ]; then
        echo -e "${RED}✗ Erreur de compilation${NC}"
        exit 1
    fi
fi

# Vérifier si Python3 est installé
if ! command -v python3 &> /dev/null; then
    echo -e "${RED}✗ Python3 n'est pas installé${NC}"
    exit 1
fi

echo -e "${GREEN}✓ Tout est prêt${NC}\n"

# Fonction pour nettoyer à la sortie
cleanup() {
    echo -e "\n${YELLOW}Arrêt du serveur...${NC}"
    if [ ! -z "$SERVER_PID" ]; then
        kill $SERVER_PID 2>/dev/null
        wait $SERVER_PID 2>/dev/null
    fi
    echo -e "${GREEN}✓ Serveur arrêté${NC}"
    exit 0
}

trap cleanup SIGINT SIGTERM

# Démarrer le serveur en arrière-plan
echo -e "${BLUE}Démarrage du serveur IRC...${NC}"
echo -e "${BLUE}Port: ${PORT}, Password: ${PASSWORD}${NC}\n"

./ircserv $PORT $PASSWORD &
SERVER_PID=$!

# Attendre que le serveur démarre
echo -e "${YELLOW}Attente du démarrage du serveur (3 secondes)...${NC}"
sleep 3

# Vérifier si le serveur tourne
if ! ps -p $SERVER_PID > /dev/null; then
    echo -e "${RED}✗ Le serveur n'a pas démarré correctement${NC}"
    exit 1
fi

echo -e "${GREEN}✓ Serveur démarré (PID: $SERVER_PID)${NC}\n"

# Rendre le script Python exécutable
chmod +x test_edge_cases.py

# Lancer les tests
echo -e "${BLUE}Lancement des tests...${NC}\n"
python3 test_edge_cases.py

# Attendre un peu avant de nettoyer
sleep 2

# Nettoyer
cleanup
