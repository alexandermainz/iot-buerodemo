

#include "RestCall.h"

#define DEBUG

#ifdef DEBUG
#define DEBUG_PRINT(x) Serial.print(x);
#else
#define DEBUG_PRINT(x)
#endif

RestCall::RestCall(const char* host, const int port)
{
    this->host = host;
    this->port = port;
    this->contentType = "text/plain";
}

int RestCall::get(const char *url)
{
    return request("GET", url, NULL);
}

int RestCall::post(const char *url, const char *body)
{
    return request("POST", url, body);
}

void RestCall::setContentType(const char *contentTypeValue)
{
    contentType = contentTypeValue;
}

int RestCall::request(const char *method, const char *url, const char *body)
{
    int statusCode = -1;
    if (!wifiClient.connect(host, port))
    {
        DEBUG_PRINT("RestCall: No connection!\n");
        return 0;
    }
    DEBUG_PRINT("RestCall: Requesting ");
    DEBUG_PRINT(method);
    DEBUG_PRINT(" ");
    DEBUG_PRINT(url);
    DEBUG_PRINT("\n");

    String request = String(method) + " " + url + " HTTP/1.1\r\n" +
               "Host: " + this->host + "\r\n" +
               "User-Agent: Arduino-RestCall\r\n" +
               "Accept: " + this->contentType + "\r\n" +
               "Accept-Encoding:none\r\n" +
               "Connection: keep-alive\r\n\r\n";
    DEBUG_PRINT(request.c_str());
    wifiClient.print(request);
    if (body != NULL) {
        wifiClient.print(body);
        wifiClient.print("\r\n");
    }


    statusCode = readStatus();
    DEBUG_PRINT("HTTP-Status: ");
    DEBUG_PRINT(statusCode); DEBUG_PRINT("\n");
    return statusCode;
}

int RestCall::readStatus()
{
    int statuscode = 400; // return "Bad Request" when no status in repsonse
    while (wifiClient.connected()) {
        String line = wifiClient.readStringUntil('\n');
        if (line.startsWith("HTTP")) {
            int space1 = line.indexOf(' ');
            int space2 = line.indexOf(' ', space1+1);
            if (space1 != -1 && space2 > space1)
                statuscode = atoi(line.substring(space1+1, space2).c_str());
        }
        DEBUG_PRINT(line.c_str()); DEBUG_PRINT("\n");
        if (line == "\r") {
            DEBUG_PRINT("headers received\n");
            break;
        }
    }

    return statuscode;
}

String RestCall::readNextLine()
{
    if (wifiClient.connected() && wifiClient.available()) {
        String s = wifiClient.readStringUntil('\n');
        DEBUG_PRINT(s); DEBUG_PRINT("\n")
        return s;
    }
    else
    {
        return "";
    }
}

void RestCall::stop()
{
    wifiClient.stop();
}