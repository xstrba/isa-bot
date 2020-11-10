/**
 * Implementation of DiscordSocket class
 * 
 * @file DiscordSocket.cpp
 * @author Boris Strbak (xstrba05)
 * @date 2020-11-18
 */

#include "DiscordSocket.hpp"

DiscordSocket::DiscordSocket()
{
    port = HTTPS_PORT; // default port
    hostName = "discord.com"; // discord host name
    error = false; // no error
    sock = -1; // no socket
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
    struct addrinfo hints = {}, *addrs; // initialize objects for getting ip address of host
    hints.ai_family = AF_INET; // set IPv4
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP; // protocol tcp (used with sock stream)

    // set port number as string
    char portStr[16] = {};
    sprintf(portStr, "%d", port);

    // get address info from host name and port
    int error = getaddrinfo(hostName.c_str(), portStr, &hints, &addrs);
    if (error)
    {
        return setError("Nepodařilo se připojít na server " + hostName);
    }

    // for each address try to connect with socket
    for (struct addrinfo *addr = addrs; addr != NULL; addr = addr->ai_next)
    {
        sock = socket(addrs->ai_family, addrs->ai_socktype, addrs->ai_protocol);

        // socket was created
        if (sock < 0) {
            continue;
        }

        if (connect(sock, addr->ai_addr, addr->ai_addrlen) == 0)
        {
            // conection was successful

            // get IPv4 string
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

    // initialize ssl context
    sslCtx = SSL_CTX_new(SSLv23_client_method());
    
    // initialize ssl connection
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
    int len = RCV_BUFFER_LENGTH; // will get packet in chunks
    char buffer[RCV_BUFFER_LENGTH];
    std::string packet(""); // will store whole packet
    long unsigned int totalRead = 0; // size of total data read
    bool hasContentLengthHeader = false;
    int contentLength = 0;

    while (1)
    {
        len = SSL_read(sslConn, buffer, RCV_BUFFER_LENGTH);

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

        // send of string so we don't have any previously loaded data
        buffer[len] = 0;

        // 0\r\n\r\n is a last chunk of packet send to signalize end of message
        // is not always present
        if (len == 5 && (strcmp(buffer, "0\r\n\r\n") == 0))
        {
            break;
        }

        packet += buffer;
        totalRead += len;

        int endHeader;
        if (!hasContentLengthHeader && (endHeader = packet.find("\r\n\r\n")) != -1)
        {
            // found double line ending and has no content-length header loaded

            int clHeaderPos;
            if ((clHeaderPos = packet.substr(0, endHeader).find("Content-Length: ")) != -1)
            {
                // find Content-Length header part in whole part of packet currently loaded
                // no 0\r\n\r\n will coe in this case

                hasContentLengthHeader = true;

                // temporary object to process currently loaded data
                // should store headers properly
                HttpResponse header(packet);
                try
                {
                    // get content length from header
                    contentLength = std::stoi(header.getHeader("Content-Length"));
                }
                catch (const std::exception &e)
                {
                    // if it fails set length to 0
                    contentLength = 0;
                }
            }
        }

        if (
            hasContentLengthHeader &&
            ((packet.substr(0, packet.find("\r\n\r\n") + 4)).length() + contentLength == totalRead))
        {
            // if has content-length header the end of packet is the end of header (or start of body. + 4)
            // + content length. If it equals to total data read we are at the end of packet
            break;
        }
    }

    return new HttpResponse(packet);
}

void DiscordSocket::closeConnection()
{
    // send http request with header Connection: close
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
    }
}

DiscordSocket::~DiscordSocket()
{
    closeConnection();
    close(sock);
    SSL_free(sslConn);
    SSL_CTX_free(sslCtx);
}