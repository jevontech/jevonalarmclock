/* 
 * @file  Main.h
 * @author Jeroen Vennegoor op Nijhuis
 *
 * 
 */

#ifndef MAIN_H
#define	MAIN_H
#include "Poco/Util/ServerApplication.h"
#include "Poco/Process.h"
#include "Poco/Logger.h"
#include "Poco/File.h"
#include "Poco/AutoPtr.h"
#include "Poco/Util/XMLConfiguration.h"
#include <string>
#include <queue>
using Poco::File;
using Poco::AutoPtr;
using Poco::Util::XMLConfiguration;
using namespace std;

#include "util/Util.h"
#include "serialport/RequestHandler.h"
#include "serialport/SerialPort.h"
#include "http/HueClient.h"



class Main : public Poco::Util::ServerApplication, public RequestHandler {
public:
    Main();
    virtual ~Main();
    int main(const std::vector<std::string>& args);
    void handler(std::vector<unsigned char> data);
    void appendToByteVector(vector<unsigned char>& OriginalVector, vector<unsigned char> AppendVector);
private:
    Poco::Logger& m_logger;
    string m_configfile;
    AutoPtr<XMLConfiguration> p_conf;
    SerialPort m_serialport;
    HueClient m_hueclient;
    int LightID;
    queue<unsigned char> m_buffer;
    tm* p_time;
};

#endif	/* MAIN_H */

