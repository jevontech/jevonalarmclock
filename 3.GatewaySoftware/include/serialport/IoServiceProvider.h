/* 
 * @file  IoServiceProvider.h
 * @author Jeroen Vennegoor op Nijhuis
 *
 * 
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
    boost::asio::io_service::work* work;
    Poco::Thread* asio_thread;

};

#endif	/* IOSERVICEPROVIDER_H */

