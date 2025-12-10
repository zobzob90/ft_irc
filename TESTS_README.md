# 🎉 Scripts de Test IRC - Guide Complet

Félicitations ! Vous disposez maintenant d'une suite complète de scripts de test pour votre serveur IRC.

---

## 📁 Fichiers créés

| Fichier | Description | Usage |
|---------|-------------|-------|
| `test_irc.sh` | **Tests automatisés complets** | `./test_irc.sh 6667 password` |
| `manual_test.sh` | **Tests manuels interactifs** | `./manual_test.sh 6667 password` |
| `stress_test.sh` | **Tests de charge** | `./stress_test.sh 6667 password 20` |
| `IRC_COMMANDS.sh` | **Guide de référence** | `./IRC_COMMANDS.sh` |
| `TESTING.md` | **Documentation complète** | (lecture) |

---

## 🚀 Démarrage rapide

### 1️⃣ Lancer le serveur
```bash
make re
./ircserv 6667 testpass
```

### 2️⃣ Lancer les tests automatisés
```bash
# Dans un autre terminal
./test_irc.sh 6667 testpass
```

### 3️⃣ Voir le guide de commandes
```bash
./IRC_COMMANDS.sh
```

---

## 📊 Ce qui est testé

### ✅ test_irc.sh (Tests automatisés)
- ✓ Authentification (PASS, NICK, USER)
- ✓ JOIN / PART
- ✓ PRIVMSG (channel et privé)
- ✓ MODE (tous les modes: i, t, k, l, o)
- ✓ INVITE
- ✓ TOPIC
- ✓ KICK
- ✓ BOT (bienvenue, commandes, kick auto)
- ✓ Cas limites et erreurs

**Total: ~45 tests**

### 🔥 stress_test.sh (Tests de charge)
- ✓ Connexions multiples simultanées (10+ clients)
- ✓ Spam de messages (100 messages rapides)
- ✓ Changements de modes rapides
- ✓ Déconnexions brutales

### 🎮 manual_test.sh (Tests interactifs)
- Session interactive pour tester manuellement
- Authentification automatique
- Saisie libre de commandes

---

## 📈 Exemples de sortie

### Tests réussis ✅
```
═══════════════════════════════════════════════════
  TESTS D'AUTHENTIFICATION
═══════════════════════════════════════════════════

[TEST] Tentative avec mauvais mot de passe
→ PASS wrongpassword
[✓] Erreur 464 reçue (mot de passe incorrect)

[TEST] Authentification complète réussie
→ PASS testpass
→ NICK Alice
→ USER alice 0 * :Alice Smith
[✓] Code 001 reçu (bienvenue)

...

RAPPORT FINAL
═══════════════════════════════════════════════════
Total de tests: 45
Tests réussis: 45
Tests échoués: 0

🎉 TOUS LES TESTS SONT PASSÉS ! 🎉
```

### Tests de charge 🔥
```
═══════════════════════════════════════════════════
  TEST DE CONNEXIONS MULTIPLES
═══════════════════════════════════════════════════

[INFO] Lancement de 10 clients simultanés...
[INFO] Client 1 lancé (PID: 12345)
[INFO] Client 2 lancé (PID: 12346)
...
[✓] 10 clients lancés
[INFO] Attente de la fin des clients...
[✓] Tous les clients ont terminé
[INFO] Durée totale: 8s
[INFO] Clients connectés avec succès: 10 / 10
[✓] ✓ TOUS LES CLIENTS ONT RÉUSSI À SE CONNECTER
```

---

## 🎯 Utilisation détaillée

### test_irc.sh - Tests automatisés

**Syntaxe:**
```bash
./test_irc.sh [port] [password]
```

**Exemples:**
```bash
./test_irc.sh                    # Port 6667, password "testpass"
./test_irc.sh 6667               # Port 6667, password "testpass"
./test_irc.sh 6667 mypass        # Port 6667, password "mypass"
```

**Options:**
- Tous les tests sont lancés automatiquement
- Les résultats sont colorés pour faciliter la lecture
- Un rapport final est généré
- Les fichiers temporaires sont automatiquement nettoyés

### manual_test.sh - Tests interactifs

**Syntaxe:**
```bash
./manual_test.sh [port] [password]
```

**Usage:**
```bash
./manual_test.sh 6667 testpass
# Puis taper vos commandes:
JOIN #test
MODE #test +i
PRIVMSG #test :Hello!
QUIT
```

### stress_test.sh - Tests de charge

**Syntaxe:**
```bash
./stress_test.sh [port] [password] [nombre_clients]
```

**Exemples:**
```bash
./stress_test.sh 6667 testpass 5     # 5 clients
./stress_test.sh 6667 testpass 20    # 20 clients
./stress_test.sh 6667 testpass 50    # 50 clients (stress!)
```

