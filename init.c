#include "init.h"
#include "externs.h"
#include <stdio.h>
#include <signal.h>
#include <string.h>

void sigint_handler(int sig);

void setup(void)
{
	signal(SIGINT, sigint_handler);
	signal(SIGQUIT, SIG_IGN);
}

void sigint_handler(int sig)
{
	printf("\n[minishell]$ ");
	fflush(stdout);
}

void init(void)
{
	memset(cmd, 0, sizeof(cmd));
	memset(cmdline, 0, sizeof(cmdline));
	memset(avline, 0, sizeof(avline));
	lineptr = cmdline;
	avptr = avline;

	memset(infile, 0, sizeof(infile));
	memset(outfile, 0, sizeof(outfile));

	cmd_count = 0;	//当前管道行当中总的命令个数
	backgnd = 0;	//if为1，检查是否有 '&', 后台运行
	append = 0;		//if为1，则是“>>”以追加的方式输出重定向
}
