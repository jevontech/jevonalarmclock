/* 
 * File:   Main.h
 * Author: jeroen
 *
 * Created on March 12, 2015, 8:16 PM
 */

#ifndef MAIN_H
#define	MAIN_H
#include "Poco/Util/ServerApplication.h"
#include "Poco/Process.h"
#include "Poco/Logger.h"
#include <string>
#include <queue>
using namespace std;

#include "serialport/RequestHandler.h"
#include "serialport/SerialPort.h"
#include "time/Time.h"
#include "http/HueClient.h"

class Main : public Poco::Util::ServerApplication, public RequestHandler {
public:
    Main();
    virtual ~Main();
    int main(const std::vector<std::string>& args);
    void handler(std::vector<unsigned char> data);
    vector<unsigned char> IntegerToBytes(int integer);
    vector<unsigned char> ShortToBytes(short shortinteger);
    void appendToByteVector(vector<unsigned char>& OriginalVector, vector<unsigned char> AppendVector);

private:
    Poco::Logger& m_logger;
    SerialPort m_serialport;
    Time m_time;
    HueClient m_hueclient;
    int LightID;
    queue<unsigned char> m_buffer;
    

};

#endif	/* MAIN_H */

