/*
 * MobileServer.hpp
 *
 *  Created on: Sun Feb  9 16:27:43 2014
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_MOBILESERVER_H
#define SAMBAG_MOBILESERVER_H

#include <boost/asio.hpp>
#include <memory>
#include <string>
#include <unordered_map>
#include <functional>
#include <boost/ref.hpp>
#include <sstream>

namespace frx { namespace mobile {

namespace bai = boost::asio::ip;

//=============================================================================
/** 
  * @class MobileServer.
  */
class MobileServer {
//=============================================================================
public:
    //-------------------------------------------------------------------------
    class TcpConnection;
    typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
    //-------------------------------------------------------------------------
    typedef boost::system::error_code ErrorCode;
    //-------------------------------------------------------------------------
    typedef std::string RequestKey;
    typedef std::string RequestValue;
    typedef std::unordered_map<RequestKey, RequestValue> Request;
    //-------------------------------------------------------------------------
    typedef std::string RequestTarget;
    typedef std::stringstream Response;
    typedef std::function< void(Request&, Response&) > RequestHandler;
    typedef std::unordered_map<RequestTarget, RequestHandler> RequestHandlers;
protected:
    //-------------------------------------------------------------------------
    typedef bai::tcp::acceptor Acceptor;
    Acceptor acceptor_;
    RequestHandlers handlers;
private:
    //-------------------------------------------------------------------------
    void startAccept();
    //-------------------------------------------------------------------------
    void handleAccept(TcpConnectionPtr connection, const ErrorCode& error);
public:
    //-------------------------------------------------------------------------
    MobileServer(boost::asio::io_service& io_service, int port=8080);
    //-------------------------------------------------------------------------
    void registerHandler(const RequestTarget &t, const RequestHandler &h);
}; // MobileServer
}} // namespace(s)

#endif /* SAMBAG_MOBILESERVER_H */
