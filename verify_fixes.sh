#!/bin/bash

# Script de vérification rapide après correctifs

echo "╔══════════════════════════════════════════════════════════════╗"
echo "║          🔍 VÉRIFICATION DES CORRECTIFS ft_irc 🔍            ║"
echo "╚══════════════════════════════════════════════════════════════╝"
echo ""

# Couleurs
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

# Compteurs
TOTAL=0
PASSED=0

# Fonction de test
check() {
    TOTAL=$((TOTAL + 1))
    if [ $1 -eq 0 ]; then
        echo -e "${GREEN}✓${NC} $2"
        PASSED=$((PASSED + 1))
    else
        echo -e "${RED}✗${NC} $2"
    fi
}

echo -e "${BLUE}1. Vérification de la compilation...${NC}"
make re > /dev/null 2>&1
check $? "Compilation sans erreurs"
echo ""

echo -e "${BLUE}2. Vérification des fichiers modifiés...${NC}"

# Vérifier Bot.cpp
grep -q "std::string channelName = channel->getName()" srcs/bot/Bot.cpp
check $? "Bot.cpp: Fix use-after-free appliqué"

# Vérifier Channel_Management.cpp
grep -q "isValidChannelName" srcs/channel/Channel_Management.cpp
check $? "Channel_Management.cpp: Fonction de validation ajoutée"

grep -q "MAX_CHANNELS" srcs/channel/Channel_Management.cpp
check $? "Channel_Management.cpp: Limite MAX_CHANNELS définie"

# Vérifier Command_function.cpp (JOIN)
grep -q "if (!channel)" srcs/client/Command_function.cpp
check $? "Command_function.cpp: Vérification NULL après createChannel"

# Vérifier Command_function.cpp (NICK validation)
grep -q "too long" srcs/client/Command_function.cpp
check $? "Command_function.cpp: Validation longueur nickname ajoutée"

grep -q "cannot start with digit" srcs/client/Command_function.cpp
check $? "Command_function.cpp: Validation début par chiffre ajoutée"

echo ""
echo -e "${BLUE}3. Vérification de l'exécutable...${NC}"

if [ -f "./ircserv" ]; then
    check 0 "Exécutable ircserv existe"
    
    # Vérifier la taille (devrait être > 200KB avec les nouvelles validations)
    SIZE=$(stat -f%z "./ircserv" 2>/dev/null || stat -c%s "./ircserv" 2>/dev/null)
    if [ $SIZE -gt 200000 ]; then
        check 0 "Taille de l'exécutable correcte (${SIZE} bytes)"
    else
        check 1 "Taille de l'exécutable trop petite"
    fi
else
    check 1 "Exécutable ircserv existe"
fi

echo ""
echo -e "${BLUE}4. Vérification des fichiers de documentation...${NC}"

[ -f "ANALYSE_BUGS.md" ]
check $? "ANALYSE_BUGS.md créé"

[ -f "CORRECTIFS_APPLIQUES.md" ]
check $? "CORRECTIFS_APPLIQUES.md créé"

[ -f "test_edge_cases.py" ]
check $? "Suite de tests disponible"

echo ""
echo "╔══════════════════════════════════════════════════════════════╗"
echo -e "║  ${BLUE}RÉSULTAT${NC}: ${GREEN}${PASSED}${NC}/${TOTAL} vérifications passées              ║"

if [ $PASSED -eq $TOTAL ]; then
    echo -e "║  ${GREEN}✓ Tous les correctifs sont appliqués correctement !${NC}       ║"
else
    echo -e "║  ${YELLOW}⚠ Certains correctifs sont manquants${NC}                     ║"
fi

echo "╚══════════════════════════════════════════════════════════════╝"
echo ""

if [ $PASSED -eq $TOTAL ]; then
    echo -e "${GREEN}🎉 Le serveur est prêt pour les tests !${NC}"
    echo ""
    echo -e "${BLUE}Prochaines étapes:${NC}"
    echo "  1. Lancer les tests: ${YELLOW}./run_tests.sh${NC}"
    echo "  2. Tester avec Valgrind: ${YELLOW}./test_valgrind.sh${NC}"
    echo "  3. Test multi-clients: ${YELLOW}python3 test_multi_clients.py${NC}"
    exit 0
else
    echo -e "${RED}⚠️  Vérifiez les correctifs manquants${NC}"
    echo ""
    echo "Consultez ANALYSE_BUGS.md pour plus de détails"
    exit 1
fi
