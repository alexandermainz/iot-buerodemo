#include <Arduino.h>
#include <WiFi.h>
//#include <WiFiClientSecure.h>

class RestCall
{
public:
    RestCall(const char* host, int port);

    int get(const char *path);
    int post(const char *path, const char *body);
    int request(const char *method, const char *path, const char *body);
    //void setHeader(const char *header);
    void setContentType(const char *contentTypeValue);
    String readNextLine();
    void stop();

private:
    int port;
    const char *host;
    const char *contentType;
    WiFiClient wifiClient;
    int readStatus();
};