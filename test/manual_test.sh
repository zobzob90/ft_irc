#!/bin/bash

# Script de test manuel avec netcat
# Utiliser ce script pour tester manuellement le serveur IRC

PORT=6667
PASSWORD="testpass"

echo "======================================"
echo "Test manuel ft_irc avec netcat"
echo "======================================"
echo ""
echo "Serveur: localhost:$PORT"
echo "Password: $PASSWORD"
echo ""
echo "Commandes IRC de base:"
echo "  PASS $PASSWORD"
echo "  NICK yournick"
echo "  USER username 0 * :Real Name"
echo "  JOIN #channel"
echo "  PRIVMSG #channel :Hello!"
echo "  QUIT :Goodbye"
echo ""
echo "======================================"
echo ""

# Se connecter au serveur
nc localhost $PORT
