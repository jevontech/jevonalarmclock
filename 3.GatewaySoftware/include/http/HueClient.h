/* 
 * @file  HueClient.h
 * @author Jeroen Vennegoor op Nijhuis
 *
 */

#ifndef HTTPCLIENT_H
#define	HTTPCLIENT_H

#include <string>
#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/Logger.h"


using namespace std;
using namespace Poco::Net;

struct LightState {
    unsigned char on;
    unsigned char sat;
    unsigned char bri;
    unsigned short hue;
};

class HueClient {
public:
    HueClient(string address);
    virtual ~HueClient();
    void setLightState(int id, struct LightState state);
    struct LightState getLightState(int id);

private:
    Poco::Logger& m_logger;
    int m_wakeuplightID;
    string m_address;
    string HttpGet(string URL);
    string HttpPutJson(string URL, string body);
    struct LightState ParseStateInformation(string);
};

#endif	/* HTTPCLIENT_H */

