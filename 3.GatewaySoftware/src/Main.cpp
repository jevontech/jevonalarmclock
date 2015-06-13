#include "Poco/LogStream.h"
#include "../include/Main.h"

Main::Main() : m_logger(Poco::Logger::get("main")), m_serialport(this), m_time(), m_hueclient("10.55.55.24:80"), LightID(3), m_buffer() {

    //m_serialport.setBaudrate(9600);
    m_serialport.open("/dev/ttyUSB0");
    m_serialport.startReading();
}

Main::~Main() {
    m_serialport.close();
}

struct HexCharStruct {
    unsigned char c;

    HexCharStruct(unsigned char _c) : c(_c) {
    }
};

inline std::ostream& operator<<(std::ostream& o, const HexCharStruct& hs) {
    return (o << std::hex << (int) hs.c);
}

inline HexCharStruct hex(unsigned char _c) {
    return HexCharStruct(_c);
}

int Main::main(const std::vector<std::string>& args) {

    // m_ntpclient.request();
    //m_hueclient.setWakeUpLight(3,250,10000);

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
    for(int i=0;i<L;i++){
    m_buffer.push(data[i]);
    }

    m_logger.debug("Buffer size=" + to_string(m_buffer.size()));
    
    if(m_buffer.back()== 0xff){
    


    if (m_buffer.front() == 0x01) {
        m_logger.debug("Command: test ");
        m_buffer.pop();
        m_buffer.pop();
        m_buffer.pop();
        
        replymessage.push_back(0x02);
        replymessage.push_back(3);
        replymessage.push_back(0xff);
        // m_serialport.sendData(replymessage);
    }

    if (m_buffer.front() == 0x03) {
        m_logger.debug("Command: time request");
        m_buffer.pop();
        m_buffer.pop();
        m_buffer.pop();

        tm* time = m_time.getTime();
        m_logger.debug("Seconds:" + to_string(time->tm_sec));
        m_logger.debug("Minutes:" + to_string(time->tm_min));
        m_logger.debug("Hours:" + to_string(time->tm_hour));

        replymessage.push_back(0x04);
        replymessage.push_back(15);


        vector<unsigned char> seconds = IntegerToBytes(time->tm_sec);
        vector<unsigned char> minutes = IntegerToBytes(time->tm_min);
        vector<unsigned char> hours = IntegerToBytes(time->tm_hour);

        appendToByteVector(replymessage, seconds);
        appendToByteVector(replymessage, minutes);
        appendToByteVector(replymessage, hours);
        replymessage.push_back(0xff);

        Poco::LogStream ls(m_logger);
        ls << "Reply message" << std::endl;
        for (int i = 0; i < replymessage.size(); i++) {
            ls << hex(replymessage[i]) << std::endl;
        }


    }


    if (m_buffer.front() == 0x05) {
        m_logger.debug("Command: GetLightState");

        m_buffer.pop();
        m_buffer.pop();
        m_buffer.pop();
        
        replymessage.push_back(0x06);
        replymessage.push_back(8);

        struct LightState state=m_hueclient.getLightState(LightID);
        vector<unsigned char> on = {state.on};
        vector<unsigned char> bri = {state.bri};
        vector<unsigned char> sat = {state.sat};
        vector<unsigned char> hue = ShortToBytes(state.hue);

        appendToByteVector(replymessage, on);
        appendToByteVector(replymessage, bri);
        appendToByteVector(replymessage, sat);
        appendToByteVector(replymessage, hue);
        replymessage.push_back(0xff);

        Poco::LogStream ls(m_logger);
        ls << "Reply message" << std::endl;
        for (int i = 0; i < replymessage.size(); i++) {
            ls << hex(replymessage[i]) << std::endl;
        }


    }

    
    
    if (m_buffer.front() == 0x07 && m_buffer.size()==8 ) {
        m_logger.debug("Command: SetLightState");

               
        m_buffer.pop();
        m_buffer.pop();
        //m_buffer.pop();
        
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
           state.hue= B << 8 | A ;
                 m_buffer.pop();
                //       m_buffer.pop();
           
         m_logger.debug(" SetLightState : on=" + to_string(state.on));
         m_logger.debug(" SetLightState : sat=" + to_string(state.sat));
         m_logger.debug(" SetLightState : bri=" + to_string(state.bri));
         //       m_logger.debug(" SetLightState : hue1=" + to_string(data.at(5)));
         //         m_logger.debug(" SetLightState : hue2=" + to_string(data.at(6)));
         m_logger.debug(" SetLightState : hue=" + to_string(state.hue));
        m_hueclient.setLightState(LightID,state);
        
        
        replymessage.push_back(0x08);
         replymessage.push_back(3);
        replymessage.push_back(0xff);

        Poco::LogStream ls(m_logger);
        ls << "Reply message" << std::endl;
        for (int i = 0; i < replymessage.size(); i++) {
            ls << hex(replymessage[i]) << std::endl;
        }


    }
    
    //m_buffer.
     m_serialport.sendData(replymessage);
    }
    
    
   


}

void Main::appendToByteVector(vector<unsigned char>& OriginalVector, vector<unsigned char> AppendVector) {

    for (int i = 0; i < AppendVector.size(); i++) {
        OriginalVector.push_back(AppendVector[i]);
    }

}

vector<unsigned char> Main::IntegerToBytes(int integer) {
    vector<unsigned char> arrayOfByte(4);
    for (int i = 0; i < 4; i++)
        arrayOfByte[i] = (integer >> (i * 8));
    return arrayOfByte;
}

vector<unsigned char> Main::ShortToBytes(short shortinteger) {
    vector<unsigned char> arrayOfByte(2);
    for (int i = 0; i < 2; i++)
        arrayOfByte[i] = (shortinteger >> (i * 8));
    return arrayOfByte;
}

int main(int argc, char** argv) {
    Poco::Logger::root().setLevel("debug");
    Main app;
    return app.run(argc, argv);
}

