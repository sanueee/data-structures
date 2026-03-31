#ifndef CMD_H
#define CMD_H

#include "memstat.h"

int execute_command(const char* buffer, FILE *output);
void cmd_clean();

#endif