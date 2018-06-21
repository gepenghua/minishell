#include "parse.h"
#include "init.h"
#include "def.h"

char avline[MAXLINE + 1];	//参数列表数组
char cmdline[MAXLINE + 1]; //cmdline存放的是读取的命令行
char *avptr;	//指向avline
char *lineptr;	//指向cmdline

char infile[MAXNAME + 1];  //输入重定向的存储的内容
char outfile[MAXNAME + 1]; //存储输出重定向的内容

COMMAND cmd[PIPELINE];	  //cmd存放的是将cmdline解析后的命令

int cmd_count;
int backgnd;


int main(void)
{
	setup();
	shell_loop();
	return 0;
}