### IRC_COMMANDS.sh - Guide de référence

**Syntaxe:**
```bash
./IRC_COMMANDS.sh
```

Affiche un guide complet avec:
- Toutes les commandes IRC supportées
- Exemples pratiques
- Scénarios d'utilisation
- Codes de réponse IRC
- Commandes du bot

---

## 🐛 Déboguer les problèmes

### Le serveur ne répond pas
```bash
# Vérifier que le serveur est lancé
ps aux | grep ircserv

# Vérifier le port
netstat -tuln | grep 6667
```

### Les tests échouent
```bash
# Examiner les fichiers de sortie
ls /tmp/irc_output_*

# Voir un fichier spécifique
cat /tmp/irc_output_badpass_*

# Voir les logs du serveur
# (dans le terminal où tourne le serveur)
```

### Netcat (nc) non installé
```bash
# Debian/Ubuntu
sudo apt-get install netcat

# Fedora/RedHat
sudo dnf install nc

# Arch
sudo pacman -S openbsd-netcat
```

---

## 📝 Personnalisation

### Modifier le délai entre commandes

Dans `test_irc.sh`, ligne 19:
```bash
SLEEP_TIME=0.5  # Augmenter si nécessaire
```

### Ajouter un nouveau test

1. Créer une fonction dans `test_irc.sh`:
```bash
test_my_feature() {
    print_header "MON NOUVEAU TEST"
    
    print_test "Description"
    client=$(create_client "test1")
    IFS=':' read -r fifo pid <<< "$client"
    
    # Authentification
    send_cmd "$fifo" "PASS $PASSWORD"
    send_cmd "$fifo" "NICK TestUser"
    send_cmd "$fifo" "USER test 0 * :Test"
    sleep 0.5
    
    # Votre test
    send_cmd "$fifo" "MA_COMMANDE"
    sleep 1
    
    # Vérification
    if grep -q "RESULTAT" "/tmp/irc_output_test1_$$"; then
        print_success "OK"
    else
        print_error "FAIL"
    fi
    
    close_client "$fifo" "$pid"
}
```

2. Appeler la fonction dans `main()`:
```bash
test_my_feature
```

---

## 🎓 Bonnes pratiques

### Avant de commit
```bash
# Lancer tous les tests
./test_irc.sh

# Vérifier les leaks
valgrind --leak-check=full ./ircserv 6667 pass

# Test de charge
./stress_test.sh 6667 pass 20
```

### Avant la correction
```bash
# Tests automatisés
./test_irc.sh

# Tester avec un vrai client IRC
irssi -c localhost -p 6667

# Vérifier la compilation
make re
make clean && make
```

### Pendant le développement
```bash
# Tests rapides
./manual_test.sh

# Consulter le guide
./IRC_COMMANDS.sh

# Test d'une fonctionnalité spécifique
# (modifier test_irc.sh pour désactiver d'autres tests)
```

---

## 📚 Documentation complète

Pour plus de détails, consultez:
- **TESTING.md** - Documentation complète des tests
- **MODE_REFACTORING.md** - Détails sur la refactorisation de MODE
- **README.md** - Documentation générale du projet

---

## 🏆 Checklist finale

Avant de soumettre votre projet:

- [ ] `make re` compile sans warnings
- [ ] `make clean` fonctionne
- [ ] `./test_irc.sh` passe tous les tests
- [ ] `./stress_test.sh` passe avec 20+ clients
- [ ] Aucun leak mémoire (valgrind)
- [ ] Testé avec un vrai client IRC (irssi/WeeChat)
- [ ] Le Bot fonctionne correctement
- [ ] Tous les modes (i, t, k, l, o) fonctionnent
- [ ] INVITE, TOPIC, MODE implémentés
- [ ] Gestion propre de Ctrl+C (SIGINT)
- [ ] Code commenté et propre

---

## 🎉 Résultat

Vous disposez maintenant d'une **suite de tests professionnelle** pour votre serveur IRC !

**Points forts:**
✅ Tests automatisés complets (~45 tests)
✅ Tests de charge (stress test)
✅ Tests manuels interactifs
✅ Documentation complète
✅ Guide de référence des commandes
✅ Scripts colorés et lisibles
✅ Nettoyage automatique
✅ Rapports détaillés

**Commandes rapides:**
```bash
# Tests complets
./test_irc.sh 6667 testpass

# Guide de commandes
./IRC_COMMANDS.sh

# Tests de charge
./stress_test.sh 6667 testpass 20

# Test manuel
./manual_test.sh 6667 testpass
```

---

**Bon courage pour votre projet ! 🚀**

*N'oubliez pas de lancer le serveur avant les tests !*
```bash
./ircserv 6667 testpass
```
