/**
 * @file   SerialPort.cpp
 * @Author Jeroen Vennegoor op Nijhuis
 * @brief  TBD
 *
 * TBD
 */

#include <Poco/File.h>
#include <Poco/String.h>
#include <Poco/NumberFormatter.h>
#include "boost/algorithm/string/find.hpp"

#include "../../include/serialport/SerialPort.h"

SerialPort::SerialPort(RequestHandler* requesthandler) : p_requesthandler(requesthandler),
m_logger(Poco::Logger::get("serialport.SerialPort")),
mp_asioIoService(NULL), mp_serialPort(NULL), mp_timer(NULL),
mp_streambuffer(), m_writeBuffer(), m_buffer(), m_readBuffer(), m_expectedReply(), m_requestCompleted(false),
m_replyOK(false), mp_ioserviceprovider(new IoServiceProvider) {
}

SerialPort::~SerialPort() {

    if (mp_serialPort) {
        if (mp_serialPort->is_open()) {
            mp_serialPort->close();
        }
    }

    if (mp_serialPort) {
        delete mp_serialPort;
    }
    if (mp_streambuffer) {
        delete mp_streambuffer;
    }
}

bool SerialPort::open(string port) {

    bool ok = true;
    Poco::File ttyusb(port);
    ok = ttyusb.exists();

    if (ok) {
        m_logger.debug("try to open port " + port);
        mp_streambuffer = new boost::asio::streambuf();
        try {
            m_logger.debug("Get ASIO service");
            mp_asioIoService = mp_ioserviceprovider->getIoService();
            mp_serialPort = new boost::asio::serial_port(*mp_asioIoService);
            mp_timer = new boost::asio::deadline_timer(*mp_asioIoService);
            mp_serialPort->open(port);
            ok = isOpen();
        } catch (std::exception& e) {
            std::ostringstream os1;
            os1 << "Get ASIO service: Error: " << e.what() << " ";
            std::string S1(os1.str());
            m_logger.critical(S1);
        }
    }


    return ok;
}

void SerialPort::close() {
    mp_serialPort->close();
}

bool SerialPort::isOpen() {
    return mp_serialPort->is_open();
}

void SerialPort::startReading() {
    read();
}

void SerialPort::setBaudrate(int baudrate) {
    boost::asio::serial_port_base::baud_rate baud_option(baudrate);
    try {
        m_logger.debug("setBaudrate");
        mp_serialPort->set_option(baud_option);
    } catch (std::exception& e) {
        std::ostringstream os1;
        os1 << "setBaudrate: Error: " << e.what() << " ";
        std::string S1(os1.str());
        m_logger.critical(S1);
    }
}

int SerialPort::getBaudrate() {

    try {
        boost::asio::serial_port_base::baud_rate baudrate;
        mp_serialPort->get_option(baudrate);
        return baudrate.value();
    } catch (std::exception& e) {
        std::ostringstream os1;
        os1 << "getBaudrate: Error: " << e.what() << " ";
        std::string S1(os1.str());
        m_logger.critical(S1);
    }
}

void SerialPort::sendData(vector<unsigned char> data) {
    // m_logger.debug("sendData:" + data);
    if (isOpen()) {

        mp_asioIoService->post(boost::bind(&SerialPort::write, this, data));
    } else {
        m_logger.debug("sendData: Error, port closed");
    }

}

void SerialPort::write(vector<unsigned char> data) {
    int n = 0;
    n = m_writeBuffer.size();

    bool write_in_progress = !m_writeBuffer.empty();
    while (write_in_progress) {
        boost::this_thread::sleep(boost::posix_time::milliseconds(50));
    }
    m_writeBuffer.push_back(data);
    if (!write_in_progress) {
        boost::asio::async_write(*mp_serialPort,
                boost::asio::buffer(m_writeBuffer.front(), m_writeBuffer.front().size()),
                boost::bind(&SerialPort::write_complete,
                this,
                boost::asio::placeholders::error));
    }
}

