/**
 * Implementation of DiscordBot class
 * 
 * @file DiscordBot.cpp
 * @author Boris Strbak (xstrba05)
 * @date 2020-11-18
 */

#include "DiscordBot.hpp"

DiscordBot::DiscordBot(DiscordSocket *socket, std::string token, bool verbose)
{
    this->socket = socket;
    this->token = token;
    this->verbose = verbose;
    lastTimestamp = "";
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
    errorCode = DBOT_NO_ERROR;

    HttpResponse *response;
    JsonValue *responseData;

    // send request for getting bot's guilds (servers)
    socket->sendData(
        "GET /api/users/@me/guilds HTTP/1.1",
        "",
        getDefaultHeaders());

    if (socket->getError())
    {
        errorCode = DBOT_ERR_SOCKET;
        return false;
    }

    response = socket->rcvData();
    responseData = response->getResponseData();
    int responseCode = response->getCode();

    if (socket->getError())
    {
        errorCode = DBOT_ERR_SOCKET;
        return false;
    }

    switch (responseCode)
    {
    case 200:
        // successful set guild id of the first guild in received array of guilds

        if (!responseData->getArray().size())
        {
            errorCode = DBOT_ERR_NO_GUILD;
            return false;
        }

        try
        {
            guildId = responseData->getArray()[0]->getObjectParam("id")->getString();
        }
        catch (const std::exception &e)
        {
            errorCode = DBOT_ERR_NO_GUILD;
            return false;
        }
        break;
    case 401:
        errorCode = DBOT_ERR_AUTHORIZATION;
        return false;
    case 403:
        errorCode = DBOT_ERR_FORBIDDEN;
        return false;
    case 500:
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

    if (socket->getError())
    {
        errorCode = DBOT_ERR_SOCKET;
        return false;
    }

    switch (responseCode)
    {
    case 200:
        channels = responseData->getArray();

        for (std::vector<JsonValue *>::iterator it = channels.begin(); it != channels.end(); ++it)
        {
            // find channel that's name is isa-bot and load it's id and last message id
            if (
                (*it)->getObjectParam("name")->getString() == "isa-bot" &&
                (*it)->getObjectParam("type")->getString() == CH_GUILD_TEXT
            )
            {
                try
                {
                    channelId = (*it)->getObjectParam("id")->getString();
                    lastMsgId = (*it)->getObjectParam("last_message_id")->getString();
                    return true;
                }
                catch (const std::exception &e)
                {
                }
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

    clearMessages();

    if (channelId.length())
    {
        // prepare requst
        // if has last message id get messages only after this message
        std::stringstream req;
        req << "GET /api/channels/" << channelId << "/messages";
        if (lastMsgId.length())
        {
            req << "?after=" << lastMsgId;
        }
        req << " HTTP/1.1";

        socket->sendData(
            req.str(),
            "",
            getDefaultHeaders());

        if (socket->getError())
        {
            errorCode = DBOT_ERR_SOCKET;
            return false;
        }

        HttpResponse *response = socket->rcvData();
        int responseCode = response->getCode();
        JsonValue *data;
        data = response->getResponseData();

        try
        {
            if (std::stoi(response->getHeader("x-ratelimit-remaining")) == 0 && responseCode != 429)
            {
                unsigned waitSeconds = std::stoi(response->getHeader("x-ratelimit-reset-after"));
                sleep(waitSeconds);
            }
        }
        catch (const std::exception &e)
        {
        }

        if (socket->getError())
        {
            errorCode = DBOT_ERR_SOCKET;
            return false;
        }

        switch (responseCode)
        {
        case 200:
            try
            {

                // was successful so store all loaded messages
                if (data->getType() == JDT_ARRAY && data->getArray().size())
                {
                    std::vector<JsonValue *> dataMessages = data->getArray();

                    // iterat over messages from the end
                    for (auto it = dataMessages.end(); it-- != dataMessages.begin();)
                    {
                        // only if message json value is object
                        if ((*it)->getType() == JDT_OBJECT)
                        {
                            // get message id
                            std::string msgId = (*it)->getObjectParam("id")->getString();

                            if (msgId.length())
                            {
                                try
                                {
                                    lastMsgId = msgId;
                                    JsonValue *user = (*it)->getObjectParam("author");

                                    if (
                                        user->getType() == JDT_OBJECT &&
                                        user->getObjectParam("username")->getString().find("bot") == ULONG_MAX &&
                                        user->getObjectParam("bot")->getString().find("true") == ULONG_MAX)
                                    {
                                        // // store message only if user doesn't have bot in name or doesn't
                                        // // have bot parameter set to true
                                        messages.push_back(*it);
                                    }
                                }
                                catch (const std::exception &e)
                                {
                                }
                            }
                        }
                    }
                }
            }
            catch (const std::exception &e)
            {
                errorCode = DBOT_ERR_INTERNAL;
                return false;
            }

            break;
        case 401:
            errorCode = DBOT_ERR_AUTHORIZATION;
            return false;
        case 403:
            errorCode = DBOT_ERR_FORBIDDEN;
            return false;
        case 500:
        case 503:
            errorCode = DBOT_ERR_SERVER_INTERNAL;
            return false;
        case 429:
            errorCode = DBOT_ERR_RATE_LIMITED;
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
    errorCode = DBOT_NO_ERROR;

    for (auto it = messages.begin(); it != messages.end(); it++)
    {
        // wait 1/5 of a second before sending new messages
        usleep(200000);
        JsonValue *user = (*it)->getObjectParam("author");
        if (user->getType() == JDT_OBJECT)
        {

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
                   << (*it)->getObjectParam("tts")->getString()
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

            if (socket->getError())
            {
                errorCode = DBOT_ERR_SOCKET;
                return;
            }

            HttpResponse *response = socket->rcvData();
            int responseCode = response->getCode();

            try
            {
                if (std::stoi(response->getHeader("x-ratelimit-remaining")) == 0 && responseCode != 429)
                {
                    unsigned waitSeconds = std::stoi(response->getHeader("x-ratelimit-reset-after"));
                    sleep(waitSeconds);
                }
            }
            catch (const std::exception &e)
            {
            }

            if (socket->getError())
            {
                errorCode = DBOT_ERR_SOCKET;
                return;
            }

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
                case 503:
                    errorCode = DBOT_ERR_SERVER_INTERNAL;
                    break;
                case 429:
                    // if bot is rate limited wait for 5 seconds
                    it--;
                    sleep(5);
                    continue;
                default:
                    std::cerr << "what";
                    errorCode = DBOT_ERR_INTERNAL;
                    break;
                }
            }
            else
            {
                lastMsgId = (*it)->getObjectParam("id")->getString();
            }

            delete response;
        }
    }
}

void DiscordBot::clearMessages()
{
    // delete all previously loaded messages
    for (auto it = messages.begin(); it != messages.end(); it++)
    {
        delete (*it);
    }

    messages = {};
}

DiscordSocket *DiscordBot::getSocket()
{
    return socket;
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