/* 
 * File:   HueClient.cpp
 * Author: jeroen
 * 
 * Created on April 7, 2015, 7:42 PM
 */

#include "boost/lexical_cast.hpp"

#include "Poco/URI.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/SocketAddress.h"
#include "Poco/Net/StreamSocket.h"
#include "Poco/LogStream.h"
#include "Poco/NumberFormatter.h"
#include "Poco/NumberParser.h"
#include "Poco/StringTokenizer.h"
#include "../../include/http/HueClient.h"
//#include "../../include/http/HueClient.h"

#include <curl/curl.h>

HueClient::HueClient(string address) : m_logger(Poco::Logger::get("HueClient")), m_address(address) {

}

HueClient::~HueClient() {
}

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    ((std::string*)userp)->append((char*) contents, size * nmemb);
    return size * nmemb;
}

string const HueClient::BoolToString(bool b) {
    return b ? "true" : "false";
}

struct LightState HueClient::ParseStateInformation(string json) {
    struct LightState state;

        m_logger.debug("HueClient: ParseStateInformation");
    
    Poco::StringTokenizer t1(json, ",");
    for (Poco::StringTokenizer::Iterator it = t1.begin(); it != t1.end(); ++it) {
        string S = *it;
        m_logger.debug("token="+S);

        //handle the "on" parameter
        std::size_t found = S.find("\"on\"");
        if (found != std::string::npos) {
            //check if on or off
            found = S.find("true");
            if (found != std::string::npos) {
                state.on = 1;
            } else {
                state.on = 0;
            }
            string s = " on=";
            Poco::NumberFormatter::append(s,state.on);
            m_logger.debug(s);
        }

        found = S.find("\"sat\"");
        if (found != std::string::npos) {
            Poco::StringTokenizer t2(S, ":");
            int sat = Poco::NumberParser::parse(t2[1]);
            state.sat = static_cast<char>(sat);
            
            string s = " sat=";
            Poco::NumberFormatter::append(s,state.sat);
            m_logger.debug(s);
        }

        found = S.find("\"bri\"");
        if (found != std::string::npos) {
            Poco::StringTokenizer t2(S, ":");
            int bri = Poco::NumberParser::parse(t2[1]);
            state.bri = static_cast<char>(bri);
            
            string s = " bri=";
            Poco::NumberFormatter::append(s,state.bri);
            m_logger.debug(s);
        }

        found = S.find("\"hue\"");
        if (found != std::string::npos) {
            Poco::StringTokenizer t2(S, ":");
            int hue = Poco::NumberParser::parse(t2[1]);
            state.hue = static_cast<short>(hue);
            
            
                string s = " hue=";
            Poco::NumberFormatter::append(s,state.hue);
            m_logger.debug(s);
        }



    }


    return state;
}

struct LightState HueClient::getLightState(int id) {
    m_logger.debug("HueClient: getLightState");
    string url = m_address + "/api/alarmclock/lights/" + to_string(id) + "";

    string output = HttpGet(url);

    struct LightState state = ParseStateInformation(output);

    return state;

}

void HueClient::setLightState(int id, struct LightState state) {

    m_logger.debug("HueClient: setLightState");
    string url = m_address + "/api/alarmclock/lights/" + to_string(id) + "/state";

    bool on = false;
    if (state.on == 1) {
        on = true;
    }

    string json = "{\"on\":" + BoolToString(on) + " , \"sat\":"+ Poco::NumberFormatter::format(state.sat) +
            ", \"bri\":"+Poco::NumberFormatter::format(state.bri)+
            ",\"hue\":"+Poco::NumberFormatter::format(state.hue)+"}";

    string output = HttpPutJson(url, json);

    m_logger.debug("HueClient: output=" + output);

}

string HueClient::HttpGet(string URL) {
    CURL *curl;
    CURLcode res;
    std::string readBuffer;

    curl = curl_easy_init();

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, URL.c_str());
        /* example.com is redirected, so we tell libcurl to follow redirection */
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        /* Perform the request, res will get the return code */
        res = curl_easy_perform(curl);
        /* Check for errors */
        if (res != CURLE_OK) {
            m_logger.error("CURL error" + string(curl_easy_strerror(res)));
        }

        curl_easy_cleanup(curl);


    }

    return readBuffer;
}

string HueClient::HttpPutJson(string URL, string body) {

    CURL *curl;
    CURLcode res;
    struct curl_slist *headers = NULL;
    std::string readBuffer;

    curl = curl_easy_init();

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, URL.c_str());
        /* example.com is redirected, so we tell libcurl to follow redirection */
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        //headers = curl_slist_append(headers, client_id_header);
        headers = curl_slist_append(headers, "Content-Type: application/json");

        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        //curl_easy_setopt(curl, CURLOPT_URL, request_url);
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT"); /* !!! */

        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str()); /* data goes here */


        /* Perform the request, res will get the return code */
        res = curl_easy_perform(curl);
        /* Check for errors */
        if (res != CURLE_OK) {
            m_logger.error("CURL error" + string(curl_easy_strerror(res)));
        }

        curl_easy_cleanup(curl);

        //m_logger.debug("data" + readBuffer);

    }

    return readBuffer;

}
