# 🌐 ft_irc - Serveur IRC

Un serveur IRC (Internet Relay Chat) fonctionnelle implémenté en C++98 conforme au spécification RFC 1459/2812 prenant en référence le client IRSSI.
Ce projet a pour objectif de comprendre les **réseaux**, les **sockets**, la **gestion de multiples clients**, ainsi que l’architecture d’un serveur temps réel.

## ✨ Fonctionnalités
### Fonctionnalités principales
- ✅ **Support multi-clients** via multiplexage `poll()`
- ✅ **Opérations I/O non-bloquantes**
- ✅ **Conformité RFC 1459/2812** du protocole
- ✅ **Système d'authentification** (protection par mot de passe)
- ✅ **Gestion de channels** (créer, rejoindre, quitter, kick, inviter)
- ✅ **Messages privés** entre utilisateurs
- ✅ **Privilèges opérateur** et modes de channel
- ✅ **Gestion des signaux** (SIGINT, SIGQUIT)

## 🧠 Qu'est ce qu'un socket ?

Un **socket** est un point de communication permettant à deux programmes d’échanger des données via un réseau.

## 🧠 Qu'est ce que TCP ?

C'est un protocole de transmission/transport fiable utilisé pour assurer :
  - l'intégrité des données;
  - l'ordre des messages;
  - une connexion persistante entre le client et le serveur.
Le protocole TCP permet de garantir que les messages qui arrivent sur le serveur arrivent de la bonne ordre et complets.

## Comment le serveur fonctionne ?
- Le serveur crée un socket et écoute sur un port choisi par l'utilisateur.
- Les clients IRC se connectent a ce socket.
- Les échanges se font via le protocole TCP (Transmission Control Protocol).
- - Le serveur utilise `poll()` pour surveiller :
  - le socket serveur (nouvelles connexions)
  - les sockets clients (messages entrants)
- Lorsqu’un événement est détecté :
  - le message est lu
  - parsé selon le protocole IRC
  - puis exécuté

## Fonction principale :

- socket()  Création du socket serveur
- bind()    Réservation du port
- listen()  Prépare le serveur a recevoir des clients
- accept()  Crée un nouveaux fd client
- recv()    Recoit les donnees d'un socket - Lit les commandes des clients
- send()    Envoie les donnees d'un socket - Envoie les réponses aux clients
- Poll()    Surveille plusieurs FD pour détecter des évenement i/o
- fnctl()   Active le mode non bloquant 

## 🧱 Architecture
```
├── inc
│   ├── Bot.hpp
│   ├── Channel.hpp
│   ├── Client.hpp
│   ├── Command.hpp
│   └── Server.hpp
├── Makefile
├── srcs
│   ├── bot
│   │   └── Bot.cpp
│   ├── channel
│   │   ├── Channel.cpp
│   │   └── Channel_Management.cpp
│   ├── client
│   │   ├── Client.cpp
│   │   ├── Command_auth.cpp
│   │   ├── Command_channel.cpp
│   │   ├── Command_info.cpp
│   │   ├── Command_parsing.cpp
│   │   └── Command_utils.cpp
│   ├── main.cpp
│   └── reseaux
│       ├── Server_Management.cpp
│       ├── Server_Utils.cpp
│       └── Socket.cpp
```

## 👥 Contributeurs
@ertrigna
@vdeliere

# 📄 Licence
Ce projet fait partie du cursus de l'école 42 et suit les politiques d'intégrité académique de l'école.
