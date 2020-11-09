#ifndef JSON_PARSER_HPP
#define JSON_PARSER_CPP

#include <unistd.h>
#include <string>
#include <variant>
#include <vector>
#include "JsonValue.hpp"

class JsonParser
{
    public:
        /**
         * Json parser constructor
         */
        JsonParser(std::string jsonData);

        /**
         * Return parsed data
         * 
         * @return JsonValue
         */
        JsonValue *getData();
    protected:
    private:
        std::string jsonData;
        JsonValue *data;

};

#endif