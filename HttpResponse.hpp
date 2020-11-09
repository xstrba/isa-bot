#ifndef HTTP_RESPONSE_HPP
#define HTTP_RESPONSE_HPP

#include <unistd.h>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map> 
#include "JsonValue.hpp"
#include "JsonParser.hpp"

class HttpResponse
{
    public:
        /**
         * Http response constructor
         * 
         * @param std::string responseText
         */
        HttpResponse(std::string responseText);

        /**
         * Http response destructor
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