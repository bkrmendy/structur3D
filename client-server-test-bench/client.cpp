//
// Created by Berci on 2020. 12. 02..
//

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/strand.hpp>

#include <spdlog/spdlog.h>

#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

//------------------------------------------------------------------------------

void fail(beast::error_code ec, char const* what) {
    spdlog::error(what);
}

// Sends a WebSocket message and prints the response
class session : public std::enable_shared_from_this<session>
{
    tcp::resolver resolver_;
    websocket::stream<beast::tcp_stream> ws_;
    beast::flat_buffer buffer_;
    std::string host_;
    std::string port_;

public:
    session(net::io_context& ioc, const char* host, const char* port)
            : resolver_(net::make_strand(ioc))
            , host_{host}
            , port_{port}
            , ws_(net::make_strand(ioc))
    {
    }

    void connect() {
        spdlog::info("Resolving host...");
        resolver_.async_resolve(
                host_,
                port_,
                beast::bind_front_handler(
                        &session::on_resolve,
                        shared_from_this()));
    }

    void on_resolve(beast::error_code ec, const tcp::resolver::results_type& results) {
        if (ec) {
            return fail(ec, "resolve");
        }

        beast::get_lowest_layer(ws_).expires_after(std::chrono::seconds(30));

        spdlog::info("Connecting to host...");
        beast::get_lowest_layer(ws_).async_connect(
                results,
                beast::bind_front_handler(
                        &session::on_connect,
                        shared_from_this()));
    }

    void on_connect(beast::error_code ec, tcp::resolver::results_type::endpoint_type ep) {
        if (ec) {
            return fail(ec, "connect");
        }

        spdlog::info("Performing handshake with host...");
        // Turn off the timeout on the tcp_stream, because
        // the websocket stream has its own timeout system.
        beast::get_lowest_layer(ws_).expires_never();

        // Set suggested timeout settings for the websocket
        ws_.set_option(
                websocket::stream_base::timeout::suggested(
                        beast::role_type::client));

        // Set a decorator to change the User-Agent of the handshake
        ws_.set_option(websocket::stream_base::decorator(
                [](websocket::request_type& req)
                {
                    req.set(http::field::user_agent,
                            std::string(BOOST_BEAST_VERSION_STRING) +
                            " websocket-client-async");
                }));

        // Update the host_ string. This will provide the value of the
        // Host HTTP header during the WebSocket handshake.
        // See https://tools.ietf.org/html/rfc7230#section-5.4
        host_ += ':' + std::to_string(ep.port());

        // Perform the websocket handshake
        ws_.async_handshake(host_, "/",
                            beast::bind_front_handler(
                                    &session::on_handshake,
                                    shared_from_this()));
    }

    void on_handshake(beast::error_code ec) {
        if (ec) {
            return fail(ec, "handshake");
        }

        spdlog::info("Connected successfully!");
    }

    void send(std::string text) {
        spdlog::info("Sending message...");
        ws_.async_write(
                net::buffer(text),
                beast::bind_front_handler(
                        &session::on_write,
                        shared_from_this()));
    }

    void on_write(beast::error_code ec, std::size_t bytes_transferred) {
        boost::ignore_unused(bytes_transferred);

        if (ec) {
            return fail(ec, "write");
        }

        spdlog::info("Message sent!");

        // Read a message into our buffer
        ws_.async_read(
                buffer_,
                beast::bind_front_handler(
                        &session::on_read,
                        shared_from_this()));
    }

    void on_read(beast::error_code ec, std::size_t bytes_transferred) {
        boost::ignore_unused(bytes_transferred);
        if (ec) {
            return fail(ec, "read");
        }

        spdlog::info("Incoming message!");
        ws_.read(buffer_);
        std::cout << beast::make_printable(buffer_.data()) << std::endl;
    }

    void close() {
        spdlog::info("Closing connection...");
        ws_.async_close(websocket::close_code::normal,
                        beast::bind_front_handler(
                                &session::on_close,
                                shared_from_this()));
    }

    void on_close(beast::error_code ec) {
        if (ec) {
            return fail(ec, "close");
        }

        spdlog::info("Connection closed!");
    }
};

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

int main()
{
    const auto host = "127.0.0.1";
    const auto port = "3000";

    // The io_context is required for all I/O
    net::io_context ioc;

    std::shared_ptr<int> state = std::make_shared<int>(0);
    std::shared_ptr<session> sesh = std::make_shared<session>(ioc, host, port);
    sesh->connect();

    bool running = true;

    while (running) {
        std::string message;
        std::getline(std::cin, message);

        if (!std::cin.good()) { running = false; }
        else {
            sesh->send(message);
        }
    }

    ioc.run();

    sesh->close();
    std::cout << "Goodbye" << std::endl;

    return EXIT_SUCCESS;
}