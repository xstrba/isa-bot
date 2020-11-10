#include "JsonParser.hpp"

int processFirstArrayValue(std::string json, JsonValue *jsonValue);
int processFirstObjectValue(std::string json, JsonValue *jsonValue);

int processFirstArrayValue(std::string json, JsonValue *jsonValue)
{
    std::vector<JsonValue *> values = {};

    if (json[0] != '[')
    {
        jsonValue->setArray(values);
        return -1;
    }

    if (json[1] == ']')
    {
        jsonValue->setArray(values);
        return 1;
    }

    unsigned long i = 1;
    bool isProcessingValue = false;
    while (i < json.length())
    {
        if (json[i] == '{' && !isProcessingValue)
        {
            JsonValue *newValue = new JsonValue();
            int endObjectIndex = processFirstObjectValue(json.substr(i), newValue);
            values.push_back(newValue);
            i += endObjectIndex + 1;
            continue;
        }
        else if (json[i] == '[' && !isProcessingValue)
        {
            JsonValue *newValue = new JsonValue();
            int endArrayIndex = processFirstArrayValue(json.substr(i), newValue);
            values.push_back(newValue);
            i += endArrayIndex + 1;
            continue;
        }
        else if (json[i] == '"' && !isProcessingValue)
        {
            std::string stringValue = "";
            unsigned long k = i + 1;
            bool hasInnerSlash = false;
            while (k < json.length())
            {
                if (json[k] == '\\')
                {
                    hasInnerSlash = true;
                    stringValue += json[k];
                    k++;
                    continue;
                }
                else if (json[k] == '"')
                {
                    if (hasInnerSlash)
                    {
                        stringValue += json[k];
                        hasInnerSlash = false;
                        k++;
                        continue;
                    }
                    else
                    {
                        k++;
                        break;
                    }
                }
                else
                {
                    hasInnerSlash = false;
                    stringValue += json[k];
                    k++;
                    continue;
                }
            }

            i = k;

            JsonValue *newValue = new JsonValue();
            newValue->setString(stringValue);
            values.push_back(newValue);
            continue;
        }
        else if (json[i] == ',' && !isProcessingValue)
        {
            i++;
            while(json[i] == ' ') {
                i++;
            }
            continue;
        }
        else if (json[i] == ']' && !isProcessingValue)
        {
            jsonValue->setArray(values);
            return i;
        }
        else
        {
            std::string stringValue = "";
            int commaIndex = json.find(",", i);
            int arrayEndIndex = json.find("]", i);
            if (commaIndex >= 0 && commaIndex < arrayEndIndex)
            {
                stringValue = json.substr(i, commaIndex - i);
                i = commaIndex;
            }
            else
            {
                stringValue = json.substr(i, arrayEndIndex - i);
                i = arrayEndIndex;
            }

            JsonValue *newValue = new JsonValue();
            newValue->setString(stringValue);
            values.push_back(newValue);
            continue;
        }

        i++;
    }

    return -1;
}

int processFirstObjectValue(std::string json, JsonValue *jsonValue)
{
    jsonValue->setObject();
    if (json[0] != '{')
    {
        return -1;
    }

    if (json[1] == '}')
    {
        return 1;
    }

    unsigned long i = 1;
    bool isProcessingValue = false;
    bool isProcessingKey = false;
    bool isKey = false;
    bool hasSlash = false;
    std::string stringValue = "";
    std::string key = "";
    while (i < json.length())
    {
        if (isKey && isProcessingKey)
        {
            if (json[i] == '\\')
            {
                hasSlash = true;
                key += '\\';
            }
            else if (json[i] == '"')
            {
                if (hasSlash)
                {
                    key += json[i];
                    hasSlash = false;
                }
                else
                {
                    isProcessingKey = false;
                }
            }
            else
            {
                hasSlash = false;
                key += json[i];
            }
        }

        if (json[i] == '"' && !isProcessingKey && !isKey)
        {
            isKey = true;
            isProcessingValue = false;
            isProcessingKey = true;
            key = "";
        }

        if (!isProcessingKey && !isProcessingValue && json[i] == ':')
        {
            i++;
            while(json[i] == ' ') {
                i++;
            }

            isProcessingKey = false;
            stringValue = "";
            JsonValue *newValue = new JsonValue();

            if (json[i] == '{')
            {
                int endObjectIndex = processFirstObjectValue(json.substr(i), newValue);
                if (endObjectIndex >= 0)
                {
                    i += endObjectIndex + 1;
                }
                else
                {
                    i++;
                }

                jsonValue->setObjectParam(key, newValue);
            }
            else if (json[i] == '[')
            {
                int endArrayIndex = processFirstArrayValue(json.substr(i), newValue);
                if (endArrayIndex >= 0)
                {
                    i += endArrayIndex + 1;
                }
                else
                {
                    i++;
                }

                jsonValue->setObjectParam(key, newValue);
            }
            else if (json[i] == '"')
            {
                unsigned long k = i + 1;
                bool hasInnerSlash = false;
                while (k < json.length())
                {
                    if (json[k] == '\\')
                    {
                        hasInnerSlash = true;
                        stringValue += json[k];
                        k++;
                        continue;
                    }
                    else if (json[k] == '"')
                    {
                        if (hasInnerSlash)
                        {
                            stringValue += json[k];
                            hasInnerSlash = false;
                            k++;
                            continue;
                        }
                        else
                        {
                            k++;
                            break;
                        }
                    }
                    else
                    {
                        hasInnerSlash = false;
                        stringValue += json[k];
                        k++;
                        continue;
                    }
                }

                i = k;

                newValue->setString(stringValue);
                jsonValue->setObjectParam(key, newValue);
            }
            else
            {
                // take string to next comma
                int commaIndex = json.find(",", i);
                int endObjectIndex = json.find("}", i);
                if (commaIndex >= 0 && commaIndex < endObjectIndex)
                {
                    stringValue = json.substr(i, commaIndex - i);
                    i = commaIndex;

                }
                else
                {
                    stringValue = json.substr(i, endObjectIndex - i);
                    i = endObjectIndex;
                }

                newValue->setString(stringValue);
                jsonValue->setObjectParam(key, newValue);
            }

            isProcessingValue = false;
            continue;
        }

        if (json[i] == ',' && !isProcessingKey && !isProcessingValue)
        {
            isKey = false;
            i ++;
            while (json[i] == ' ') {
                i++;
            }

            continue;
        }

        if (json[i] == '}' && !isProcessingKey && !isProcessingValue)
        {
            return i;
        }

        i++;
    }

    return -1;
}

JsonParser::JsonParser(std::string json)
{
    data = new JsonValue();
    if (!json.length())
    {
        data->setString("");
    }
    else
    {
        switch (json[0])
        {
        case '[':
            processFirstArrayValue(json, data);
            break;
        case '{':
            processFirstObjectValue(json, data);
            break;
        case '"':
            data->setString(json.substr(0, json.length() - 1));
            break;
        default:
            data->setString(json);
            break;
        }
    }
}


JsonValue *JsonParser::getData()
{
    return data;
}