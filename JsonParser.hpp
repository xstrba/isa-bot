#ifndef JSON_PARSER_HPP
#define JSON_PARSER_CPP

#include <unistd.h>
#include <string>
#include <variant>
#include <vector>
#include "JsonValue.hpp"

/**
 * Class JsonParser
 * 
 * Class for parsing json string into objects that provide some 
 * interface for accessing value
 * 
 * @param std::string jsonData json value passed to constructor
 * @param JsonValue data parsed json data
 */
class JsonParser
{
    public:
        /**
         * JsonParser constructor
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