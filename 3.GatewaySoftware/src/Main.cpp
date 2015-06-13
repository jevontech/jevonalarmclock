/**
 * @file   Main.cpp
 * @Author Jeroen Vennegoor op Nijhuis
 * @brief  TBD
 *
 * TBD
 */

#include "Poco/LogStream.h"
#include "../include/Main.h"

Main::Main() : m_logger(Poco::Logger::get("main")), m_serialport(this), m_hueclient("10.55.55.24:80"), LightID(3), m_buffer() {
    m_serialport.open("/dev/ttyUSB0");
    m_serialport.startReading();
}

Main::~Main() {
    m_serialport.close();
}

int Main::main(const std::vector<std::string>& args) {
    
    m_logger.information("Application fully initialized. ");
    m_logger.information("Press CTRL+C to stop this application.");
    waitForTerminationRequest();
}

void Main::handler(std::vector<unsigned char> data) {

    vector<unsigned char> replymessage;
    Poco::LogStream ls(m_logger);
    int L = data.size();
    m_logger.debug("Data received : size=" + to_string(L));
    //put data in a queue
    for (int i = 0; i < L; i++) {
        m_buffer.push(data[i]);
    }
    m_logger.debug("Buffer size=" + to_string(m_buffer.size()));

    if (m_buffer.back() == 0xff) {
        if (m_buffer.front() == 0x01) {
            m_logger.debug("Command: test ");
            m_buffer.pop();
            m_buffer.pop();
            m_buffer.pop();
            replymessage.push_back(0x02);
            replymessage.push_back(3);
            replymessage.push_back(0xff);
        }

        if (m_buffer.front() == 0x03) {
            m_logger.debug("Command: time request");
            m_buffer.pop();
            m_buffer.pop();
            m_buffer.pop();
            std::time_t t = std::time(NULL);
            p_time = std::localtime(&t);
            m_logger.debug("Seconds:" + to_string(p_time->tm_sec));
            m_logger.debug("Minutes:" + to_string(p_time->tm_min));
            m_logger.debug("Hours:" + to_string(p_time->tm_hour));
            replymessage.push_back(0x04);
            replymessage.push_back(15);
            vector<unsigned char> seconds = IntegerToBytes(p_time->tm_sec);
            vector<unsigned char> minutes = IntegerToBytes(p_time->tm_min);
            vector<unsigned char> hours = IntegerToBytes(p_time->tm_hour);
            appendToByteVector(replymessage, seconds);
            appendToByteVector(replymessage, minutes);
            appendToByteVector(replymessage, hours);
            replymessage.push_back(0xff);

        }

        if (m_buffer.front() == 0x05) {
            m_logger.debug("Command: GetLightState");

            m_buffer.pop();
            m_buffer.pop();
            m_buffer.pop();

            replymessage.push_back(0x06);
            replymessage.push_back(8);

            struct LightState state = m_hueclient.getLightState(LightID);
            vector<unsigned char> on = {state.on};
            vector<unsigned char> bri = {state.bri};
            vector<unsigned char> sat = {state.sat};
            vector<unsigned char> hue = ShortToBytes(state.hue);

            appendToByteVector(replymessage, on);
            appendToByteVector(replymessage, bri);
            appendToByteVector(replymessage, sat);
            appendToByteVector(replymessage, hue);
            replymessage.push_back(0xff);
        }

        if (m_buffer.front() == 0x07 && m_buffer.size() == 8) {
            m_logger.debug("Command: SetLightState");

            m_buffer.pop();
            m_buffer.pop();

            struct LightState state;
            state.on = m_buffer.front();
            m_buffer.pop();
            state.sat = m_buffer.front();
            m_buffer.pop();
            state.bri = m_buffer.front();
            m_buffer.pop();
            unsigned char A = m_buffer.front();
            m_buffer.pop();
            unsigned char B = m_buffer.front();
            m_buffer.pop();
            state.hue = B << 8 | A;
            m_buffer.pop();

            //debug printing
            m_logger.debug(" SetLightState : on=" + to_string(state.on));
            m_logger.debug(" SetLightState : sat=" + to_string(state.sat));
            m_logger.debug(" SetLightState : bri=" + to_string(state.bri));
            m_logger.debug(" SetLightState : hue=" + to_string(state.hue));
            m_hueclient.setLightState(LightID, state);

            replymessage.push_back(0x08);
            replymessage.push_back(3);
            replymessage.push_back(0xff);

        }
        m_serialport.sendData(replymessage);
    }


}

void Main::appendToByteVector(vector<unsigned char>& OriginalVector, vector<unsigned char> AppendVector) {
    for (int i = 0; i < AppendVector.size(); i++) {
        OriginalVector.push_back(AppendVector[i]);
    }
}

int main(int argc, char** argv) {
    //check if we need to enable debug logging
    for (int i = 0; i < argc; i++) {
        if (  string(argv[i]).find("debug") != std::string::npos) {
                Poco::Logger::root().setLevel("debug");
        }
    }
    
    Main app;
    return app.run(argc, argv);
}

