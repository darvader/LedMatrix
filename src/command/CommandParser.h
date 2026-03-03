#ifndef COMMAND_PARSER_H
#define COMMAND_PARSER_H

#include "Command.h"

class CommandParser {
public:
    static Command parse(const char* packet, int len);
};

#endif // COMMAND_PARSER_H
