#ifndef _CMD_H_
#define _CMD_H_

#include <stdio.h>

void execute_command(const char *line, FILE *out);
void cmd_cleanup(void);

#endif
