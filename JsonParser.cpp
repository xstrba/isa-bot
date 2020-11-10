/**
 * Definition of JsonParser class
 * 
 * @file JsonParser.hpp
 * @author Boris Strbak (xstrba05)
 * @date 2020-11-18
 */

#include "JsonParser.hpp"

/**
 * Processes first array value in json string and stores value in given
 * object. String should start with '['. Otherwise will return empty array.
 * 
 * @param std::string json
 * @param JsonValue jsonValue
 * @return int position of closing bracket ']' in json string.
 */
int processFirstArrayValue(std::string json, JsonValue *jsonValue);

/**
 * Processes first object value in json string and stores value in given
 * object. String should start with '{'. Otherwise will return empty object.
 * 
 * @param std::string json
 * @param JsonValue jsonValue
 * @return int position of closing bracket '}' in json string.
 */
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
            // is not processing any value and found object bracket
            
            JsonValue *newValue = new JsonValue();

            // process first object in json string  starting from position i
            int endObjectIndex = processFirstObjectValue(json.substr(i), newValue);
            values.push_back(newValue);

            // continue at the end of object
            i += endObjectIndex + 1;
            continue;
        }
        else if (json[i] == '[' && !isProcessingValue) // 
        {
            // is not processing any value and found array bracket

            JsonValue *newValue = new JsonValue();

            // process first array in json starting from position i
            int endArrayIndex = processFirstArrayValue(json.substr(i), newValue);
            values.push_back(newValue);

            // continue from end of array
            i += endArrayIndex + 1;
            continue;
        }
        else if (json[i] == '"' && !isProcessingValue)
        {
            // is not processing any value and found " so will process string value

            std::string stringValue = "";
            unsigned long k = i + 1;
            bool hasInnerSlash = false;
            while (k < json.length())
            {
                if (json[k] == '\\')
                {
                    // if i find slash in string i know that if
                    // double quote follows it is not ending the string

                    hasInnerSlash = true;
                    stringValue += json[k];
                    k++;
                    continue;
                }
                else if (json[k] == '"')
                {
                    if (hasInnerSlash)
                    {
                        // if has double qoute after slash siply add it to string value

                        stringValue += json[k];
                        hasInnerSlash = false;
                        k++;
                        continue;
                    }
                    else
                    {
                        // else ends processing string value

                        k++;
                        break;
                    }
                }
                else
                {
                    // process all other characters in string

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
            // if founf array items separator will move to next value

            i++;
            while(json[i] == ' ') {
                i++;
            }
            continue;
        }
        else if (json[i] == ']' && !isProcessingValue)
        {
            // found array end will set value of object and return position

            jsonValue->setArray(values);
            return i;
        }
        else
        {
            // processing non string, non object, non array value

            std::string stringValue = "";

            // index of array items separator
            int commaIndex = json.find(",", i);

            // index of array closing bracket
            int arrayEndIndex = json.find("]", i);
            if (commaIndex >= 0 && commaIndex < arrayEndIndex)
            {
                // if array items separator is first take everything
                // after current position and right before separator

                stringValue = json.substr(i, commaIndex - i);
                i = commaIndex;
            }
            else
            {
                // same for closing bracket

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

    // is true when processing value of some key
    bool isProcessingValue = false;

    // is set to true when processing key
    bool isProcessingKey = false;

    // is set to true since start of getting key until end of processing value of this key
    bool isKey = false;
    bool hasSlash = false;
    std::string stringValue = "";
    std::string key = "";
    while (i < json.length())
    {
        if (isKey && isProcessingKey)
        {
            // processing key

            if (json[i] == '\\')
            {
                // if i find slash in string i know that if
                // double quote follows it is not ending the string

                hasSlash = true;
                key += '\\';
            }
            else if (json[i] == '"')
            {
                if (hasSlash)
                {
                    // if has double qoute after slash siply add it to string value

                    key += json[i];
                    hasSlash = false;
                }
                else
                {
                    // else ends processing string value

                    isProcessingKey = false;
                }
            }
            else
            {
                // process any other character

                hasSlash = false;
                key += json[i];
            }
        }

        if (json[i] == '"' && !isProcessingKey && !isKey)
        {
            // not processing key and key is not set and found double quote
            // will start processing key

            isKey = true;
            isProcessingValue = false;
            isProcessingKey = true;
            key = "";
        }

        if (!isProcessingKey && !isProcessingValue && json[i] == ':')
        {
            // is not processing key and is not processing value and found ':'
            // will skip spaces and process value

            i++;
            while(json[i] == ' ') {
                i++;
            }

            isProcessingKey = false;
            stringValue = "";
            JsonValue *newValue = new JsonValue();

            if (json[i] == '{')
            {
                // found object bracket so will process first object starting from current position

                int endObjectIndex = processFirstObjectValue(json.substr(i), newValue);
                if (endObjectIndex >= 0)
                {
                    i += endObjectIndex + 1;
                }
                else
                {
                    // some error happend
                    // mostly badly formatted json

                    i++;
                }

                jsonValue->setObjectParam(key, newValue);
            }
            else if (json[i] == '[')
            {
                // found array bracket so will process first array starting from current position

                int endArrayIndex = processFirstArrayValue(json.substr(i), newValue);
                if (endArrayIndex >= 0)
                {
                    i += endArrayIndex + 1;
                }
                else
                {
                    // some error happend
                    // mostly badly formatted json

                    i++;
                }

                jsonValue->setObjectParam(key, newValue);
            }
            else if (json[i] == '"')
            {
                // found quote so will process string value

                unsigned long k = i + 1;
                bool hasInnerSlash = false;
                while (k < json.length())
                {
                    if (json[k] == '\\')
                    {
                        // found slash so if quote is next it does not mean
                        // the end of string

                        hasInnerSlash = true;
                        stringValue += json[k];
                        k++;
                        continue;
                    }
                    else if (json[k] == '"')
                    {
                        if (hasInnerSlash)
                        {
                            // if has slash just add quote to string and continue

                            stringValue += json[k];
                            hasInnerSlash = false;
                            k++;
                            continue;
                        }
                        else
                        {
                            // else end processing string value

                            k++;
                            break;
                        }
                    }
                    else
                    {
                        // just add any other character

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
                // found value that is not string, object nor array

                // find index of value separator
                int commaIndex = json.find(",", i);

                // find index of object ending bracket
                int endObjectIndex = json.find("}", i);
                if (commaIndex >= 0 && commaIndex < endObjectIndex)
                {
                    // value separator is first so process value until it

                    stringValue = json.substr(i, commaIndex - i);
                    i = commaIndex;

                }
                else
                {
                    // process value until ending bracket

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
            // is not processing key nor value and found value separator
            // skip all spaces until next key

            isKey = false;
            i ++;
            while (json[i] == ' ') {
                i++;
            }

            continue;
        }

        if (json[i] == '}' && !isProcessingKey && !isProcessingValue)
        {
            // is not processing key nor value and found object end
            // return index

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
        // process json string according to first character
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
                // is not even json but store it as string
                data->setString(json);
                break;
        }
    }
}


JsonValue *JsonParser::getData()
{
    return data;
}