/*
 * Bridge: main.cpp
 *
 *  Created on: Mon Dec  2 10:54:09 2013
 *      Author: Johannes Unger
 */
#include <processing/interprocess/BridgeSessionManager.hpp>
#include <processing/interprocess/HostSession.hpp>
#include <processing/FrxAsyncDSPTimer.hpp>
#include <sambag/com/BoostTimer2.hpp>

enum {
    FRX_BRIDGE_AUTOCLOSE_CHECK_INTERVAL = 5000
};
  
void checkIsNeeded(frx::processing::interprocess::HostSession *session_ptr) {
    using namespace frx::processing::interprocess;
    if ( BridgeSessionManager::instance().getNumSessions() == 0 ) {
        session_ptr->stopMainLoop();
    }
}

int main(int argc, char **argv) {
    using namespace frx::processing;
    using namespace frx::processing::interprocess;
    std::string id("VSTForx.Bridge.Mainsession2");
    if (argc>=2) {
        id += std::string(argv[1]);
    }
    typedef sambag::com::BoostTimer2 AutoCloseTimer;
    AutoCloseTimer::WorkerThreadHolder wth = AutoCloseTimer::startWorkerThread();
    
    try {
        SAMBAG_LOG_INFO<<id<<": starting";
        HostSession session(id);
        // start is needed check
        typedef sambag::com::BoostTimer2 AutoCloseTimer;
        AutoCloseTimer::Ptr autoclosetimer = AutoCloseTimer::create(FRX_BRIDGE_AUTOCLOSE_CHECK_INTERVAL);
        autoclosetimer->setNumRepetitions(-1);
        autoclosetimer->addEventListener(
            boost::bind(&checkIsNeeded, &session)
        );
        autoclosetimer->start();
        // start main session
        SAMBAG_LOG_INFO<<id<<": started";
        session.startMainLoop();
    } catch (const std::exception &ex) {
        SAMBAG_LOG_ERR<<id<<": failed, "<<ex.what();
    } catch (...) {
        SAMBAG_LOG_ERR<<id<<": failed, unknown error";
    }
    
    AutoCloseTimer::closeAllTimer();
    return 0;
}