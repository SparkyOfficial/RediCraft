/*
 * session.h
 * author: Андрій Будильников
 */

#ifndef REDICRAFT_SESSION_H
#define REDICRAFT_SESSION_H

#include <array>
#include <memory>
#include <string>

#ifdef ASIO_STANDALONE
#include <asio.hpp>
#else
#include <asio.hpp>
#endif

class Storage;

class Session : public std::enable_shared_from_this<Session> {
public:
    Session(asio::ip::tcp::socket socket, Storage& storage);
    void start();
    
private:
    void do_read();
    void do_write();
    void handle_command(const std::string& command);
    
    asio::ip::tcp::socket socket_;
    Storage& storage_;
    std::array<char, 1024> data_;
    std::string response_;
    asio::strand<asio::any_io_executor> strand_;
};

#endif // REDICRAFT_SESSION_H