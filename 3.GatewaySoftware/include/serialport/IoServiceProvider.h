/* 
 * File:   IoServiceProvider.h
 * Author: jeroen
 *
 * Created on February 28, 2015, 10:05 AM
 */

#ifndef IOSERVICEPROVIDER_H
#define	IOSERVICEPROVIDER_H

#include "Poco/Thread.h"
#include "Poco/Logger.h"
#include "boost/asio/io_service.hpp"
#include "IoService.h"

class IoServiceProvider {
public:
    IoServiceProvider();
    
    virtual ~IoServiceProvider();

    IoService* getIoService();

private:
    Poco::Logger& m_logger;
    IoService* asioIoService;
    //boost::asio::io_service io_service;
    boost::asio::io_service::work* work;
    Poco::Thread* asio_thread;

};

#endif	/* IOSERVICEPROVIDER_H */

