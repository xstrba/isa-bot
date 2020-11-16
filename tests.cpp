#include <iostream>
#include <unistd.h>
#include <string>
#include "JsonValue.cpp"
#include "JsonParser.cpp"
#include "HttpResponse.cpp"

using namespace std;

#define ASSERT(param) {try{ if (!(param)) throw exception();} catch (const exception &e) {cerr << "Test failed in file " << __FILE__ << " on line " << __LINE__ << endl;}}

/**
 * Run all tests
 */
void test();

int main()
{
    cout << "Tests started" << endl;
    test();
    cout << "Tests ended" << endl;
    return 0;
}

void test()
{
    JsonValue jsonValue;
    
    jsonValue.setString("test string");
    ASSERT(jsonValue.getType() == JDT_STRING);
    ASSERT(jsonValue.getString() == "test string");

    jsonValue.setObject();
    ASSERT(jsonValue.getType() == JDT_OBJECT);

    JsonValue *idValue = new JsonValue();
    idValue->setString("123456");
    jsonValue.setObjectParam("id", idValue);
    ASSERT(jsonValue.getObjectParam("id")->getString() == "123456");

    cout << "End of json value tests" << endl;

    JsonParser jsonParser("123456");
    ASSERT(jsonParser.getData()->getType() == JDT_STRING);
    ASSERT(jsonParser.getData()->getString() == "123456");

    jsonParser = JsonParser("{\"id\": 123456, \"name\":\"ferko\"}");
    ASSERT(jsonParser.getData()->getType() == JDT_OBJECT);
    ASSERT(jsonParser.getData()->getObjectParam("id")->getString() == "123456");
    ASSERT(jsonParser.getData()->getObjectParam("name")->getString() == "ferko");
    ASSERT(jsonParser.getData()->getObjectParam("random")->getString() == "");

    jsonParser = JsonParser("\"string\"");
    ASSERT(jsonParser.getData()->getType() == JDT_STRING);
    ASSERT(jsonParser.getData()->getString() == "string");

    jsonParser = JsonParser("[{\"id\": 123456, \"name\":\"ferko\"},{\"id\": 2222, \"name\":\"jozko\"}]");
    ASSERT(jsonParser.getData()->getType() == JDT_ARRAY);
    ASSERT(jsonParser.getData()->getArray()[0]->getObjectParam("id")->getString() == "123456");
    ASSERT(jsonParser.getData()->getArray()[0]->getObjectParam("name")->getString() == "ferko");
    ASSERT(jsonParser.getData()->getArray()[1]->getObjectParam("id")->getString() == "2222");
    ASSERT(jsonParser.getData()->getArray()[1]->getObjectParam("name")->getString() == "jozko");

    jsonParser = JsonParser("[123456, \"value\", {\"id\": 2222, \"name\":\"jozko\"}]");
    ASSERT(jsonParser.getData()->getType() == JDT_ARRAY);
    ASSERT(jsonParser.getData()->getArray()[0]->getString() == "123456");
    ASSERT(jsonParser.getData()->getArray()[1]->getString() == "value");
    ASSERT(jsonParser.getData()->getArray()[2]->getObjectParam("id")->getString() == "2222");
    ASSERT(jsonParser.getData()->getArray()[2]->getObjectParam("name")->getString() == "jozko");

    jsonParser = JsonParser("{\"id\": 123456, \"name\":\"ferko\", \"post\": {\"id\": 123, \"content\": \"text of post\\\"hey there\"}}");
    ASSERT(jsonParser.getData()->getType() == JDT_OBJECT);
    ASSERT(jsonParser.getData()->getObjectParam("id")->getString() == "123456");
    ASSERT(jsonParser.getData()->getObjectParam("name")->getString() == "ferko");
    ASSERT(jsonParser.getData()->getObjectParam("random")->getString() == "");
    ASSERT(jsonParser.getData()->getObjectParam("post")->getType() == JDT_OBJECT);
    ASSERT(jsonParser.getData()->getObjectParam("post")->getObjectParam("id")->getString() == "123");
    ASSERT(jsonParser.getData()->getObjectParam("post")->getObjectParam("content")->getString() == "text of post\\\"hey there");

    jsonParser = JsonParser("{\"id\": 123456, \"name\":\"ferko\", \"posts\": [\"123\", 456]}");
    ASSERT(jsonParser.getData()->getType() == JDT_OBJECT);
    ASSERT(jsonParser.getData()->getObjectParam("id")->getString() == "123456");
    ASSERT(jsonParser.getData()->getObjectParam("name")->getString() == "ferko");
    ASSERT(jsonParser.getData()->getObjectParam("random")->getString() == "");
    ASSERT(jsonParser.getData()->getObjectParam("posts")->getType() == JDT_ARRAY);
    ASSERT(jsonParser.getData()->getObjectParam("posts")->getArray()[0]->getString() == "123");
    ASSERT(jsonParser.getData()->getObjectParam("posts")->getArray()[1]->getString() == "456");

    jsonParser = JsonParser("");
    ASSERT(jsonParser.getData()->getString() == "");
    
    cout << "End of json parser tests" << endl;

    HttpResponse *response = new HttpResponse("");
    ASSERT(response->getCode() == 500);
    ASSERT(response->getProtocol() == "");
    ASSERT(response->getStatus() == "");
    ASSERT(response->getResponseDataText() == "");
    ASSERT(response->getResponseData()->getType() == JDT_STRING);
    ASSERT(response->getResponseData()->getString() == "");
    delete response;
  
    response = new HttpResponse("HTTP/1.1\r\n");
    ASSERT(response->getCode() == 500);
    ASSERT(response->getProtocol() == "");
    ASSERT(response->getStatus() == "");
    ASSERT(response->getResponseDataText() == "");
    ASSERT(response->getResponseData()->getType() == JDT_STRING);
    ASSERT(response->getResponseData()->getString() == "");
    delete response;
    
    response = new HttpResponse("HTTP/1.1 200\r\n");
    ASSERT(response->getCode() == 500);
    ASSERT(response->getProtocol() == "");
    ASSERT(response->getStatus() == "");
    ASSERT(response->getResponseDataText() == "");
    ASSERT(response->getResponseData()->getType() == JDT_STRING);
    ASSERT(response->getResponseData()->getString() == "");
    delete response;

    response = new HttpResponse("HTTP/1.1 200 OK\r\n");
    ASSERT(response->getCode() == 200);
    ASSERT(response->getProtocol() == "HTTP/1.1");
    ASSERT(response->getStatus() == "OK");
    ASSERT(response->getResponseDataText() == "");
    ASSERT(response->getResponseData()->getType() == JDT_STRING);
    ASSERT(response->getResponseData()->getString() == "");
    delete response;

    response = new HttpResponse("HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n");
    ASSERT(response->getCode() == 200);
    ASSERT(response->getProtocol() == "HTTP/1.1");
    ASSERT(response->getStatus() == "OK");
    ASSERT(response->getHeader("Content-Type") == "application/json");
    ASSERT(response->getResponseDataText() == "");
    ASSERT(response->getResponseData()->getType() == JDT_STRING);
    ASSERT(response->getResponseData()->getString() == "");
    delete response;

    response = new HttpResponse("HTTP/1.1 200 OK\r\nContent-Length: 20\r\nContent-Type: application/json\r\n\r\n");
    ASSERT(response->getCode() == 200);
    ASSERT(response->getProtocol() == "HTTP/1.1");
    ASSERT(response->getStatus() == "OK");
    ASSERT(response->getHeader("Content-Type") == "application/json");
    ASSERT(response->getHeader("Content-Length") == "20");
    ASSERT(response->getResponseDataText() == "");
    ASSERT(response->getResponseData()->getType() == JDT_STRING);
    ASSERT(response->getResponseData()->getString() == "");
    delete response;

    response = new HttpResponse("HTTP/1.1 200 OK\r\nContent-Length: 20\r\nContent-Type: application/json\r\n\r\n2\r\n[]\r\n");
    ASSERT(response->getCode() == 200);
    ASSERT(response->getProtocol() == "HTTP/1.1");
    ASSERT(response->getStatus() == "OK");
    ASSERT(response->getHeader("Content-Type") == "application/json");
    ASSERT(response->getHeader("Content-Length") == "20");
    ASSERT(response->getResponseDataText() == "[]");
    ASSERT(response->getResponseData()->getType() == JDT_ARRAY);
    delete response;

    response = new HttpResponse("HTTP/1.1 200 OK\r\nContent-Length: 20\r\nContent-Type: application/json\r\n\r\n[]\r\n\r\n");
    ASSERT(response->getCode() == 200);
    ASSERT(response->getProtocol() == "HTTP/1.1");
    ASSERT(response->getStatus() == "OK");
    ASSERT(response->getHeader("Content-Type") == "application/json");
    ASSERT(response->getHeader("Content-Length") == "20");
    ASSERT(response->getResponseDataText() == "[]");
    ASSERT(response->getResponseData()->getType() == JDT_ARRAY);
    delete response;

    cout << "End of http response tests" << endl;
}