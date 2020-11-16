# isa-bot

Program isa-bot, po spuštění který bude působit jako bot na komunikační službě Discord. Bot se připojí na Discord server na kanál "#isa-bot" a bude reagovat na všechny zprávy zaslané ostatními uživateli. Bot bude fungovat jako echo pro všechny zprávy, které zachytí. V případě, že bot na daném kanále zachytí jakoukoli zprávu jiného uživatele (tedy jinou než svou vlastní) a zároveň, která není jiného bota (uživatelské jméno neobsahuje podřetězec "bot"), odešle tuto zprávu zpátky na kanál a to ve formátu "echo: <username> - <message>" (kde <username> představuje uživatelské jméno uživatele, který odeslal původní zprávu).

Pokaždé když server vrátí odpověď 429(TOO MANY REQUESTS), bot čeká 1 sekundu před odesláním dalšího requestu. Defaultne bot čeká 1 sekundu před spracovaním nových správ a 1/5 sekundy mezi odesílaním requestů na vytvoření nových správ.

## Příklady spuštění

### Pro zobrazení nápovědy:
isabot -h
isabot --help

### Normální spuštění:
isabot -t 123bot_access_token123

### Normální spuštění se zobrazovaním správ v konzoli:
isabot -t 123bot_access_token123 -v
isabot -t 123bot_access_token123 --verbose

## Seznam odevzdaných souborů:
README.md
Makefile
manual.pdf
isabot.cpp
DiscordBot.hpp
DiscordBot.cpp
DiscordSocket.hpp
DiscordSocket.cpp
HttpResponse.hpp
HttpResponse.cpp
JsonParser.hpp
JsonParser.cpp
JsonValue.cpp
JsonValue.hpp
tests.cpp