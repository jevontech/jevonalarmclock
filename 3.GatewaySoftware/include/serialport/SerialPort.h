/* 
 * @file  SerialPort.h
 * @author Jeroen Vennegoor op Nijhuis
 *
 * 
 */

#ifndef SERIALPORT_H
#define	SERIALPORT_H

#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/asio/serial_port.hpp>

#include "Poco/Thread.h"
#include "Poco/Logger.h"

#include <iostream>
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
    std::array<unsigned char, 4 > m_buffer;
    vector<unsigned char> m_readBuffer;
    string m_expectedReply;
    bool m_requestCompleted;
    bool m_replyOK;
    shared_ptr<IoServiceProvider> mp_ioserviceprovider;

};

#endif	/* SERIALPORT_H */

