#include "JsonValue.hpp"

JsonValue::JsonValue()
{
    dataType = JDT_STRING;
    data = "";
}

void JsonValue::setString(std::string value)
{
    data = value;
    dataType = JDT_STRING;
}

void JsonValue::setArray(std::vector<JsonValue *> value)
{
    data = value;
    dataType = JDT_ARRAY;
}

void JsonValue::setObject()
{
    dataType = JDT_OBJECT;
}

void JsonValue::setObjectParam(std::string key, JsonValue *value)
{
    params[key] = value;
}

JsonValue *JsonValue::getObjectParam(std::string key)
{
    // if data type is not object returns json value with empty string
    if (dataType != JDT_OBJECT) {
        return new JsonValue();
    }

    std::unordered_map<std::string, JsonValue *>::iterator it = params.find(key);

    // key is not in param list return empty string 
    if (it == params.end()) {
        return new JsonValue();
    }

    return it->second;
}

std::string JsonValue::getString()
{
    if (dataType != JDT_STRING) {
        return "";
    }

    return std::get<std::string>(data);
}

std::vector<JsonValue *> JsonValue::getArray()
{
    if (dataType != JDT_ARRAY) {
        return {};
    }

    return std::get<std::vector<JsonValue *>>(data);
}

JsonDataTypesEnum JsonValue::getType()
{
    return dataType;
}

JsonValue::~JsonValue()
{
    // first destroy params unordered list
    // no need to check if JsonValue type is object
    for (auto it = params.begin(); it != params.end(); it++)
    {
        delete it->second;
    }
    params.clear();
    
    // check if data is vector
    if (data.index() == 0)
    {

        // then destroy every item in array
        std::vector<JsonValue *> values = std::get<std::vector<JsonValue *>>(data);
        for (auto it = values.begin(); it != values.end(); it++)
        {
            delete (*it);
        }
    }
}