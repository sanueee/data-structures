#ifndef _CMD_H_
#define _CMD_H_

#include "types.h"

void read_file();
void usage();
void read_console(command*);
void execute_command(char*, list*, FILE*);

#endif