/* 
 * @file  IoService.h
 * @author Jeroen Vennegoor op Nijhuis
 *
 * 
 */

#ifndef IOSERVICE_H
#define	IOSERVICE_H

#include <boost/asio.hpp>
#include "Poco/Runnable.h"
  
class IoService: public boost::asio::io_service , public Poco::Runnable{
public:
    void run(){
        boost::asio::io_service::run();
    }
};

#endif	/* IOSERVICE_H */