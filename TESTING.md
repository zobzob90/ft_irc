# 🧪 Scripts de Test IRC

Ce dossier contient des scripts de test pour votre serveur IRC.

---

## 📋 Fichiers disponibles

### 1. `test_irc.sh` - Tests automatisés complets
Script de test automatisé qui vérifie tous les aspects du serveur IRC.

### 2. `manual_test.sh` - Tests manuels interactifs
Script pour tester manuellement le serveur avec une connexion interactive.

---

## 🚀 Utilisation

### Préparation

1. **Rendre les scripts exécutables:**
```bash
chmod +x test_irc.sh manual_test.sh
```

2. **Compiler et lancer le serveur:**
```bash
make re
./ircserv 6667 testpass
```

### Tests automatisés

**Lancer tous les tests:**
```bash
./test_irc.sh 6667 testpass
```

**Arguments:**
- Premier argument: port (défaut: 6667)
- Deuxième argument: mot de passe (défaut: testpass)

**Exemple:**
```bash
./test_irc.sh          # Utilise les valeurs par défaut
./test_irc.sh 6667     # Port 6667, password par défaut
./test_irc.sh 6667 mypass  # Port et password personnalisés
```

### Tests manuels

**Lancer une session interactive:**
```bash
./manual_test.sh 6667 testpass
```

Une fois connecté, vous pouvez taper vos commandes:
```
JOIN #test
MODE #test
MODE #test +i
TOPIC #test :Mon nouveau topic
PRIVMSG #test :Hello everyone!
KICK #test Bob :Spam
INVITE Alice #test
QUIT :Bye
```

---

## 📊 Ce qui est testé

### ✅ Authentification
- [x] Mauvais mot de passe (erreur 464)
- [x] Authentification complète (PASS + NICK + USER)
- [x] Nickname déjà utilisé (erreur 433)

### ✅ JOIN / PART
- [x] JOIN basique
- [x] PART d'un channel
- [x] JOIN avec nom invalide (erreur 403)

### ✅ PRIVMSG
- [x] Message dans un channel
- [x] Message privé direct
- [x] Erreur sans destinataire (411)
- [x] Erreur sans texte (412)

### ✅ MODE
- [x] Affichage des modes (code 324)
- [x] Mode +i (invite only)
- [x] Mode +t (topic restrict)
- [x] Mode +k (password)
- [x] Mode +l (user limit)
- [x] Modes combinés (+it, -it)
- [x] Mode +o (operator)
- [x] Erreur paramètres manquants (461)

### ✅ INVITE
- [x] Invitation basique
- [x] JOIN après invitation
- [x] Erreur channel +i sans invitation (473)

### ✅ TOPIC
- [x] Définir un topic
- [x] Afficher un topic (code 332)
- [x] Mode +t restriction (erreur 482)

### ✅ KICK
- [x] KICK par un opérateur
- [x] Erreur KICK par non-opérateur (482)

### ✅ BOT
- [x] Message de bienvenue
- [x] Commande !ping
- [x] Commande !rules
- [x] Kick automatique pour mot interdit

### ✅ Cas limites
- [x] Commande inconnue (erreur 421)
- [x] Paramètres manquants (erreur 461)

---

## 📈 Interpréter les résultats

### Sortie des tests automatisés

```
[TEST] Description du test
→ Commande envoyée
[✓] Test réussi
[✗] Test échoué
[INFO] Information
```

### Codes couleur

- 🟢 **Vert**: Test réussi
- 🔴 **Rouge**: Test échoué
- 🟡 **Jaune**: Information/Test en cours
- 🔵 **Bleu**: Information générale

### Rapport final

```
RAPPORT FINAL
═══════════════════════════════════════════
Total de tests: 45
Tests réussis: 45
Tests échoués: 0

🎉 TOUS LES TESTS SONT PASSÉS ! 🎉
```

---

## 🐛 Déboguer les échecs

Si des tests échouent:

