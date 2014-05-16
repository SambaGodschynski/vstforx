

#include "MobileServer.hpp"
#include <boost/asio.hpp>
#include <fstream>
#include <exception>
#include <iostream>
namespace frx { namespace mobile {


void getFile(MobileServer::Request &rq, MobileServer::Response &rp) {
    const std::string &target = rq["TARGET"];
    std::fstream fs(target.c_str(), std::ios_base::in);
    if (fs.fail()) {
        throw std::runtime_error("file: '" + target + "' not found.");
    }
    std::copy(std::istreambuf_iterator<char>(fs),
        std::istreambuf_iterator<char>(),
        std::ostreambuf_iterator<char>(rp)
    );
    fs.close();
}

void hello(MobileServer::Request &rq, MobileServer::Response &rp) {
    rp<<"{\"VSTForx_1\": {\"param1\":\"0\", \"param2\":\"0.2\", \"param3\":\"0.2\"},";
    rp<<"\"VSTForx_2\": {\"param1\":\"0\", \"param2\":\"0.2\", \"param3\":\"0.2\"}}";
}


}} // namespace(s)

int main() {
    try {
        boost::asio::io_service io_service;
        frx::mobile::MobileServer server(io_service);
        server.registerHandler("main/hello", &frx::mobile::getFile);
        io_service.run();
    }
    catch (std::exception& e) {
        std::cout << e.what() << std::endl;
    }
    return 0;
}
