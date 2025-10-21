/*
 * session.cpp
 * author: Андрій Будильников
 */

#include "session.h"
#include "storage.h"
#include "parser.h"
#include <iostream>
#include <sstream>

using asio::ip::tcp;

Session::Session(tcp::socket socket, Storage& storage)
    : socket_(std::move(socket)), storage_(storage), strand_(asio::make_strand(socket_.get_executor())) {
}

void Session::start() {
    do_read();
}

void Session::do_read() {
    auto self(shared_from_this());
    socket_.async_read_some(asio::buffer(data_, 1024),
        asio::bind_executor(strand_,
            [this, self](std::error_code ec, std::size_t length) {
                if (!ec) {
                    std::string command(data_.data(), length);
                    // Remove any trailing newlines or carriage returns
                    command.erase(std::remove(command.begin(), command.end(), '\n'), command.end());
                    command.erase(std::remove(command.begin(), command.end(), '\r'), command.end());
                    
                    handle_command(command);
                    do_write();
                }
            }));
}

void Session::do_write() {
    auto self(shared_from_this());
    asio::async_write(socket_, asio::buffer(response_),
        asio::bind_executor(strand_,
            [this, self](std::error_code ec, std::size_t /*length*/) {
                if (!ec) {
                    response_.clear();
                    do_read();
                }
            }));
}

