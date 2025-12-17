#!/bin/bash

# Script pour tester automatiquement avec Valgrind

PORT=6667
PASSWORD="testpass"

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m'

echo -e "${CYAN}======================================"
echo -e "ft_irc - Test Valgrind Automatisé"
echo -e "======================================${NC}\n"

# Vérifier si valgrind est installé
if ! command -v valgrind &> /dev/null; then
    echo -e "${RED}✗ Valgrind n'est pas installé${NC}"
    echo -e "${YELLOW}Installation: sudo apt-get install valgrind${NC}"
    exit 1
fi

# Vérifier si le serveur est compilé
if [ ! -f "../ircserv" ]; then
    echo -e "${YELLOW}⚠ Compilation du serveur...${NC}"
    cd .. && make && cd test
    if [ $? -ne 0 ]; then
        echo -e "${RED}✗ Erreur de compilation${NC}"
        exit 1
    fi
fi

echo -e "${GREEN}✓ Lancement du serveur avec Valgrind...${NC}"
echo -e "${BLUE}Port: $PORT, Password: $PASSWORD${NC}\n"

# Nettoyer les anciens fichiers
rm -f valgrind-output.txt

# Lancer le serveur avec valgrind en arrière-plan
valgrind \
    --leak-check=full \
    --show-leak-kinds=all \
    --track-origins=yes \
    --verbose \
    --log-file=valgrind-output.txt \
    ../ircserv $PORT $PASSWORD &

SERVER_PID=$!

echo -e "${YELLOW}Serveur PID: $SERVER_PID${NC}"
echo -e "${YELLOW}Attente du démarrage (5 secondes)...${NC}\n"
sleep 5

# Vérifier si le serveur tourne
if ! ps -p $SERVER_PID > /dev/null; then
    echo -e "${RED}✗ Le serveur n'a pas démarré${NC}"
    exit 1
fi

echo -e "${GREEN}✓ Serveur démarré${NC}"
echo -e "${CYAN}Lancement des tests...${NC}\n"

# Lancer les tests
python3 test_edge_cases.py

echo -e "\n${YELLOW}Arrêt du serveur...${NC}"
kill -SIGINT $SERVER_PID 2>/dev/null
sleep 2

# Si le serveur ne s'est pas arrêté proprement
if ps -p $SERVER_PID > /dev/null 2>&1; then
    echo -e "${YELLOW}Arrêt forcé...${NC}"
    kill -9 $SERVER_PID 2>/dev/null
fi

wait $SERVER_PID 2>/dev/null

echo -e "\n${CYAN}======================================"
echo -e "ANALYSE VALGRIND"
echo -e "======================================${NC}\n"

# Attendre que Valgrind termine d'écrire
sleep 2

if [ ! -f "valgrind-output.txt" ]; then
    echo -e "${RED}✗ Fichier valgrind-output.txt introuvable${NC}"
    exit 1
fi

# Afficher le résumé
echo -e "${YELLOW}=== HEAP SUMMARY ===${NC}"
grep -A 10 "HEAP SUMMARY" valgrind-output.txt | head -15

echo -e "\n${YELLOW}=== LEAK SUMMARY ===${NC}"
grep -A 10 "LEAK SUMMARY" valgrind-output.txt | head -15

echo -e "\n${YELLOW}=== ERROR SUMMARY ===${NC}"
grep "ERROR SUMMARY" valgrind-output.txt

# Analyse détaillée
echo -e "\n${CYAN}======================================"
echo -e "DÉTAILS"
echo -e "======================================${NC}\n"

# Vérifier les erreurs
ERRORS=$(grep "ERROR SUMMARY" valgrind-output.txt | grep -o "[0-9]* errors" | grep -o "[0-9]*" | head -1)
DEFINITELY_LOST=$(grep "definitely lost" valgrind-output.txt | grep -o "[0-9,]* bytes" | head -1 | grep -o "[0-9,]*" | tr -d ',')
INDIRECTLY_LOST=$(grep "indirectly lost" valgrind-output.txt | grep -o "[0-9,]* bytes" | head -1 | grep -o "[0-9,]*" | tr -d ',')
POSSIBLY_LOST=$(grep "possibly lost" valgrind-output.txt | grep -o "[0-9,]* bytes" | head -1 | grep -o "[0-9,]*" | tr -d ',')
STILL_REACHABLE=$(grep "still reachable" valgrind-output.txt | grep -o "[0-9,]* bytes" | head -1 | grep -o "[0-9,]*" | tr -d ',')

echo -e "${BLUE}Erreurs Valgrind:${NC} ${ERRORS:-0}"
echo -e "${BLUE}Definitely lost:${NC} ${DEFINITELY_LOST:-0} bytes"
echo -e "${BLUE}Indirectly lost:${NC} ${INDIRECTLY_LOST:-0} bytes"
echo -e "${BLUE}Possibly lost:${NC} ${POSSIBLY_LOST:-0} bytes"
echo -e "${BLUE}Still reachable:${NC} ${STILL_REACHABLE:-0} bytes"

echo -e "\n${CYAN}======================================"
echo -e "VERDICT"
echo -e "======================================${NC}\n"

# Verdict final
CRITICAL=0

if [ "${ERRORS:-0}" != "0" ]; then
    echo -e "${RED}✗ Des erreurs Valgrind ont été détectées${NC}"
    CRITICAL=1
fi

if [ "${DEFINITELY_LOST:-0}" != "0" ]; then
    echo -e "${RED}✗ Fuites mémoire critiques détectées (definitely lost)${NC}"
    CRITICAL=1
fi

if [ "${INDIRECTLY_LOST:-0}" != "0" ]; then
    echo -e "${YELLOW}⚠ Fuites indirectes détectées (indirectly lost)${NC}"
fi

if [ "${POSSIBLY_LOST:-0}" != "0" ]; then
    echo -e "${YELLOW}⚠ Fuites possibles détectées (possibly lost)${NC}"
fi

if [ "${STILL_REACHABLE:-0}" != "0" ]; then
    echo -e "${BLUE}ℹ Mémoire encore accessible à la sortie (still reachable)${NC}"
    echo -e "${BLUE}  → Pas critique, mais devrait être libérée${NC}"
fi

if [ $CRITICAL -eq 0 ]; then
    if [ "${DEFINITELY_LOST:-0}" = "0" ] && [ "${ERRORS:-0}" = "0" ]; then
        echo -e "\n${GREEN}✓✓✓ PARFAIT ! Aucune fuite critique détectée !${NC}"
    fi
fi

echo -e "\n${YELLOW}Rapport complet: ${NC}valgrind-output.txt"
echo -e "${YELLOW}Commande pour voir les détails:${NC}"
echo -e "${GREEN}  cat valgrind-output.txt | less${NC}"
echo -e "${GREEN}  grep 'definitely lost' valgrind-output.txt -A 10${NC}\n"

exit 0
