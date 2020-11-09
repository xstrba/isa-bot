#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <string>

class Message
{
    public:
        /**
         * Message contructor
         * 
         * @param std::string channel
         * @param std::string author
         * @param std::string content
         */
        Message(std::string channel, std::string author, std::string content);

        /**
         * Get message channel
         * 
         * @return std::string
         */
        std::string getChannel();

        /**
         * Get message author
         * 
         * @return std::string
         */
        std::string getAuthor();

        /**
         * Get message content
         * 
         * @return std::string
         */
        std::string getContent();

        /**
         * Get message sent to channel in a format
         * echo: <username> - <message>
         * 
         * @return std::string
         */
        std::string getChannelMessage();

        /**
         * Get message sent to console in a format
         * <channel> - <username>: <message>
         * 
         * @return std::string
         */
        std::string getConsolelMessage();
    protected:
    private:
        std::string channel;
        std::string author;
        std::string content;
};

#endif