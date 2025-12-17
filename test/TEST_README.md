# Tests Edge Cases - ft_irc

Ce repository contient une suite de tests complète pour tester un maximum de cas limites du projet ft_irc de 42.

## 📋 Tests Inclus

### 🔐 Tests d'Authentification (Tests 1-4)
- ✓ Mauvais mot de passe
- ✓ Connexion sans PASS
- ✓ Ordre incorrect des commandes (USER avant PASS)
- ✓ Commande PASS envoyée plusieurs fois

### 👤 Tests de Nicknames (Tests 5-7)
- ✓ Nicknames avec caractères invalides (@, #, espaces, etc.)
- ✓ Nicknames commençant par un chiffre
- ✓ Nicknames trop longs
- ✓ Nicknames vides
- ✓ Tentative d'utiliser un nickname déjà pris
- ✓ Changement de nickname pendant la session

### 📺 Tests de Channels (Tests 8-13)
- ✓ Noms de channels invalides (sans #, avec espaces, trop longs)
- ✓ Mode +i (invite-only)
- ✓ Mode +t (topic protection)
- ✓ Mode +k (password/key)
- ✓ Mode +l (user limit)
- ✓ Mode +o (operator)
- ✓ Modes invalides
- ✓ Channel protégé par clé (avec/sans bonne clé)
- ✓ Limite d'utilisateurs (channel plein)
- ✓ Mode invite-only (avec/sans invitation)

### 💬 Tests PRIVMSG (Tests 14-15)
- ✓ PRIVMSG sans destinataire
- ✓ PRIVMSG sans message
- ✓ PRIVMSG vers utilisateur inexistant
- ✓ PRIVMSG vers channel inexistant
- ✓ Messages très longs (500+ caractères)
- ✓ Messages avec caractères spéciaux et émojis
- ✓ PRIVMSG vers channel sans être membre

### ⚡ Tests des Opérations (Tests 16-24)
- ✓ KICK sans raison
- ✓ KICK avec raison
- ✓ KICK d'utilisateur inexistant
- ✓ KICK de soi-même
- ✓ KICK sans privilèges d'opérateur
- ✓ TOPIC (définir, consulter, supprimer)
- ✓ TOPIC très long
- ✓ TOPIC avec mode +t par non-op
- ✓ PART de channel non rejoint
- ✓ PART avec/sans raison
- ✓ PART de plusieurs channels
- ✓ INVITE d'utilisateur inexistant
- ✓ INVITE vers channel inexistant
- ✓ INVITE sans être opérateur
- ✓ INVITE d'utilisateur déjà présent
- ✓ Combinaisons de modes (+it, +kl, etc.)

### 🔥 Tests de Stress (Tests 17-25)
- ✓ Rejoindre plusieurs channels (10+)
- ✓ Commandes rapides en rafale (20+)
- ✓ Tentatives de buffer overflow (messages 10000+ chars)
- ✓ Commandes énormes
- ✓ Nombreux paramètres
- ✓ Caractères spéciaux (NULL bytes, \r\n, UTF-8, émojis)
- ✓ Déconnexion brutale (sans QUIT)
- ✓ Déconnexion avec QUIT
- ✓ QUIT avec message long
- ✓ Commandes vides
- ✓ Lignes vides multiples
- ✓ Espaces seulement
- ✓ Commandes sans paramètres requis

### 🤖 Tests Bot (Test 26)
- ✓ Interaction avec le bot BadWord Filter
- ✓ Messages normaux
- ✓ Messages avec mots interdits

## 🚀 Utilisation

### Méthode 1 : Script automatique (recommandé)

```bash
# Rendre le script exécutable
chmod +x run_tests.sh

# Lancer les tests (démarre automatiquement le serveur)
./run_tests.sh
```

### Méthode 2 : Manuel

```bash
# Terminal 1 : Démarrer le serveur
./ircserv 6667 testpass

# Terminal 2 : Lancer les tests
python3 test_edge_cases.py
```

## ⚙️ Configuration

Vous pouvez modifier les paramètres de connexion dans `test_edge_cases.py` :

```python
SERVER_HOST = "127.0.0.1"  # Adresse du serveur
SERVER_PORT = 6667          # Port du serveur
SERVER_PASS = "testpass"    # Mot de passe du serveur
```

## 📊 Résultats

Les tests affichent :
- ✓ en **vert** : Tests réussis avec comportement attendu
- ⚠ en **jaune** : Avertissements (à vérifier)
- ✗ en **rouge** : Erreurs ou échecs
- ℹ en **cyan** : Informations sur les réponses du serveur

## 📝 Notes Importantes

### Ce que les tests vérifient :
1. **Sécurité** : Authentification, privilèges, protections
2. **RFC Compliance** : Codes d'erreur IRC standard (461, 433, 471, etc.)
3. **Robustesse** : Gestion des cas limites et erreurs
4. **Performance** : Gestion de charge et commandes rapides
5. **Fonctionnalités** : Tous les modes de channel requis

### Points critiques pour ft_irc de 42 :
- ✓ Authentification obligatoire (PASS/NICK/USER)
- ✓ Modes de channel (+i, +t, +k, +l, +o)
- ✓ Commandes : JOIN, PART, KICK, INVITE, TOPIC, MODE, PRIVMSG, QUIT
- ✓ Gestion multi-clients
- ✓ Pas de memory leaks (utiliser valgrind)
- ✓ Gestion propre des déconnexions

## 🐛 Debugging

Si les tests échouent :

1. **Vérifier les logs du serveur** : Observer les messages d'erreur côté serveur
2. **Tester manuellement avec netcat** :
   ```bash
   nc localhost 6667
   PASS testpass
   NICK testuser
   USER testuser 0 * :Test User
   ```
3. **Vérifier avec valgrind** :
   ```bash
   valgrind --leak-check=full --show-leak-kinds=all ./ircserv 6667 testpass
   ```

## 📚 Ressources

- [RFC 1459 - Internet Relay Chat Protocol](https://tools.ietf.org/html/rfc1459)
- [RFC 2812 - IRC Client Protocol](https://tools.ietf.org/html/rfc2812)
- [Modern IRC Documentation](https://modern.ircdocs.horse/)

## 🎯 Checklist pour la correction

- [ ] Le serveur compile sans warnings
- [ ] Pas de memory leaks (valgrind)
- [ ] Gestion correcte de l'authentification
- [ ] Tous les modes de channel fonctionnent
- [ ] Gestion multi-clients stable
- [ ] Codes d'erreur IRC corrects
- [ ] Déconnexions propres
- [ ] Résistance aux buffer overflow
- [ ] Bot fonctionnel
- [ ] Pas de crash avec caractères spéciaux

## 💡 Tips

- Tester avec **plusieurs clients simultanés** (irssi, weechat, hexchat)
- Vérifier les **fuites mémoire** après chaque test
- Tester avec **valgrind** et **helgrind** (threads)
- Utiliser **nc (netcat)** pour tester manuellement
- Lire les **RFC** pour les cas ambigus

---

**Bon courage pour votre ft_irc ! 🚀**
