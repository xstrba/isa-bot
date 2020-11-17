/**
 * DiscordBot class definition
 * 
 * @file DiscordBot.hpp
 * @author Boris Strbak (xstrba05)
 * @date 2020-11-18
 */

#ifndef DISCORD_BOT_HPP
#define DISCORD_BOT_HPP

#include <unistd.h>
#include <string>
#include <time.h>
#include "DiscordSocket.hpp"

/** Type of text channel */
#define CH_GUILD_TEXT "0"

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
    DBOT_ERR_SOCKET = 13,
    DBOT_ERR_INTERNAL = 99,
    DBOT_ERR_AUTHORIZATION = 401,
    DBOT_ERR_FORBIDDEN = 403,
    DBOT_ERR_NOT_FOUND = 404,
    DBOT_ERR_BAD_METHOD = 405,
    DBOT_ERR_RATE_LIMITED = 429,
    DBOT_ERR_SERVER_INTERNAL = 500,
};

/**
 * Function that stops execution of a program for given miliseconds
 * Implementation inspired by http://timmurphy.org/2009/09/29/nanosleep-in-c-c/
 * 
 * @param unsigned milisec miliseconds
 * @return void
 */
void nsleep(unsigned milisec);

/**
 * DiscordBot class
 * 
 * Object with interface for communicating with discord server through their api
 * with the help of DiscordSocket object. Providing functions like getting isa-bot channel id,
 * loading new messages and sending responses to new messages.
 * 
 * @param DiscordSocket *socket socket for sending requests and receiving responses
 * @param std::string token bot token
 * @param std::string channelId id of isa-bot channel on server bot is added to
 * @param std::string guildId id of server bot is added to
 * @param std::string lastMsgId id of last message in channel
 * @param bool verbose true if bot should print messages to console
 * @param DBotErrors errorCode error code set in case some error happens
 * @param std::vector<JsonValue *> messages loaded new messages
 * @oaram std::lastTimestamp timestamp of last message loaded
 */
class DiscordBot
{
    public:
        /**
         * DiscordBot constructor
         * 
         * @param DiscordSocket socket
         * @param std::string token to authorize bote
         * @param bool verbose
         */
        DiscordBot(DiscordSocket *socket, std::string token, bool verbose = false);

        /**
         * DiscordBot destructor
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
         * Loads messages after lastMsgId. Clears previously loaded messages
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

        /**
         * Clear all loaded messages
         * 
         * @return void
         */
        void clearMessages();

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
        std::string lastTimestamp;
};

#endif