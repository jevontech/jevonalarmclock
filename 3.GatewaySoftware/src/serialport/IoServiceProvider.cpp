/**
 * @file   IoServiceProvider.cpp
 * @Author Jeroen Vennegoor op Nijhuis
 * @brief  IoServiceProvider
 *
 * TBD
 */

#include "../../include/serialport/IoServiceProvider.h"

IoServiceProvider::IoServiceProvider() : m_logger(Poco::Logger::get("serialport.IoServiceProvider")) {
    try {
        asioIoService = new IoService();
        work = new boost::asio::io_service::work(*asioIoService);
        asio_thread = new Poco::Thread();
        m_logger.debug("Start thread for ASIO service");
        asio_thread->start(*asioIoService);

    } catch (std::exception& e) {
        std::ostringstream os1;
        os1 << "start IoService: error" << e.what() << " ";
        std::string S1(os1.str());
        m_logger.critical(S1);
    }

}

IoServiceProvider::~IoServiceProvider() {
    asioIoService->stop();
    asio_thread->join();
}

IoService* IoServiceProvider::getIoService() {
    return asioIoService;
}