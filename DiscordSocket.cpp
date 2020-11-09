#include "DiscordSocket.hpp"

DiscordSocket::DiscordSocket()
{
    port = HTTPS_PORT;
    hostName = "discord.com";
    error = false;
    sock = 0;
}


int DiscordSocket::getPort()
{
    return port;
}

std::string DiscordSocket::getHostName()
{
    return hostName;
}

bool DiscordSocket::getError()
{
    return error;
}

std::string DiscordSocket::getErrorMessage()
{
    return errorMessage;
}

struct sockaddr_in DiscordSocket::getServerAddressStruct()
{
    return sAddrStruct;
}

std::string DiscordSocket::getIpAddress()
{
    return ipAddress;
}

void DiscordSocket::setError(std::string message)
{
    errorMessage = message;
    error = true;
}

SSL *DiscordSocket::getSSLConnection()
{
    return sslConn;
}

void DiscordSocket::initialize()
{
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        return setError("Chyba při vytváření soketu");
    }

    sAddrStruct.sin_family = AF_INET;
    sAddrStruct.sin_port = htons(port);

    struct hostent *host;
    host = gethostbyname(hostName.c_str());
    if (!host) {
        return setError("Nepodařilo se připojít na server " + hostName);
    }

    ipAddress = inet_ntoa(*((struct in_addr*) host->h_addr_list[0]));

    if(inet_pton(AF_INET, ipAddress.c_str(), &sAddrStruct.sin_addr) <= 0)  
    {
        return setError("Adresa není validní nebo není podporována");
    }

    if (connect(sock, (struct sockaddr *)&sAddrStruct, sizeof(sAddrStruct)) < 0) 
    {
        return setError("Nepodařilo se připojít na discord server pomocí soketu");
    }

    SSL_load_error_strings ();
    SSL_library_init ();
    sslCtx = SSL_CTX_new (SSLv23_client_method ());
    
    sslConn = SSL_new(sslCtx);
    SSL_set_fd(sslConn, sock);
    
    if (SSL_connect(sslConn) != 1) {
        return setError("Nepodařilo se vytvořit ssl spojení");
    }
}

void DiscordSocket::sendData(std::string message, std::string data, std::string headers)
{
    std::stringstream sentData;
    sentData << message << "\r\n"
        << "Host: " << hostName << "\r\n"
        << headers
        << "\r\n";
    
    
    if (data.length()) {
        sentData << data << "\r\n\r\n";
    }

    SSL_write(sslConn, sentData.str().c_str(), sentData.str().length());
}

HttpResponse *DiscordSocket::rcvData()
{
    int len=100;
    char buffer[100000];
    std::string packet("");
    int endPos = -1;
    int totalRead = 0;
    bool hasContentLengthHeader = false;
    int contentLength = 0;
    
    while(1) {
        len = SSL_read(sslConn, buffer, 100);

        if (len <= 0) {
            int err = SSL_get_error(sslConn, len);
            if (
                err == SSL_ERROR_ZERO_RETURN ||
                err == SSL_ERROR_SYSCALL  ||
                err == SSL_ERROR_SSL ||
                err == SSL_ERROR_WANT_READ ||
                err == SSL_ERROR_WANT_WRITE
            ) {
                setError("Chyba při spracovaní paketu");
                std::cerr << "Error: " + errorMessage << std::endl;
                // return empty response
                return new HttpResponse("");
            }
            break;
        }

        buffer[len] = 0;

        if (len == 5 && (strcmp(buffer, "0\r\n\r\n") == 0)) {
            break;
        }

        packet += buffer;
        totalRead += len;

        int endHeader;
        if (!hasContentLengthHeader && (endHeader = packet.find("\r\n\r\n")) != -1) {
            int clHeaderPos;
            if ((clHeaderPos = packet.substr(0, endHeader).find("Content-Length: ")) != -1) {
                hasContentLengthHeader = true;
                HttpResponse header(packet);
                try
                {
                    contentLength = std::stoi(header.getHeader("Content-Length"));
                }
                catch(const std::exception& e)
                {
                    contentLength = 0;
                }
            }
        }

        if (
            hasContentLengthHeader &&
            ((packet.substr(0, packet.find("\r\n\r\n") + 4)).length() + contentLength == totalRead)
        ) {
            break;
        }
    }

    return new HttpResponse(packet);
}

void DiscordSocket::closeConnection()
{
    std::stringstream sentData;
    sentData << "GET / HTTP/1.1" << "\r\n"
        << "Host: " << hostName << "\r\n"
        << "Connection: close\r\n"
        << "UserAgent: DiscordBot (https://discordapp.com, 2.0)\r\n"
        << "\r\n";

    SSL_write(sslConn, sentData.str().c_str(), sentData.str().length());
    char buffer[RCV_BUFFER_LENGTH];
    int rcvLen;
    if ((rcvLen = SSL_read(sslConn, buffer, RCV_BUFFER_LENGTH)) <= 0) {
        std::cerr << "Nepodařilo se uzavřít spojení ze serverem" << std::endl;
    } else {
        std::cout << "Connection closed" << std::endl;;
    }
}


DiscordSocket::~DiscordSocket()
{
    closeConnection();
    close(sock);
    SSL_free(sslConn);
    SSL_CTX_free(sslCtx);
}