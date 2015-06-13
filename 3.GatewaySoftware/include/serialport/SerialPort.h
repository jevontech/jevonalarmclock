/* 
 * File:   SerialPort.h
 * Author: jeroen
 *
 * Created on February 28, 2015, 9:46 AM
 */

#ifndef SERIALPORT_H
#define	SERIALPORT_H

#include "Poco/Thread.h"
#include "Poco/Logger.h"

#include <string>
using namespace std;

#include "RequestHandler.h"
#include "IoService.h"
#include "IoServiceProvider.h"

class SerialPort {
public:
    SerialPort( RequestHandler* requesthandler );

    virtual ~SerialPort();

    bool open(string port);
    void close();
    bool isOpen();
    void startReading();

    void setBaudrate(int baudrate);
    int getBaudrate();

    void sendData(vector<unsigned char>);

    

protected:
   //  bool sendRequestAndExpectReply(string request, string reply);
    // void handler(const boost::system::error_code& err, std::size_t size);
    //virtual void processData();
   


private:

    RequestHandler*  p_requesthandler;

    void write(vector<unsigned char>);
    void write_complete(const boost::system::error_code& error);

    void read();
    void read_complete(const boost::system::error_code& error, size_t bytes_transferred);

    void do_close(const boost::system::error_code& error);
   
    void timeout(const boost::system::error_code& e);

   

    void readData(size_t bytes_transferred);
    
    Poco::Logger& m_logger;
    IoService* mp_asioIoService;
    boost::asio::serial_port* mp_serialPort;
    boost::asio::deadline_timer* mp_timer;

    boost::asio::streambuf* mp_streambuffer;
    std::deque<vector<unsigned char> > m_writeBuffer;
    boost::array<unsigned char, 4 > m_buffer;
    vector<unsigned char> m_readBuffer;

    string m_expectedReply;
    bool m_requestCompleted;
    bool m_replyOK;

   // AutoGlobalContext m_context;
   // AutoRequired<IoServiceProvider> auto_ioserviceprovider;
    shared_ptr<IoServiceProvider> mp_ioserviceprovider;

};

#endif	/* SERIALPORT_H */

