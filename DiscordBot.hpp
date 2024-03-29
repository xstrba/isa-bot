#ifndef DISCORD_BOT_HPP
#define DISCORD_BOT_HPP

#include <unistd.h>
#include <string>
#include "DiscordSocket.hpp"
#include "Message.hpp"

/**
 * @enum DBotErrors
 * 
 * Errors 1 and 99 are internal errors of application
 * Errors above 300 are errors from discord server
 */
enum DBotErrors {
    DBOT_NO_ERROR = 0,
    DBOT_ERR_NO_GUILD = 11,
    DBOT_ERR_NO_CHANNEL = 12,
    DBOT_ERR_INTERNAL = 99,
    DBOT_ERR_AUTHORIZATION = 401,
    DBOT_ERR_FORBIDDEN = 403,
    DBOT_ERR_NOT_FOUND = 404,
    DBOT_ERR_BAD_METHOD = 405,
    DBOT_ERR_SERVER_INTERNAL = 500,
};

class DiscordBot
{
    public:
        /**
         * Discord bot constructor
         * 
         * @param DiscordSocket socket
         * @param std::string token to authorize bote
         * @param bool verbose
         */
        DiscordBot(DiscordSocket *socket, std::string token, bool verbose = false);

        /**
         * Discord bot destructor
         */
        ~DiscordBot();

        /**
         * Returns bots socket
         * 
         * @return DiscordSocket
         */
        DiscordSocket *getSocket();

        /**
         * Sends requests through socket to get channel and set id
         * 
         * @return bool
         */
        bool setChannelId();

        /**
         * Get bots default headers
         * 
         * @return std::string
         */
        std::string getDefaultHeaders();

        /**
         * Return if bot has channel id
         * 
         * @return bool
         */
        bool isReadyToWork();

        /**
         * Get bot's error code
         * 
         * @return DBotErrors
         */
        DBotErrors getErrorCode();

        /**
         * Loads messages after lastMsgId
         * 
         * @return bool TRUE if message are successfully loaded
         */
        bool loadNewMessages();

        /**
         * Reacts to loaded messages
         * 
         * @return void
         */
        void reactToMessages();

    protected:
    private:
        DiscordSocket *socket;
        std::string token = "";
        std::string channelId = "";
        std::string guildId = "";
        std::string lastMsgId = "";
        bool verbose;
        DBotErrors errorCode;
        std::vector<JsonValue *> messages = {};
};

#endif