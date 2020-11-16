/**
 * Implementation of HttpResponse class
 * 
 * @file HttpResponse.cpp
 * @author Boris Strbak (xstrba05)
 * @date 2020-11-18
 */

#include "HttpResponse.hpp"

HttpResponse::HttpResponse(std::string responseText)
{
    try
    {
        this->responseText = responseText;

        // copy response because we will erase it. At success completely
        std::string responseCopy = responseText;

        // get protocl is part until first space
        protocol = responseCopy.substr(0, responseCopy.find(' '));
        responseCopy.erase(0, responseCopy.find(' ') + 1);

        if (responseCopy.find(' ') == ULONG_MAX)
        {
            throw std::exception();
        }

        // get response code
        responseCode = std::stoi(responseCopy.substr(0, responseCopy.find(' ')));
        responseCopy.erase(0, responseCopy.find(' ') + 1);

        responseStatus = responseCopy.substr(0, responseCopy.find("\r\n"));
        responseCopy.erase(0, responseCopy.find("\r\n") + 2);

        // get headers part until the wirst double line end
        if (responseCopy.find("\r\n\r\n") != ULONG_MAX) {
            std::string headerString = responseCopy.substr(0, responseCopy.find("\r\n\r\n") + 2);

            // we got header string stored so we can erase it from response copy
            responseCopy.erase(0, responseCopy.find("\r\n\r\n") + 4);

            int lineEndPos = -1;
            while ((lineEndPos = headerString.find("\r\n")) != -1)
            {

                // take content until first line end
                std::string headerLine = headerString.substr(0, lineEndPos);
                headerString.erase(0, lineEndPos + 2);

                if (headerLine.find(": ") != ULONG_MAX)
                {
                    // find separator of key and value

                    // key (or name) is of header is first part of string seprated by separator
                    std::string name = headerLine.substr(0, headerLine.find(": "));

                    // remove this part of string so the rest is our value
                    headerLine.erase(0, headerLine.find(": ") + 2);

                    if (name.length())
                    {
                        headers[name] = headerLine;
                    }
                }
            }
        }

        // header is mostly succeded by one line giving content length of response
        // but also check if it's not the last line
        unsigned long endLinePos = responseCopy.find("\r\n");
        if (endLinePos != ULONG_MAX && endLinePos != (responseCopy.length() - 2) && endLinePos != responseCopy.find("\r\n\r\n"))
        {
            responseCopy.erase(0, endLinePos + 2);
        }

        // process everythin else until nex line end
        // is mostly json value but can be also an html so it will just store
        // as string json value
        responseDataText = responseCopy.find("\r\n") != ULONG_MAX
                               ? responseCopy.substr(0, responseCopy.find("\r\n"))
                               : responseCopy;
        responseData = JsonParser(responseDataText).getData();
    }
    catch (const std::exception &e)
    {
        // wasn't properly formatted HTTP response so set code to 500

        protocol = "";
        responseCode = 500;
        responseDataText = "";
        responseData = JsonParser("").getData();
    }
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

    if (it == headers.end())
    {
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