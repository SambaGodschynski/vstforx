

#include "MobileServer.hpp"
#include <boost/asio.hpp>


namespace frx { namespace mobile {

void hello(MobileServer::Request &rq, MobileServer::Response &rp) {
    rp<<"{\"VSTForx_1\": {\"param1\":\"0\", \"param2\":\"0.2\", \"param3\":\"0.2\"},";
    rp<<"\"VSTForx_2\": {\"param1\":\"0\", \"param2\":\"0.2\", \"param3\":\"0.2\"}}";
}


}} // namespace(s)

int main() {
    try {
        boost::asio::io_service io_service;
        frx::mobile::MobileServer server(io_service);
        server.registerHandler("main/hello", &frx::mobile::hello);
        io_service.run();
    }
    catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}
