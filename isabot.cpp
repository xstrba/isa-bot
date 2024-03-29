/**
 * @file isabot.c
 * @author Boris Strbak (xstrba05) 
 */

#include <string>
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <openssl/ssl.h>
#include "string.h"
#include "DiscordSocket.hpp"
#include "DiscordBot.hpp"

#define PORT 443

using namespace std;

/**
 * Display help message
 * 
 * @return 0
 */
int displayHelp();

/**
 * Process arguments and set verbose and token
 * 
 * @param int argc number of commant line arguments
 * @param char[][] argv command line arguments
 * @param bool verbose variable to store if verbose is on
 * @param bool help variable to store if help is on
 * @param string token variable to store token
 * @return void
 */
void processArgs(int argc, char **argv, bool *verbose, bool *isHelp, string *token);

/**
 * Main function
 * 
 * @param int argc number of commant line arguments
 * @param char[][] argv command line arguments
 * @return 0 if success or help message, 1 if error 
 */
int main(int argc, char **argv)
{
    bool verbose = false;
    bool help = false;
    string token = "";

    processArgs(argc, argv, &verbose, &help, &token);

    if (help || !token.length())
    {
        return displayHelp();
    }

    DiscordSocket *socket = new DiscordSocket();
    DiscordBot *bot;

    // try to initialize socket
    socket->initialize();
    if (socket->getError())
    {
        cerr << socket->getErrorMessage() << endl;
        delete socket;
        return 1;
    }

    cout << "Connected to server[name address]: "
         << socket->getHostName()
         << " "
         << socket->getIpAddress()
         << endl;

    bot = new DiscordBot(socket, token, verbose);

    if (bot->setChannelId())
    {
        while (1)
        {
            sleep(1);

            if (!bot->loadNewMessages())
            {
                if (verbose)
                {
                    cerr << "Error: Chyba při načítaní správ" << endl;
                }
            } else {
                bot->reactToMessages();
            }

            if (socket->getError() || bot->getErrorCode() != DBOT_NO_ERROR)
            {
                delete bot;
                return 1;
            }
        }
    }

    int err = bot->getErrorCode();

    if (err == DBOT_NO_ERROR)
    {
        delete bot;
        return 0;
    }

    std::string errMessage = "";
    switch (err)
    {
    case DBOT_ERR_AUTHORIZATION:
        errMessage = "Chyba autorizace. Zkontrolujte token";
        break;
    case DBOT_ERR_NO_GUILD:
        errMessage = "Bot není připojenej na žádnej server";
        break;
    case DBOT_ERR_NO_CHANNEL:
        errMessage = "Nebyl nalezen kanál isa-bot";
        break;
    case DBOT_ERR_INTERNAL:
        errMessage = "Došlo ku chybe pri spracovaní odpovědí discord serveru";
        break;
    case DBOT_ERR_SERVER_INTERNAL:
        errMessage = "Došlo ku chybe při komunikaci s discord serverem";
        break;
    case DBOT_ERR_FORBIDDEN:
        errMessage = "Bot nemá dostatečné práva. Povolte práva \"View Channels\", \"Send Messages\", \"Read Message History\" a \"Embed Links\" z rozsahu \"Bot\"";
        break;
    default:
        errMessage = "Neznáma chyba";
        break;
    }

    delete bot;
    cerr << "Error: " + errMessage << endl;
    return err ? err : 1;
}

int displayHelp()
{
    cout << "Spuštění programu" << endl;
    cout << "Použití: isabot [-h|--help] [-v|--verbose] -t <bot_access_token>" << endl
         << endl;
    cout << "Pořadí parametrů je libovolné. Popis parametrů:" << endl;
    cout << "\t- Spuštění programu bez parametrů zobrazí nápovědu." << endl;
    cout << "\t-h|--help : Vypíše nápovědu na standardní výstup." << endl;
    cout << "\t-v|--verbose : Bude zobrazovat zprávy, na které bot reaguje na standardní výstup ve formátu \"<channel> - <username>: <message>\"." << endl;
    cout << "\t-t <bot_access_token> : Zde je nutno zadat autentizační token pro přístup bota na Discord." << endl
         << endl;
    cout << "Ukončení programu proběhne zasláním signálu SIGINT (tedy například pomocí kombinace kláves Ctrl + c), do té doby bude bot vykonávat svou funkcionalitu." << endl;
    return 0;
}

void processArgs(int argc, char **argv, bool *verbose, bool *isHelp, string *token)
{
    // parse command line arguments
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)
        {
            (*isHelp) = true;
            return;
        }

        if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0)
        {
            (*verbose) = true;
        }

        if (strcmp(argv[i], "-t") == 0)
        {
            if (i + 1 >= argc || argv[i + 1][0] == '-')
            {
                (*isHelp) = true;
                return;
            }

            (*token) = argv[i + 1];
        }
    }
}