#include "HttpResponse.hpp"

HttpResponse::HttpResponse(std::string responseText)
{
    this->responseText = responseText;
    std::string responseCopy = responseText;
    protocol = responseCopy.substr(0, responseCopy.find(' '));
    responseCopy.erase(0, responseCopy.find(' ') + 1);
    
    try
    {
        responseCode = std::stoi(responseCopy.substr(0, responseCopy.find(' ')));
        responseCopy.erase(0, responseCopy.find(' ') + 1);
    }
    catch(const std::exception& e)
    {
        protocol = "";
        responseCode = 500;
        responseDataText = "";
        responseData = JsonParser("").getData();
    }

    responseStatus = responseCopy.substr(0, responseCopy.find("\r\n"));
    responseCopy.erase(0, responseCopy.find("\r\n") + 2);

    std::string headerString = responseCopy.substr(0, responseCopy.find("\r\n\r\n"));
    if (headerString.length()) {
        responseCopy.erase(0, responseCopy.find("\r\n\r\n") + 4);
        int lineEndPos = -1;
        while((lineEndPos = headerString.find("\r\n")) != -1) {
            std::string headerLine = headerString.substr(0, lineEndPos);
            headerString.erase(0, lineEndPos + 2);
            
            if (headerLine.find(": ") != -1) {
                std::string name = headerLine.substr(0, headerLine.find(": "));
                headerLine.erase(0, headerLine.find(": ") + 2);

                if (name.length()) {
                    headers[name] = headerLine;
                }
            }
        }
    }

    if (responseCopy.find("\r\n") != -1) {    
        responseCopy.erase(0, responseCopy.find("\r\n") + 2);
    }
    
    responseDataText = responseCopy.find("\r\n") != -1
        ? responseCopy.substr(0, responseCopy.find("\r\n"))
        : responseCopy;
    responseData = JsonParser(responseDataText).getData();
}

int HttpResponse::getCode()
{
    return responseCode;
}

std::string HttpResponse::getProtocol()
{
    return protocol;
}

std::string HttpResponse::getStatus()
{
    return responseStatus;
}

std::string HttpResponse::getResponseDataText()
{
    return responseDataText;
}

std::string HttpResponse::getHeader(std::string headerName)
{
    std::unordered_map<std::string, std::string>::iterator it = headers.find(headerName);

    if (it == headers.end()) {
        return "";
    }

    return it->second;
}

JsonValue *HttpResponse::getResponseData()
{
    return responseData;
}

HttpResponse::~HttpResponse()
{
    delete responseData;
}