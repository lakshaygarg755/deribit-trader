#ifndef WEBSOCKET_CLIENT_HPP
#define WEBSOCKET_CLIENT_HPP

#include "example/common/root_certificates.hpp"
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl.hpp>
#include <nlohmann/json.hpp>
#include <iostream>
#include <chrono>
#include <thread>
#include <cstdlib>
#include <unistd.h>
#include <fstream>

namespace beast = boost::beast;
namespace websocket = beast::websocket;
namespace net = boost::asio;
namespace ssl = boost::asio::ssl;
namespace http = beast::http;
using tcp = boost::asio::ip::tcp;
using json = nlohmann::json;

json buffertojson(beast::flat_buffer &input){
    return json::parse(beast::buffers_to_string(input.data()));
}
auto start = std::chrono::high_resolution_clock::now();
auto end = std::chrono::high_resolution_clock::now();

class WebSocketClient {
private:
    net::io_context ioc;
    ssl::context ctx{ssl::context::tlsv12_client};
    tcp::resolver resolver{ioc};
    websocket::stream<ssl::stream<tcp::socket>> ws{ioc, ctx};
    beast::flat_buffer buffer;
    std::string host = "test.deribit.com";
    std::string port = "443";

public:
    WebSocketClient(){
        ctx.set_verify_mode(ssl::verify_peer);
        load_root_certificates(ctx);
        auto const results = resolver.resolve(host, port);
        auto ep = net::connect(beast::get_lowest_layer(ws), results);
        if(! SSL_set_tlsext_host_name(ws.next_layer().native_handle(), host.c_str()))
        {
            throw beast::system_error(
                static_cast<int>(::ERR_get_error()),
                net::error::get_ssl_category());
        }
        ws.next_layer().set_verify_callback(ssl::host_name_verification(host));
        host += ':' + std::to_string(ep.port());
        ws.next_layer().handshake(ssl::stream_base::client);
        ws.set_option(websocket::stream_base::decorator(
            [](websocket::request_type& req)
            {
                req.set(http::field::user_agent,
                    std::string(BOOST_BEAST_VERSION_STRING) +
                        " websocket-client-coro");
            }));
        ws.handshake(host, "/ws/api/v2");
    }
    
    json send(const json &message) {
        if (!ws.is_open()) {
            std::cerr << "WebSocket connection is closed!" << std::endl;
            return json();
        }
        auto start = std::chrono::high_resolution_clock::now();
    
        ws.write(net::buffer(message.dump()));
        buffer.consume(buffer.size());
        ws.read(buffer);
    
        auto end = std::chrono::high_resolution_clock::now();
    
        std::cout << "Latency: " 
                  << std::chrono::duration<double, std::micro>(end - start).count() 
                  << " µs\n";
    
        return buffertojson(buffer);
    }

    void subscribe(const json &message){
        ws.write(net::buffer(message.dump()));
        while(true){
            buffer.consume(buffer.size());
            ws.read(buffer);
            std::cout << buffertojson(buffer).dump(4);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

    }

    void close() {
        beast::error_code ec;
        ws.close(websocket::close_code::normal, ec);
        if (ec) {
            std::cerr << "WebSocket close error: " << ec.message() << std::endl;
        }
    }

    bool isopen() {
        return ws.is_open();
    }
    
};

#endif