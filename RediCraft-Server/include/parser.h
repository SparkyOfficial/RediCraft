/*
 * parser.h
 * author: Андрій Будильников
 */

#ifndef REDICRAFT_PARSER_H
#define REDICRAFT_PARSER_H

#include <string>
#include <vector>

enum class CommandType {
    PING,
    SET,
    GET,
    INCR,
    DECR,
    INCRBY,
    HSET,
    HGET,
    HGETALL,
    LPUSH,
    RPOP,
    LRANGE,
    EXPIRE,
    TTL,
    SADD,
    SMEMBERS,
    SREM,
    SISMEMBER,
    SCARD,
    UNKNOWN
};

struct Command {
    CommandType type;
    std::vector<std::string> args;
};

class Parser {
public:
    static Command parse(const std::string& input);
};

#endif // REDICRAFT_PARSER_H