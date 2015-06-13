/* 
 * File:   HueClient.h
 * Author: jeroen
 *
 * Created on April 7, 2015, 7:42 PM
 */

#ifndef HTTPCLIENT_H
#define	HTTPCLIENT_H

#include <string>
using namespace std;

#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/Logger.h"

using namespace Poco::Net;

struct LightState{
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
    //int setLight(int id,bool on);
    
private:
        Poco::Logger& m_logger;
    int m_wakeuplightID;
    string m_address;
    
    string HttpGet(string URL);
    string HttpPutJson(string URL, string body);
    
    string const BoolToString(bool b);
    
    struct LightState ParseStateInformation(string );

};

#endif	/* HTTPCLIENT_H */

