# Tests ft_irc

Ce dossier contient tous les tests pour le serveur IRC.

## Scripts de test disponibles

### Tests automatisés

- **`run_tests.sh`** - Lance le serveur et exécute les tests edge cases automatiquement
- **`test_edge_cases.py`** - Suite complète de tests pour les cas limites
- **`test_multi_clients.py`** - Tests avec plusieurs clients simultanés
- **`test_new_fixes.py`** - Tests pour les nouveaux correctifs
- **`test_debug.py`** - Tests de débogage

### Tests Valgrind

- **`test_valgrind.sh`** - Lance le serveur avec Valgrind (test manuel)
- **`test_valgrind_auto.sh`** - Lance Valgrind avec tests automatiques

### Tests manuels

- **`manual_test.sh`** - Guide pour tester manuellement avec netcat
- **`quick_test.py`** - Test rapide de connexion au serveur

### Vérification

- **`verify_fixes.sh`** - Vérifie que tous les correctifs sont appliqués

## Comment lancer les tests

### Depuis le dossier test

```bash
cd test
./run_tests.sh                # Tests complets
./quick_test.py               # Test rapide
./verify_fixes.sh             # Vérification des correctifs
./test_valgrind_auto.sh       # Test avec Valgrind
```

### Depuis le dossier racine

Les scripts peuvent aussi être lancés depuis le dossier racine :

```bash
./test/run_tests.sh
./test/verify_fixes.sh
# etc.
```

## Prérequis

- Python 3
- make
- valgrind (pour les tests mémoire)
- netcat (pour les tests manuels)

## Fichiers de documentation

- **`TEST_README.md`** - Documentation complète des tests
- **`TESTS_GUIDE.txt`** - Guide des tests
- **`valgrind-output.txt`** - Sortie des tests Valgrind (généré)

## Notes

- Le serveur doit être compilé dans le dossier parent (`../ircserv`)
- Les tests utilisent le port 6667 par défaut avec le mot de passe "testpass"
- Tous les scripts sont exécutables et peuvent être lancés directement
