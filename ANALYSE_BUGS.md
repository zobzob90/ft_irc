# 🔍 RAPPORT D'ANALYSE COMPLÈTE - ft_irc

## 🐛 PROBLÈMES IDENTIFIÉS

### 1. ❌ **CRITIQUE - Validation des noms de channels insuffisante**

**Fichier**: `srcs/client/Command_function.cpp:90-143`

**Problème actuel**:
```cpp
if (channelName[0] != '#')
{
    sendError(403, channelName + " :No such channel");
    return ;
}
// Aucune autre validation !
```

**Conséquences**:
- Accepte `#` seul → Channel invalide créé
- Accepte `#chan nel` avec espaces → Problème de parsing
- Accepte `#ch@nnel` avec @ → Caractères interdits IRC
- Accepte noms de 300+ caractères → Buffer overflow potentiel
- Accepte `##doublehash` → Non conforme IRC

**Impact**: ⭐⭐⭐⭐⭐ CRITIQUE - Crash du serveur

---

### 2. ❌ **GRAVE - Bot détruit le channel avec le client dedans**

**Fichier**: `srcs/bot/Bot.cpp:59-67`

**Problème actuel**:
```cpp
void Bot::kickUser(Channel* channel, Client* user, const std::string& reason)
{
    channel->broadcast(msg, NULL);
    channel->removeMember(user);
    if (channel->getMembersCount() == 0)
        _serv->destroyChannel(channel->getName());  // ⚠️ DANGER
    // Ne pas déconnecter l'utilisateur, juste le retirer du channel
}
```

**Problème**: Le bot détruit le channel avec `delete` pendant qu'il est encore utilisé !

**Impact**: ⭐⭐⭐⭐⭐ CRITIQUE - Segmentation fault

---

### 3. ⚠️ **Pas de limite sur le nombre de channels**

**Fichier**: `srcs/channel/Channel_Management.cpp:23-34`

**Problème**: Aucune limite, un client peut créer 1000+ channels

**Impact**: ⭐⭐⭐ MOYEN - DoS possible

---

### 4. ⚠️ **Pas de validation sur la longueur des nicknames**

**Fichier**: `srcs/client/Command_function.cpp:33-56`

**Problème**: Accepte des nicknames de 100+ caractères

**Impact**: ⭐⭐ FAIBLE - Non conforme RFC

---

## ✅ SOLUTIONS

### Solution 1: Validation stricte des noms de channels

**Action**: Créer une fonction de validation

**Fichier à modifier**: `srcs/channel/Channel_Management.cpp`

```cpp
// Ajouter cette fonction AVANT createChannel
static bool isValidChannelName(const std::string& name)
{
    // Doit commencer par #
    if (name.empty() || name[0] != '#')
        return false;
    
    // Minimum 2 caractères (#a), maximum 50
    if (name.length() < 2 || name.length() > 50)
        return false;
    
    // Ne peut pas être juste '#'
    if (name == "#")
        return false;
    
    // Vérifier les caractères invalides (selon RFC 2812)
    for (size_t i = 1; i < name.length(); i++)
    {
        char c = name[i];
        // Interdire: espace, virgule, control chars
        if (c == ' ' || c == ',' || c == 7 || c == '\r' || c == '\n' || c < 32)
            return false;
        
        // Interdire caractères spéciaux problématiques
        if (c == '@' || c == '!' || c == ':')
            return false;
    }
    
    return true;
}
```

---

### Solution 2: Corriger le Bot pour éviter use-after-free

**Fichier à modifier**: `srcs/bot/Bot.cpp`

```cpp
void Bot::kickUser(Channel* channel, Client* user, const std::string& reason)
{
    std::string channelName = channel->getName(); // Sauvegarder le nom
    
    std::string msg = ":" + _botClient->getPrefix() + " KICK " + channelName + " " 
                      + user->getNickname() + " :" + reason;
    channel->broadcast(msg, NULL);
    
    std::cout << "🚨 " << _name << " kicked " << user->getNickname() 
              << " from " << channelName << ": " << reason << std::endl;
    
    channel->removeMember(user);
    
    // IMPORTANT: Vérifier APRÈS avoir fini d'utiliser le pointeur
    if (channel->getMembersCount() == 0)
    {
        // Le channel sera détruit, ne plus utiliser le pointeur après !
        _serv->destroyChannel(channelName);
    }
}
```

