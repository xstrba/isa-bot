#ifndef DISCORD_SOCKET_HPP
#define DISCORD_SOCKET_HPP

#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h> 
#include <openssl/ssl.h>
#include <sstream> 
#include <string>
#include "HttpResponse.hpp"
#include "string.h"

#define HTTPS_PORT 443
#define HTTP_PORT 80
#define RCV_BUFFER_LENGTH 2048

class DiscordSocket
{
    public:
        /**
         * Discord socket constructor
         */
        DiscordSocket();

        /**
         * Dicord socket destructor
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
         * Get structure representing server address
         * 
         * @return sockaddr_in
         */
        struct sockaddr_in getServerAddressStruct();

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

        // /**
        //  * Send request and receive data
        //  * 
        //  * @param std::string message
        //  * @param std::string data
        //  * @param std::string headers
        //  * @return HttpResponse
        //  */
        // HttpResponse *sendAndRecv(std::string message, std::string data = "", std::string headers = "");

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
        struct sockaddr_in sAddrStruct;
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