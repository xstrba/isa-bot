#ifndef DISCORD_SOCKET_HPP
#define DISCORD_SOCKET_HPP

#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h> 
#include <openssl/ssl.h>
#include <sstream> 
#include <string>
#include <netinet/in.h>
#include <sys/socket.h>
#include "HttpResponse.hpp"
#include "string.h"

#define HTTPS_PORT 443
#define HTTP_PORT 80
#define RCV_BUFFER_LENGTH 2048

/**
 * DiscordSocket class
 * 
 * Class connecting through socket to discord server and providing
 * interface for sending and receiving packets
 * 
 * @param int port port on which socket will connect (is set to HTTPS_PORT by default)
 * @param int sock identifier of socket for communicating with discord server
 * @param std::string hostName is set by default to discord.com
 * @param bool error signalizing that some error happend
 * @param std::string errorMessage containing error message
 * @param std::string ipAddress ip address of host (set if connection with socket was successfull)
 * @param SSL *sslConn ssl connection to server
 * @param SSL_CTX *sslCtx ssl context
 */
class DiscordSocket
{
    public:
        /**
         * DiscordSocket constructor
         */
        DiscordSocket();

        /**
         * DicordSocket destructor
         */
        ~DiscordSocket();

        /**
         * Get default port for connecting to server
         * 
         * @return int port
         */
        int getPort();

        /**
         * Get host name
         * 
         * @return std::string name of host
         */
        std::string getHostName();

        /**
         * Get error status
         * 
         * @return bool error status
         */
        bool getError();

        /**
         * Get current error message
         * 
         * @return std::string error message
         */
        std::string getErrorMessage();

        /**
         * Initialize socket connection
         * 
         * @return void
         */
        void initialize();

        /**
         * Get ip address of server socket is connected to
         * 
         * @return std::string ip address
         */
        std::string getIpAddress();

        /**
         * Get socket's ssl connection
         * 
         * @return SSL ssl connection
         */
        SSL *getSSLConnection();

        /**
         * Sends http request with header indicating closing the connection
         * 
         * @return void
         */
        void closeConnection();

        void sendData(std::string message, std::string data = "", std::string headers = "");

        HttpResponse *rcvData();

    protected:
    private:
        int port;
        int sock;
        std::string hostName;
        bool error;
        std::string errorMessage;
        std::string ipAddress;
        SSL *sslConn;
        SSL_CTX *sslCtx;

        /**
         * Set error state with given message
         * 
         * @return void
         */
        void setError(std::string message);
};

#endif