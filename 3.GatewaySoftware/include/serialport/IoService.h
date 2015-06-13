/* 
 * @file  IoService.h
 * @author Jeroen Vennegoor op Nijhuis
 *
 * 
 */


#ifndef IOSERVICE_H
#define	IOSERVICE_H

#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/asio/serial_port.hpp>
#include <boost/thread.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include "Poco/Runnable.h"
  
class IoService: public boost::asio::io_service , public Poco::Runnable{
public:
    void run(){
        boost::asio::io_service::run();
    }
};

#endif	/* IOSERVICE_H */