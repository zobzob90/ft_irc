1. **Lancer irssi :**
   ```bash
   irssi
   ```

2. **Se connecter au serveur :**
   ```
   /connect 127.0.0.1 <port> <password>
   ```
   - Remplace `<port>` par le port utilisé lors du lancement du serveur.
   - Remplace `<password>` par le mot de passe du serveur.

## Tableau récapitulatif des commandes

### Commandes d'authentification
|===============================================================================================================|
| Commande | Syntaxe | Description 				| ==============================================================|
|----------|---------|--------------------------| -----------------------------------------------------------	|
| `PASS` | `/PASS <password>` 	   				| S'authentifier auprès du serveur (obligatoire en premier) 	|
| `NICK` | `/NICK <pseudo>` 	   				| Définir ou changer son pseudo 								|
| `USER` | `/USER <username> 0 * :<realname>` 	| S'identifier (obligatoire après PASS/NICK) 					|
| `QUIT` | `/QUIT [message]` 	   				| Se déconnecter du serveur 									|
| ----------------------------------------------| ==============================================================|

### Commandes de channel

|===============================================================================================================|
| Commande 	| Syntaxe | Description 						| ==================================================|
|----------	|---------|-------------------------------------| --------------------------------------------------|
| `JOIN` 	| `/JOIN #<channel> [password]` 				| Rejoindre ou créer un salon 						|
| `PART` 	| `/PART #<channel> [message]` 					| Quitter un salon 									|
| `PRIVMSG` | `/PRIVMSG <#channel ou user> :<message>` 		| Envoyer un message à un salon ou un utilisateur	|
| `KICK` 	| `/KICK #<channel> <user> [raison]` 			| Expulser un utilisateur (opérateur uniquement) 	|
| `INVITE` 	| `/INVITE <user> #<channel>` 					| Inviter un utilisateur dans un salon 				|
| `TOPIC` 	| `/TOPIC #<channel> [nouveau topic]` 			| Afficher ou modifier le topic du salon 			|
| `MODE` 	| `/MODE #<channel> <+/-mode> [params]` 		| Gérer les modes d'un salon (opérateur uniquement) |
| ----------------------------------------------------------| ==================================================|

### Commandes d'information

|===================================================================================|
| Commande 	| Syntaxe | Description 	| ==========================================|
|----------	|---------|-----------------| ------------------------------------------|
| `WHO` 	| `/WHO` 					| Lister tous les utilisateurs connectés 	|
| `LIST` 	| `/LIST` 					| Lister tous les channels disponibles 		|
| --------------------------------------| ==========================================|


## Modes de channel disponibles

|===========================================================================================|
| Mode | Syntaxe | Description 			| ==================================================|
|------|---------|----------------------| --------------------------------------------------|
| `+i` | `/MODE #channel +i` 			| Channel invite-only (sur invitation uniquement) 	|
| `-i` | `/MODE #channel -i` 			| Désactiver le mode invite-only 					|
| `+t` | `/MODE #channel +t` 			| Seuls les opérateurs peuvent changer le topic 	|
| `-t` | `/MODE #channel -t` 			| Tout le monde peut changer le topic 				|
| `+k` | `/MODE #channel +k <password>` | Protéger le channel par un mot de passe 			|
| `-k` | `/MODE #channel -k` 			| Retirer le mot de passe du channel 				|
| `+o` | `/MODE #channel +o <user>` 	| Donner le statut opérateur à un utilisateur 		|
| `-o` | `/MODE #channel -o <user>` 	| Retirer le statut opérateur 						|
| `+l` | `/MODE #channel +l <limit>` 	| Limiter le nombre d'utilisateurs 					|
| `-l` | `/MODE #channel -l` 			| Retirer la limite d'utilisateurs 					|
|---------------------------------------| ==================================================|
---

## Commandes du Bot (BotCop 👮)
|==========|========================================|
| Commande | Description 				| ==========|
|----------|----------------------------| ----------|
| `!ping`  | Le bot répond "pong 🏓" 	|			|
| `!dadjoke` | Le bot raconte une blague			|
| `!rules` | Le bot affiche les règles du channel 	|
|---------------------------------------| ==========|