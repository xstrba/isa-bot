#include "DiscordBot.hpp"

DiscordBot::DiscordBot(DiscordSocket *socket, std::string token, bool verbose)
{
    this->socket = socket;
    this->token = token;
    this->verbose = verbose;
    errorCode = DBOT_NO_ERROR;
}

std::string DiscordBot::getDefaultHeaders()
{
    // default header is for bot authorization and for giving information
    // that we are bot
    return "Authorization: Bot " + token + "\r\nUser-Agent: DiscordBot (https://discordapp.com, 2.0)\r\n";
}

bool DiscordBot::setChannelId()
{
    HttpResponse *response;
    JsonValue *responseData;

    // send request for getting bot's guilds (servers)
    socket->sendData(
        "GET /api/users/@me/guilds HTTP/1.1",
        "",
        getDefaultHeaders());

    response = socket->rcvData();
    responseData = response->getResponseData();
    int responseCode = response->getCode();

    switch (responseCode)
    {
        case 200:
            // successful set guild id of the first guild in received array of guilds

            if (!responseData->getArray().size())
            {
                errorCode = DBOT_ERR_NO_GUILD;
                return false;
            }
            guildId = responseData->getArray()[0]->getObjectParam("id")->getString();
            break;
        case 401:
            errorCode = DBOT_ERR_AUTHORIZATION;
            return false;
        case 403:
            errorCode = DBOT_ERR_FORBIDDEN;
            return false;
        case 500:
            errorCode = DBOT_ERR_SERVER_INTERNAL;
            return false;
        case 503:
            errorCode = DBOT_ERR_SERVER_INTERNAL;
            return false;
        default:
            errorCode = DBOT_ERR_INTERNAL;
            return false;
            break;
    }

    delete response;

    // send request to get all channels in guild
    socket->sendData(
        std::string("GET /api/guilds/") + guildId + std::string("/channels HTTP/1.1"),
        "",
        getDefaultHeaders());

    response = socket->rcvData();
    responseData = response->getResponseData();
    responseCode = response->getCode();
    std::vector<JsonValue *> channels = {};
    
    switch (responseCode)
    {
        case 200:
            channels = responseData->getArray();

            for (std::vector<JsonValue *>::iterator it = channels.begin(); it != channels.end(); ++it)
            {
                // find channel that's name is isa-bot and load it's id and last message id
                if ((*it)->getObjectParam("name")->getString() == "isa-bot")
                {
                    channelId = (*it)->getObjectParam("id")->getString();
                    lastMsgId = (*it)->getObjectParam("last_message_id")->getString();
                    return true;
                }
            }

            errorCode = DBOT_ERR_NO_CHANNEL;
            return false;
        case 401:
            errorCode = DBOT_ERR_AUTHORIZATION;
            return false;
        case 403:
            errorCode = DBOT_ERR_FORBIDDEN;
            return false;
        case 500:
            errorCode = DBOT_ERR_SERVER_INTERNAL;
            return false;
        case 503:
            errorCode = DBOT_ERR_SERVER_INTERNAL;
            return false;
        default:
            errorCode = DBOT_ERR_INTERNAL;
            return false;
            break;
    }

    return false;
}

bool DiscordBot::isReadyToWork()
{
    // bot is ready only when has channel id loaded
    return channelId.length() > 0 ? true : false;
}

DBotErrors DiscordBot::getErrorCode()
{
    return errorCode;
}

