#include "parse.h"
#include "init.h"
#include "def.h"

char avline[MAXLINE + 1];
char cmdline[MAXLINE + 1]; //cmdline存放的是读取的命令行
COMMAND cmd;			   //cmd存放的是将cmdline解析后的命令


int main(void)
{
	setup();
	shell_loop();
	return 0;
}