void Session::handle_command(const std::string& commandStr) {
    Command cmd = Parser::parse(commandStr);
    
    switch (cmd.type) {
        case CommandType::PING:
            response_ = "PONG\r\n";
            break;
            
        case CommandType::SET:
            if (cmd.args.size() >= 2) {
                storage_.set(cmd.args[0], cmd.args[1]);
                response_ = "OK\r\n";
            } else {
                response_ = "ERROR: SET requires key and value\r\n";
            }
            break;
            
        case CommandType::GET:
            if (cmd.args.size() >= 1) {
                std::string value;
                if (storage_.get(cmd.args[0], value)) {
                    response_ = value + "\r\n";
                } else {
                    response_ = "(nil)\r\n";
                }
            } else {
                response_ = "ERROR: GET requires key\r\n";
            }
            break;
            
        case CommandType::INCR:
            if (cmd.args.size() >= 1) {
                long long value = storage_.incr(cmd.args[0]);
                response_ = std::to_string(value) + "\r\n";
            } else {
                response_ = "ERROR: INCR requires key\r\n";
            }
            break;
            
        case CommandType::DECR:
            if (cmd.args.size() >= 1) {
                long long value = storage_.decr(cmd.args[0]);
                response_ = std::to_string(value) + "\r\n";
            } else {
                response_ = "ERROR: DECR requires key\r\n";
            }
            break;
            
        case CommandType::INCRBY:
            if (cmd.args.size() >= 2) {
                try {
                    long long increment = std::stoll(cmd.args[1]);
                    long long value = storage_.incrby(cmd.args[0], increment);
                    response_ = std::to_string(value) + "\r\n";
                } catch (const std::exception&) {
                    response_ = "ERROR: Invalid increment value\r\n";
                }
            } else {
                response_ = "ERROR: INCRBY requires key and increment\r\n";
            }
            break;
            
        case CommandType::HSET:
            if (cmd.args.size() >= 3) {
                storage_.hset(cmd.args[0], cmd.args[1], cmd.args[2]);
                response_ = "OK\r\n";
            } else {
                response_ = "ERROR: HSET requires hash key, field, and value\r\n";
            }
            break;
            
        case CommandType::HGET:
            if (cmd.args.size() >= 2) {
                std::string value;
                if (storage_.hget(cmd.args[0], cmd.args[1], value)) {
                    response_ = value + "\r\n";
                } else {
                    response_ = "(nil)\r\n";
                }
            } else {
                response_ = "ERROR: HGET requires hash key and field\r\n";
            }
            break;
            
        case CommandType::HGETALL:
            if (cmd.args.size() >= 1) {
                auto fields = storage_.hgetall(cmd.args[0]);
                if (fields.empty()) {
                    response_ = "(empty hash)\r\n";
                } else {
                    std::ostringstream oss;
                    for (const auto& pair : fields) {
                        oss << pair.first << ": " << pair.second << "\r\n";
                    }
                    response_ = oss.str();
                }
            } else {
                response_ = "ERROR: HGETALL requires hash key\r\n";
            }
            break;
            
        case CommandType::LPUSH:
            if (cmd.args.size() >= 2) {
                std::vector<std::string> values(cmd.args.begin() + 1, cmd.args.end());
                long long new_length = storage_.lpush(cmd.args[0], values);
                response_ = std::to_string(new_length) + "\r\n";
            } else {
                response_ = "ERROR: LPUSH requires list key and at least one value\r\n";
            }
            break;
            
        case CommandType::RPOP:
            if (cmd.args.size() >= 1) {
                std::string value;
                if (storage_.rpop(cmd.args[0], value)) {
                    response_ = value + "\r\n";
                } else {
                    response_ = "(nil)\r\n";
                }
            } else {
                response_ = "ERROR: RPOP requires list key\r\n";
            }
            break;
            
        case CommandType::LRANGE:
            if (cmd.args.size() >= 3) {
                try {
                    long long start = std::stoll(cmd.args[1]);
                    long long end = std::stoll(cmd.args[2]);
                    auto values = storage_.lrange(cmd.args[0], start, end);
                    
                    if (values.empty()) {
                        response_ = "(empty list)\r\n";
                    } else {
                        std::ostringstream oss;
                        for (size_t i = 0; i < values.size(); ++i) {
                            oss << i << ") " << values[i] << "\r\n";
                        }
                        response_ = oss.str();
                    }
                } catch (const std::exception&) {
                    response_ = "ERROR: Invalid range values\r\n";
                }
            } else {
                response_ = "ERROR: LRANGE requires list key, start index, and end index\r\n";
            }
            break;
            
        case CommandType::SADD:
            if (cmd.args.size() >= 2) {
                std::vector<std::string> members(cmd.args.begin() + 1, cmd.args.end());
                long long added = storage_.sadd(cmd.args[0], members);
                response_ = std::to_string(added) + "\r\n";
            } else {
                response_ = "ERROR: SADD requires set key and at least one member\r\n";
            }
            break;
            
        case CommandType::SMEMBERS:
            if (cmd.args.size() >= 1) {
                auto members = storage_.smembers(cmd.args[0]);
                if (members.empty()) {
                    response_ = "(empty set)\r\n";
                } else {
                    std::ostringstream oss;
                    for (const auto& pair : members) {
                        oss << pair.first << "\r\n";
                    }
                    response_ = oss.str();
                }
            } else {
                response_ = "ERROR: SMEMBERS requires set key\r\n";
            }
            break;
            
        case CommandType::SREM:
            if (cmd.args.size() >= 2) {
                std::vector<std::string> members(cmd.args.begin() + 1, cmd.args.end());
                long long removed = storage_.srem(cmd.args[0], members);
                response_ = std::to_string(removed) + "\r\n";
            } else {
                response_ = "ERROR: SREM requires set key and at least one member\r\n";
            }
            break;
            
        case CommandType::SISMEMBER:
            if (cmd.args.size() >= 2) {
                bool isMember = storage_.sismember(cmd.args[0], cmd.args[1]);
                response_ = std::string(isMember ? "1" : "0") + "\r\n";
            } else {
                response_ = "ERROR: SISMEMBER requires set key and member\r\n";
            }
            break;
            
        case CommandType::SCARD:
            if (cmd.args.size() >= 1) {
                long long count = storage_.scard(cmd.args[0]);
                response_ = std::to_string(count) + "\r\n";
            } else {
                response_ = "ERROR: SCARD requires set key\r\n";
            }
            break;
            
        case CommandType::EXPIRE:
            if (cmd.args.size() >= 2) {
                try {
                    long long seconds = std::stoll(cmd.args[1]);
                    if (storage_.expire(cmd.args[0], seconds)) {
                        response_ = "1\r\n";
                    } else {
                        response_ = "0\r\n";
                    }
                } catch (const std::exception&) {
                    response_ = "ERROR: Invalid seconds value\r\n";
                }
            } else {
                response_ = "ERROR: EXPIRE requires key and seconds\r\n";
            }
            break;
            
        case CommandType::TTL:
            if (cmd.args.size() >= 1) {
                long long remaining = storage_.ttl(cmd.args[0]);
                response_ = std::to_string(remaining) + "\r\n";
            } else {
                response_ = "ERROR: TTL requires key\r\n";
            }
            break;
            
        case CommandType::UNKNOWN:
        default:
            response_ = "ERROR: Unknown command\r\n";
            break;
    }
}