1. **Vérifier les logs du serveur**
   - Regarder la sortie du serveur pendant les tests

2. **Examiner les fichiers temporaires**
   - Les sorties sont dans `/tmp/irc_output_*`
   - Exemple: `cat /tmp/irc_output_badpass_*`

3. **Lancer les tests manuellement**
   - Utiliser `manual_test.sh` pour reproduire le problème
   - Ou utiliser directement `nc localhost 6667`

4. **Vérifier les codes d'erreur IRC**
   - 401: No such nick/channel
   - 403: No such channel
   - 411: No recipient given
   - 412: No text to send
   - 421: Unknown command
   - 431: No nickname given
   - 433: Nickname in use
   - 441: User not on channel
   - 442: You're not on that channel
   - 451: Not registered
   - 461: Not enough parameters
   - 462: Already registered
   - 464: Password incorrect
   - 471: Channel is full (+l)
   - 473: Invite only (+i)
   - 475: Bad channel key (+k)
   - 482: You're not channel operator

---

## 🔧 Personnalisation

### Modifier le délai entre les commandes

Dans `test_irc.sh`, ligne 19:
```bash
SLEEP_TIME=0.5  # Augmenter si le serveur est lent
```

### Ajouter de nouveaux tests

Créer une nouvelle fonction:
```bash
test_my_feature() {
    print_header "TESTS DE MA FONCTIONNALITÉ"
    
    print_test "Description de mon test"
    client=$(create_client "myclient")
    IFS=':' read -r fifo pid <<< "$client"
    
    # Authentification
    send_cmd "$fifo" "PASS $PASSWORD"
    send_cmd "$fifo" "NICK MyNick"
    send_cmd "$fifo" "USER myuser 0 * :My User"
    sleep 0.5
    
    # Votre test
    send_cmd "$fifo" "MA_COMMANDE param1 param2"
    sleep 1
    
    # Vérification
    if grep -q "EXPECTED_RESULT" "/tmp/irc_output_myclient_$$" 2>/dev/null; then
        print_success "Test réussi"
    else
        print_error "Test échoué"
    fi
    
    close_client "$fifo" "$pid"
}
```

Puis l'appeler dans `main()`:
```bash
test_my_feature
```

---

## 📝 Notes importantes

### Prérequis

- `nc` (netcat) doit être installé
- Le serveur doit être lancé **avant** d'exécuter les tests
- Les tests utilisent des fichiers temporaires dans `/tmp/`

### Limitations

- Les tests sont séquentiels (un par un)
- Certains tests nécessitent que le serveur soit "propre" (pas de channels existants)
- Les timeouts sont fixes (ajuster `SLEEP_TIME` si nécessaire)

### Nettoyage

Les fichiers temporaires sont automatiquement supprimés:
- À la fin du script
- En cas d'interruption (Ctrl+C)

Pour nettoyer manuellement:
```bash
rm -f /tmp/irc_test_* /tmp/irc_output_*
```

---

## 🎯 Checklist avant la correction

- [ ] Tous les tests automatisés passent
- [ ] Tester avec un vrai client IRC (irssi, WeeChat, HexChat)
- [ ] Vérifier les leaks mémoire: `valgrind ./ircserv 6667 pass`
- [ ] Tester avec plusieurs clients simultanés
- [ ] Vérifier la déconnexion propre (Ctrl+C)
- [ ] Tester tous les modes combinés
- [ ] Vérifier le comportement du Bot

---

## 🤝 Contribution

Pour ajouter de nouveaux tests ou améliorer les existants:

1. Créer une nouvelle fonction de test
2. L'ajouter dans `main()`
3. Documenter dans ce README
4. Tester que tous les tests passent toujours

---

## 📞 Support

En cas de problème:
1. Vérifier que le serveur est bien lancé
2. Vérifier les permissions des scripts (`chmod +x`)
3. Consulter les logs du serveur
4. Examiner les fichiers temporaires dans `/tmp/`

---

**Bon courage pour les tests ! 🚀**
