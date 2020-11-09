#include "Message.hpp"

Message::Message(std::string channel, std::string author, std::string content)
{
    this->channel = channel;
    this->author = author;
    this->content = content;
}

std::string Message::getChannel()
{
    return channel;
}

std::string Message::getAuthor()
{
    return author;
}

std::string Message::getContent()
{
    return content;
}

std::string Message::getChannelMessage()
{
    return "echo: " + author + " - " + content;
}

std::string Message::getConsolelMessage()
{
    return channel + " - " + author + ": " + content;
}