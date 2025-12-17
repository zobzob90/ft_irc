#!/bin/bash

# Script pour tester les fuites mémoire avec Valgrind

PORT=6667
PASSWORD="testpass"

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

echo -e "${BLUE}======================================"
echo -e "ft_irc - Test Valgrind"
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

echo -e "${GREEN}✓ Démarrage du serveur avec Valgrind...${NC}"
echo -e "${BLUE}Port: $PORT, Password: $PASSWORD${NC}\n"

echo -e "${YELLOW}Le serveur va démarrer avec Valgrind.${NC}"
echo -e "${YELLOW}Dans un autre terminal, lancez:${NC}"
echo -e "${GREEN}  python3 test_edge_cases.py${NC}"
echo -e "${YELLOW}ou${NC}"
echo -e "${GREEN}  ./manual_test.sh${NC}\n"

echo -e "${YELLOW}Appuyez sur Ctrl+C pour arrêter le serveur et voir le rapport Valgrind${NC}\n"

# Lancer le serveur avec valgrind
valgrind \
    --leak-check=full \
    --show-leak-kinds=all \
    --track-origins=yes \
    --verbose \
    --log-file=valgrind-output.txt \
    ../ircserv $PORT $PASSWORD

echo -e "\n${GREEN}✓ Rapport Valgrind sauvegardé dans: valgrind-output.txt${NC}"
echo -e "${BLUE}Affichage du résumé:${NC}\n"

# Afficher le résumé
if [ -f "valgrind-output.txt" ]; then
    echo -e "${YELLOW}=== RÉSUMÉ ===${NC}"
    grep -A 50 "HEAP SUMMARY" valgrind-output.txt || echo "Pas de résumé trouvé"
    
    echo -e "\n${YELLOW}=== FUITES DÉTECTÉES ===${NC}"
    grep -A 20 "LEAK SUMMARY" valgrind-output.txt || echo "Pas de fuites détectées"
    
    # Vérifier s'il y a des erreurs
    ERRORS=$(grep "ERROR SUMMARY" valgrind-output.txt | grep -o "[0-9]* errors" | grep -o "[0-9]*")
    if [ "$ERRORS" = "0" ]; then
        echo -e "\n${GREEN}✓ Aucune erreur Valgrind détectée!${NC}"
    else
        echo -e "\n${RED}✗ $ERRORS erreurs détectées${NC}"
        echo -e "${YELLOW}Voir valgrind-output.txt pour les détails${NC}"
    fi
fi
