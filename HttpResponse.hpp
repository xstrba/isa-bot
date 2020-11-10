/**
 * Definition of HttpResponse class
 * 
 * @file HttpResponse.hpp
 * @author Boris Strbak (xstrba05)
 * @date 2020-11-18
 */


#ifndef HTTP_RESPONSE_HPP
#define HTTP_RESPONSE_HPP

#include <unistd.h>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <limits.h>
#include "JsonValue.hpp"
#include "JsonParser.hpp"

/**
 * HttpResponse class
 * 
 * Class storing http response from socket
 * 
 * @param std::string responseText whole response as string
 * @param std::string protocol protocol of response (e.g. HTTP/1.1)
 * @param int responseCode response code (e.g. 200)
 * @param std::string responseStatus response status (e.g. "OK")
 * @param std::unordered_map<std::string, std::string> headers array of headers
 * @param std::string responseDataText response data as string
 * @param JsonValue *responseData response data as json value
 */
class HttpResponse
{
    public:
        /**
         * HttpResponse constructor
         * 
         * @param std::string responseText
         */
        HttpResponse(std::string responseText);

        /**
         * HttpResponse destructor
         */
        ~HttpResponse();

        /**
         * Get response code
         * 
         * @return int response code
         */
        int getCode();

        /**
         * Get response protocol
         * 
         * @return std::string protocol
         */
        std::string getProtocol();

        /**
         * Get response status
         * 
         * @return std::string status
         */
        std::string getStatus();

        /**
         * Get reponse text data
         * 
         * @return std::string
         */
        std::string getResponseDataText();

        /**
         * Returns header value
         * 
         * @param std::string headerName name of header
         * @return std::string value of header
         */
        std::string getHeader(std::string headerName);

        /**
         * Get response data object
         * 
         * @return JsonValue
         */
        JsonValue *getResponseData();

    protected:
    private:
        std::string responseText;
        std::string protocol;
        int responseCode;
        std::string responseStatus;
        std::unordered_map<std::string, std::string> headers;
        std::string responseDataText;
        JsonValue *responseData;
};

#endif