#include "DiscordSocket.hpp"

DiscordSocket::DiscordSocket()
{
    port = HTTPS_PORT;
    hostName = "discord.com";
    error = false;
    sock = -1;
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
    struct addrinfo hints = {}, *addrs;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = 0;

    char portStr[16] = {};
    sprintf(portStr, "%d", port);

    int error = getaddrinfo(hostName.c_str(), portStr, &hints, &addrs);
    if (error)
    {
        return setError("Nepodařilo se připojít na server " + hostName);
    }

    for (struct addrinfo *addr = addrs; addr != NULL; addr = addr->ai_next)
    {
        sock = socket(addrs->ai_family, addrs->ai_socktype, addrs->ai_protocol);

        if (sock < 0) {
            continue;
        }

        if (connect(sock, addr->ai_addr, addr->ai_addrlen) == 0)
        {
            char ipStr[INET_ADDRSTRLEN];
            inet_ntop(addr->ai_family, &((struct sockaddr_in *)addr->ai_addr)->sin_addr, ipStr, sizeof ipStr);
            ipAddress = ipStr;
            break;
        }

        close(sock);
        sock = -1;
    }

    freeaddrinfo(addrs);

    if (sock < 0) {
        return setError("Nepodařilo se vytvořit spojení pomocí soketu");
    }

    SSL_load_error_strings ();
    SSL_library_init ();
    sslCtx = SSL_CTX_new (SSLv23_client_method ());
    
    sslConn = SSL_new(sslCtx);
    SSL_set_fd(sslConn, sock);
    
    if (SSL_connect(sslConn) != 1) {
        SSL_free(sslConn);
        SSL_CTX_free(sslCtx);
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

    if (data.length())
    {
        sentData << data << "\r\n\r\n";
    }

    SSL_write(sslConn, sentData.str().c_str(), sentData.str().length());
}

HttpResponse *DiscordSocket::rcvData()
{
    int len = 100;
    char buffer[100000];
    std::string packet("");
    long unsigned int totalRead = 0;
    bool hasContentLengthHeader = false;
    int contentLength = 0;

    while (1)
    {
        len = SSL_read(sslConn, buffer, 100);

        if (len <= 0)
        {
            int err = SSL_get_error(sslConn, len);
            if (
                err == SSL_ERROR_ZERO_RETURN ||
                err == SSL_ERROR_SYSCALL ||
                err == SSL_ERROR_SSL ||
                err == SSL_ERROR_WANT_READ ||
                err == SSL_ERROR_WANT_WRITE)
            {
                setError("Chyba při spracovaní paketu");
                std::cerr << "Error: " + errorMessage << std::endl;
                // return empty response
                return new HttpResponse("");
            }
            break;
        }

        buffer[len] = 0;

        if (len == 5 && (strcmp(buffer, "0\r\n\r\n") == 0))
        {
            break;
        }

        packet += buffer;
        totalRead += len;

        int endHeader;
        if (!hasContentLengthHeader && (endHeader = packet.find("\r\n\r\n")) != -1)
        {
            int clHeaderPos;
            if ((clHeaderPos = packet.substr(0, endHeader).find("Content-Length: ")) != -1)
            {
                hasContentLengthHeader = true;
                HttpResponse header(packet);
                try
                {
                    contentLength = std::stoi(header.getHeader("Content-Length"));
                }
                catch (const std::exception &e)
                {
                    contentLength = 0;
                }
            }
        }

        if (
            hasContentLengthHeader &&
            ((packet.substr(0, packet.find("\r\n\r\n") + 4)).length() + contentLength == totalRead))
        {
            break;
        }
    }

    return new HttpResponse(packet);
}

void DiscordSocket::closeConnection()
{
    std::stringstream sentData;
    sentData << "GET / HTTP/1.1"
             << "\r\n"
             << "Host: " << hostName << "\r\n"
             << "Connection: close\r\n"
             << "UserAgent: DiscordBot (https://discordapp.com, 2.0)\r\n"
             << "\r\n";

    SSL_write(sslConn, sentData.str().c_str(), sentData.str().length());
    char buffer[RCV_BUFFER_LENGTH];
    int rcvLen;
    if ((rcvLen = SSL_read(sslConn, buffer, RCV_BUFFER_LENGTH)) <= 0)
    {
        std::cerr << "Nepodařilo se uzavřít spojení ze serverem" << std::endl;
    }
    else
    {
        std::cout << "Connection closed" << std::endl;
        ;
    }
}

DiscordSocket::~DiscordSocket()
{
    closeConnection();
    close(sock);
    SSL_free(sslConn);
    SSL_CTX_free(sslCtx);
}