bool DiscordBot::loadNewMessages()
{
    errorCode = DBOT_NO_ERROR;
    
    // delete all previously loaded messages
    for (auto it = messages.begin(); it != messages.end(); it++)
    {
        delete (*it);
    }

    messages = {};

    if (channelId.length())
    {
        // prepare requst
        // if has last message id get messages only after this message
        std::stringstream req;
        req << "GET /api/channels/" <<  channelId << "/messages";
        if (lastMsgId.length()) {
            req << "?after=" << lastMsgId;
        }
        req << " HTTP/1.1";

        socket->sendData(
            req.str(),
            "",
            getDefaultHeaders());
        
        HttpResponse *response = socket->rcvData();
        int responseCode = response->getCode();
        JsonValue *data;
        data = response->getResponseData();

        switch (responseCode)
        {
            case 200:
                // was successful so store all loaded messages
                if (data->getType() == JDT_ARRAY && data->getArray().size()) {
                    
                    std::vector<JsonValue *> dataMessages = data->getArray();
                    for (auto it = dataMessages.begin(); it != dataMessages.end(); it++) {
                        if ((*it)->getType() == JDT_OBJECT) {
                            // only if message json value is object

                            // get message id
                            std::string msgId = (*it)->getObjectParam("id")->getString();
                            
                            if (msgId.length()) {
                                lastMsgId = msgId;
                                JsonValue *user = (*it)->getObjectParam("author");

                                if (
                                    user->getType() == JDT_OBJECT &&
                                    user->getObjectParam("username")->getString().find("bot") == ULONG_MAX &&
                                    user->getObjectParam("bot")->getString().find("true") == ULONG_MAX
                                ) {
                                    // store message only if user doesn't have bot in name or doesn't
                                    // have bot parameter set to true

                                    messages.push_back(*it);
                                }
                            }
                        }
                    }
                }

                break;
            case 401:
                errorCode = DBOT_ERR_AUTHORIZATION;
                return false;
            case 403:
                errorCode = DBOT_ERR_FORBIDDEN;
                return false;
            case 500:
                errorCode = DBOT_ERR_SERVER_INTERNAL;
                return false;
            case 503:
                errorCode = DBOT_ERR_SERVER_INTERNAL;
                return false;
            default:
                errorCode = DBOT_ERR_INTERNAL;
                return false;
                break;
        }
    }

    return true;
}

void DiscordBot::reactToMessages()
{
    for (auto it = messages.begin(); it != messages.end(); it++)
    {
        JsonValue *user = (*it)->getObjectParam("author");
        if (user->getType() == JDT_OBJECT) {
            
            if (verbose)
            {
                // print message to console

                std::cout << "isa-bot - "
                    << user->getObjectParam("username")->getString().c_str()
                    << ": "
                    << (*it)->getObjectParam("content")->getString().c_str() << std::endl;
            }
            
            // set parameters of message to be sent back to server
            // content and tts
            // tts unchanged value
            std::stringstream params;
            params << "{\"content\": \"echo: "
                << user->getObjectParam("username")->getString()
                << " - "
                << (*it)->getObjectParam("content")->getString()
                << "\", \"tts\": "
                <<  (*it)->getObjectParam("tts")->getString()
                << "}";

            // headers Content-Length and Content-Type
            std::stringstream headers;
            headers << getDefaultHeaders()
                << "Content-Length: " << params.str().length()
                << "\r\nContent-Type: application/json\r\n";

            // send request to create message in bot's channel
            socket->sendData(
                std::string("POST /api/channels/") + channelId + std::string("/messages HTTP/1.1"),
                params.str(),
                headers.str());

            HttpResponse *response = socket->rcvData();
            int responseCode = response->getCode();

            if (responseCode != 200)
            {
                // just check if request was successful
                
                switch (responseCode)
                {
                    case 401:
                        errorCode = DBOT_ERR_AUTHORIZATION;
                        break;
                    case 403:
                        errorCode = DBOT_ERR_FORBIDDEN;
                        break;
                    case 500:
                        errorCode = DBOT_ERR_SERVER_INTERNAL;
                        break;
                    case 503:
                        errorCode = DBOT_ERR_SERVER_INTERNAL;
                        break;
                    default:
                        errorCode = DBOT_ERR_INTERNAL;
                        break;
                }
            }

            delete response;
        }
    }
}

DiscordBot::~DiscordBot()
{
    // delete socket and messages
    delete socket;

    for (auto it = messages.begin(); it != messages.end(); it++)
    {
        delete (*it);
    }
}