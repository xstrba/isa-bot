#ifndef JSON_VALUE_HPP
#define JSON_VALUE_HPP

#include <string>
#include <variant>
#include <vector>
#include <unordered_map> 
#include <iostream>

/**
 * Enumerator of datatypes for json
 */
enum JsonDataTypesEnum {JDT_ARRAY, JDT_OBJECT, JDT_STRING};

class JsonValue
{
    public:
        /**
         * Json value constructor
         */
        JsonValue();

        /**
         * Json value destructor
         */
        ~JsonValue();

        /**
         * Set string as value
         * 
         * @param std::string value
         * @return void
         */
        void setString(std::string value);

        /**
         * Set array as value
         * 
         * @param std::vector<JsonValue> value
         * @return void
         */
        void setArray(std::vector<JsonValue *> value);

        /**
         * Set object as value
         * 
         * @return void
         */
        void setObject();

        /**
         * Sets object parameter
         * 
         * @param std::string key
         * @param JsonValue value
         * @return void
         */
        void setObjectParam(std::string key, JsonValue *value);

        /**
         * Get object param by key
         * 
         * @param std::string key
         * @return JsonValue
         */
        JsonValue *getObjectParam(std::string key);

        /**
         * Get string value
         * 
         * @return std::string
         */
        std::string getString();

        /**
         * Get array value
         * 
         * @return std::vector<JsonValue *>
         */
        std::vector<JsonValue *> getArray();

        /**
         * Get type of data stored in json value
         * 
         * @return JsonDataTypesEnum
         */
        JsonDataTypesEnum getType();
    protected:
    private:
        JsonDataTypesEnum dataType;
        std::variant<std::vector<JsonValue *>, std::string> data;
        std::unordered_map<std::string, JsonValue *> params;
};

#endif