/*
 * parser.cpp
 * author: Андрій Будильников
 */

#include "parser.h"
#include <sstream>
#include <algorithm>
#include <cctype>

Command Parser::parse(const std::string& input) {
    Command cmd;
    cmd.type = CommandType::UNKNOWN;
    
    // Split input into tokens
    std::istringstream iss(input);
    std::vector<std::string> tokens;
    std::string token;
    
    while (iss >> token) {
        tokens.push_back(token);
    }
    
    if (tokens.empty()) {
        return cmd;
    }
    
    // Convert first token to uppercase for comparison
    std::string command = tokens[0];
    std::transform(command.begin(), command.end(), command.begin(), ::toupper);
    
    if (command == "PING") {
        cmd.type = CommandType::PING;
    } else if (command == "SET" && tokens.size() >= 3) {
        cmd.type = CommandType::SET;
        cmd.args.push_back(tokens[1]);  // key
        cmd.args.push_back(tokens[2]);  // value
    } else if (command == "GET" && tokens.size() >= 2) {
        cmd.type = CommandType::GET;
        cmd.args.push_back(tokens[1]);  // key
    } else if (command == "INCR" && tokens.size() >= 2) {
        cmd.type = CommandType::INCR;
        cmd.args.push_back(tokens[1]);  // key
    } else if (command == "DECR" && tokens.size() >= 2) {
        cmd.type = CommandType::DECR;
        cmd.args.push_back(tokens[1]);  // key
    } else if (command == "INCRBY" && tokens.size() >= 3) {
        cmd.type = CommandType::INCRBY;
        cmd.args.push_back(tokens[1]);  // key
        cmd.args.push_back(tokens[2]);  // increment
    } else if (command == "HSET" && tokens.size() >= 4) {
        cmd.type = CommandType::HSET;
        cmd.args.push_back(tokens[1]);  // hash key
        cmd.args.push_back(tokens[2]);  // field
        cmd.args.push_back(tokens[3]);  // value
    } else if (command == "HGET" && tokens.size() >= 3) {
        cmd.type = CommandType::HGET;
        cmd.args.push_back(tokens[1]);  // hash key
        cmd.args.push_back(tokens[2]);  // field
    } else if (command == "HGETALL" && tokens.size() >= 2) {
        cmd.type = CommandType::HGETALL;
        cmd.args.push_back(tokens[1]);  // hash key
    } else if (command == "LPUSH" && tokens.size() >= 3) {
        cmd.type = CommandType::LPUSH;
        cmd.args.push_back(tokens[1]);  // list key
        // Add all remaining tokens as values
        for (size_t i = 2; i < tokens.size(); ++i) {
            cmd.args.push_back(tokens[i]);
        }
    } else if (command == "RPOP" && tokens.size() >= 2) {
        cmd.type = CommandType::RPOP;
        cmd.args.push_back(tokens[1]);  // list key
    } else if (command == "LRANGE" && tokens.size() >= 4) {
        cmd.type = CommandType::LRANGE;
        cmd.args.push_back(tokens[1]);  // list key
        cmd.args.push_back(tokens[2]);  // start index
        cmd.args.push_back(tokens[3]);  // end index
    } else if (command == "EXPIRE" && tokens.size() >= 3) {
        cmd.type = CommandType::EXPIRE;
        cmd.args.push_back(tokens[1]);  // key
        cmd.args.push_back(tokens[2]);  // seconds
    } else if (command == "TTL" && tokens.size() >= 2) {
        cmd.type = CommandType::TTL;
        cmd.args.push_back(tokens[1]);  // key
    }
    
    return cmd;
}