void SerialPort::write_complete(const boost::system::error_code& error) {
    if (!error) {
        m_writeBuffer.pop_front();
        if (!m_writeBuffer.empty()) {
            boost::asio::async_write(*mp_serialPort,
                    boost::asio::buffer(m_writeBuffer.front(), m_writeBuffer.front().size()),
                    boost::bind(&SerialPort::write_complete,
                    this,
                    boost::asio::placeholders::error));
        }
    } else {
        m_logger.critical("error");
    }
}

void SerialPort::read() {
    mp_serialPort->async_read_some(boost::asio::buffer(m_buffer),
            boost::bind(&SerialPort::read_complete,
            this,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
}

void SerialPort::read_complete(const boost::system::error_code& error, size_t bytes_transferred) {
    if (!error) { // read completed, so process the data
        readData(bytes_transferred);
        read(); // start waiting for another asynchronous read again
    } else
        do_close(error);
}

void SerialPort::readData(size_t bytes_transferred) {
    for (int i = 0; i < bytes_transferred; i++) {
        m_readBuffer.push_back(m_buffer[i]);
    }

    unsigned char lastchar = m_readBuffer.at(m_readBuffer.size() - 1);
    //std::cout << "last char" <<  std::hex << lastchar << std::endl ;
    //std::cout << "FF char" <<  std::hex << 0xFF << std::endl ;

    if (lastchar == 0xFF) {
        //m_logger.debug("last char is FF");
        p_requesthandler->handler(m_readBuffer);
        m_readBuffer.clear();
    }
}

void SerialPort::do_close(const boost::system::error_code& error) {
    if (error == boost::asio::error::operation_aborted)
        return;
    if (error) {
        m_logger.critical("Error: " + error.message());
    } else {
        m_logger.critical("Error: Connection did not succeed.\n");
    }
    mp_serialPort->close();

}

//bool SerialPort::sendRequestAndExpectReply(string request, string reply) {
//    m_replyOK=false;
//    m_expectedReply=reply;
//    char lastchar = reply.at(reply.length()-1); 
//    
//    boost::system::error_code ignored_error;
//    boost::asio::write(*mp_serialPort, boost::asio::buffer(request),
//            boost::asio::transfer_all(), ignored_error);
//    m_logger.debug("Read until end character found");
//    boost::asio::async_read_until(*mp_serialPort, *mp_streambuffer, lastchar,
//            boost::bind(&SerialPort::handler, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
//
//    mp_timer->expires_from_now(boost::posix_time::milliseconds(3000));
//    mp_timer->async_wait(boost::bind(&SerialPort::timeout, this, _1));
//
//    while (!m_requestCompleted) {
//        boost::this_thread::sleep(boost::posix_time::milliseconds(50));
//    }
//    if (m_replyOK) {
//        m_logger.debug("This is the correct serial port: device connected");
//    } 
//
//    return m_replyOK;
//}

void SerialPort::timeout(const boost::system::error_code& e) {
    if (e == boost::asio::error::operation_aborted) {
    } else {
        std::istream is(mp_streambuffer);
        std::string B;
        is >> B;
        m_logger.debug("Timeout: Reply received : " + B);
        mp_serialPort->cancel();
        if (mp_streambuffer) {
            delete mp_streambuffer;
        }
        m_requestCompleted = true;
    }
}

//void SerialPort::handler(const boost::system::error_code& err, std::size_t size) {
//    if (err) {
//        m_logger.error("Error: " + err.message());
//    } else {
//        std::istream is(mp_streambuffer);
//        std::string B;
//        is >> B;
//        m_logger.debug("Reply received : " + B);
//        if (boost::algorithm::find_first(B, m_expectedReply)) {
//            mp_timer->cancel();
//            m_replyOK = true;
//        } else {
//            mp_timer->cancel();
//            mp_serialPort->cancel();
//            m_replyOK = false;
//            if (mp_streambuffer) {
//                delete mp_streambuffer;
//            }
//        }
//        m_requestCompleted = true;
//    }
//}