---

### Solution 3: Limiter le nombre de channels

**Fichier à modifier**: `inc/Server.hpp`

```cpp
// Dans la classe Server, ajouter:
#define MAX_CHANNELS 100
```

**Fichier à modifier**: `srcs/channel/Channel_Management.cpp`

```cpp
Channel* Server::createChannel(const std::string& name, Client* creator)
{
    // Vérifier la limite
    if (_channels.size() >= MAX_CHANNELS)
    {
        std::string errorMsg = ":server 405 " + creator->getNickname() 
                              + " " + name + " :You have joined too many channels\r\n";
        send(creator->getFd(), errorMsg.c_str(), errorMsg.length(), 0);
        return NULL;
    }
    
    // Le reste du code...
}
```

---

### Solution 4: Valider la longueur des nicknames

**Fichier à modifier**: `srcs/client/Command_function.cpp`

```cpp
void Command::executeNick()
{
    if (!checkParamSize(1, "NICK"))
        return ;
    
    std::string newNickname = _params[0];
    
    if (newNickname.empty())
    {
        sendError(431, ":No Nickname Given");
        return ;
    }
    
    // Nouveau: Vérifier la longueur (RFC 2812: max 9 chars, nous: max 30)
    if (newNickname.length() > 30)
    {
        sendError(432, newNickname + " :Erroneous nickname (too long)");
        return;
    }
    
    // Nouveau: Vérifier caractères invalides
    for (size_t i = 0; i < newNickname.length(); i++)
    {
        char c = newNickname[i];
        // RFC 2812: letters, digits, special: [ ] \ ` _ ^ { | }
        bool isLetter = (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
        bool isDigit = (c >= '0' && c <= '9');
        bool isSpecial = (c == '[' || c == ']' || c == '\\' || c == '`' || 
                          c == '_' || c == '^' || c == '{' || c == '|' || c == '}' ||
                          c == '-');
        
        if (!isLetter && !isDigit && !isSpecial)
        {
            sendError(432, newNickname + " :Erroneous nickname (invalid characters)");
            return;
        }
    }
    
    // Le reste du code existant...
}
```

---

## 📊 PRIORISATION DES FIXES

### 🔴 URGENT (Faire en PREMIER)
1. ✅ **Fix Bot.cpp** - Éviter le use-after-free (5 min)
2. ✅ **Validation channels** - Ajouter isValidChannelName() (10 min)

### 🟡 IMPORTANT (Faire ENSUITE)
3. ✅ **Limite channels** - MAX_CHANNELS (5 min)
4. ✅ **Validation nicknames** - Longueur et caractères (15 min)

### 🟢 OPTIONNEL
5. Ajouter des logs détaillés
6. Améliorer les messages d'erreur

---

## 🎯 RÉSULTATS ATTENDUS APRÈS FIX

- ✅ Tests 1-8: Déjà OK
- ✅ Tests 9-26: Devraient tous passer
- ✅ Pas de crash
- ✅ Pas de segfault
- ✅ Conforme RFC IRC

**Score estimé**: 24-26 / 26 tests (92-100%)

---

## 📝 ORDRE D'APPLICATION DES FIXES

1. Modifier `Bot.cpp` (kickUser)
2. Modifier `Channel_Management.cpp` (isValidChannelName + limite)
3. Modifier `Command_function.cpp` (executeJoin - appeler validation)
4. Modifier `Command_function.cpp` (executeNick - validation)
5. Recompiler et tester

---

## 🧪 COMMANDES DE TEST APRÈS FIX

```bash
# Recompiler
make re

# Test rapide
./run_tests.sh

# Test avec Valgrind
./test_valgrind.sh

# Test multi-clients
python3 test_multi_clients.py
```
