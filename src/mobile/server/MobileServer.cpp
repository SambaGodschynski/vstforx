/*
 * MobileServer.cpp
 *
 *  Created on: Sun Feb  9 16:27:43 2014
 *      Author: Johannes Unger
 */

#include "MobileServer.hpp"
#include <boost/xpressive/xpressive.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <sambag/com/Common.hpp>

namespace frx { namespace mobile {
namespace {

void parseRequest(const std::string &req, MobileServer::Request &out) {
    using namespace boost::xpressive;
    //GET /index.main?v=1 HTTP/1.1
    mark_tag trq(1), tGet(2);
    sregex r ="GET">>*_s>>"/">>(trq=-*_)>>repeat<0,1>("?">>(tGet=-*_))>>
        *_s>>"HTTP/">>*digit>>".">>*digit;
    smatch what;
    if( regex_search( req, what, r ) ) {
        out["TARGET"] = what[trq];
        if (what.size()>=2) {
            const std::string &get = out["GET"] = what[tGet];
            mark_tag k(1), v(2);
            sregex r=(bos|"&")>>(k=*set[alnum | '%'])>>"=">>(v=*set[alnum | '%']);
            sregex_iterator cur( get.begin(), get.end(), r );
            sregex_iterator end;
            for(; cur!=end; ++cur) {
                smatch what = *cur;
                out[what[k]] = what[v];
            }
        }
    }
}


}
///////////////////////////////////////////////////////////////////////////////
class MobileServer::TcpConnection
  : public boost::enable_shared_from_this<TcpConnection>
{
public:
    //-------------------------------------------------------------------------
    typedef boost::shared_ptr<TcpConnection> Ptr;
    //-------------------------------------------------------------------------
    static Ptr create(boost::asio::io_service& io_service,
        const MobileServer::RequestHandlers &handlers) {
        return Ptr(new TcpConnection(io_service, handlers));
    }
    //-------------------------------------------------------------------------
    bai::tcp::socket& socket() {
        return socket_;
    }
    //-------------------------------------------------------------------------
    void start() {
        try {
            size_t s = socket_.receive(boost::asio::buffer(request_));
            std::string srq(request_.data(), s);
            Request rq;
            parseRequest(srq, rq);
            Response rp;
            const std::string &target = rq["TARGET"];
            SAMBAG_LOG_INFO<<socket_.local_endpoint()<<" requests:"<<target;
            MobileServer::RequestHandlers::const_iterator it = handlers.find(target);
            if (it==handlers.end()) {
                rp<<target<<" Not Found";
            } else {
                it->second(rq, rp);
            }
            responseStr = rp.str();
            boost::asio::async_write(socket_, boost::asio::buffer(responseStr),
                boost::bind(&TcpConnection::handleWrite, shared_from_this(),
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred)
            );
        } catch (const std::exception &ex) {
            SAMBAG_LOG_ERR<<"exception: "<<ex.what();
        } catch (...) {}
    }
private:
    //-------------------------------------------------------------------------
    const MobileServer::RequestHandlers &handlers;
    //-------------------------------------------------------------------------
    TcpConnection(boost::asio::io_service& io_service,
        const MobileServer::RequestHandlers &handlers
    ) : handlers(handlers), socket_(io_service) {}
    //-------------------------------------------------------------------------
    void handleWrite(const boost::system::error_code&, size_t bytes) {}
    //-------------------------------------------------------------------------
    bai::tcp::socket socket_;
    std::string responseStr;
    boost::array<char, 4096> request_;
};


//=============================================================================
//  Class MobileServer
//=============================================================================
//-----------------------------------------------------------------------------
MobileServer::MobileServer(boost::asio::io_service& io_service, int port)
    : acceptor_(io_service, bai::tcp::endpoint(bai::tcp::v4(), port))
{
    SAMBAG_LOG_INFO<<"server listen to:"<<acceptor_.local_endpoint();
    startAccept();
}
//-----------------------------------------------------------------------------
void MobileServer::startAccept() {
    TcpConnection::Ptr nc =
        TcpConnection::create(acceptor_.get_io_service(), handlers);
    
    acceptor_.async_accept(nc->socket(),
        boost::bind(&MobileServer::handleAccept, this, nc,
        boost::asio::placeholders::error)
    );
}
//-----------------------------------------------------------------------------
void MobileServer::handleAccept(TcpConnectionPtr cn, const ErrorCode &error) {
    if (!error) {
        cn->start();
    }
    startAccept();
}
//-----------------------------------------------------------------------------
void MobileServer::registerHandler(const RequestTarget &t, const RequestHandler &h)
{
    handlers[t] = h;
}

}} // namespace(s)
