#include "parse.h"
#include "externs.h"
#include "init.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void get_command(int i);
int check(const char *str);
void get_name(char *name);
/*
 * shell主循环
 */
void shell_loop(void)
{
	while (1)
	{
		printf("[minishell]$ ");
		fflush(stdout);//清除缓存区，打印输出流
		init();
		if (read_command() == -1)
			break;
		parse_command();
		execute_command();
	}

	printf("\nexit\n");
}

/*
 * 读取命令
 * 成功返回0，失败或者读取到文件结束符（EOF)返回-1
 */
int read_command(void)
{
	if (fgets(cmdline, MAXLINE, stdin) == NULL)
		return -1;
	return 0;
}

/*
 * 解析命令
 * 成功返回解析到的命令个数，失败返回-1
 */
int parse_command(void)
{
	/* cat < test.txt | grep -n public > test2.txt & */
	/* 1. 解析第一条简单命令 */
	get_command(0);
	/* 2. 判定是否有输入重定向符 < */
	if (check("<"))
		getname(infile);
	/* 3. 判定是否有管道 */
	int i ;
	for (i = 1; i < PIPELINE; i++)
	{
		if (check("|"))
			get_command(i);
		else
			break;
	}
	/* 4.判定是否有输出重定向符 */
	if (check(">"))
		getname(outfile);
	/* 5.判定是否后台作业 */
	if (check("&"))
		backgnd = 1;
	/* 6.判定命令结束 */
	if (check("\n"))
	{
		cmd_count = i;
		return cmd_count;
	}
	else
	{
		fprintf(stderr, "Command line syntax error\n");
		return -1;
	}
}

/*
 *执行命令
 *成功返回0，失败返回-1
 */
int execute_command(void)
{
	/* 创建子进程 */
	/*
	pid_t pid = fork();
	if (pid == -1) //创建子进程失败
		ERR_EXIT("fork");

	if (pid == 0)
		execvp(cmd.args[0], cmd.args); //unistd.h 中int execvp(const char *file, char *const argv[]);
	wait(NULL);
	*/
	return 0;
}

/*解析简单命令至 cmd[i] 
 *提取命令参数到avline数组中
 *并且将COMMAND结构中的args[]中的每个指针指向这些字符串
 */
void get_command(int i)
{
	/* cat < test.txt | grep -n public > test2.txt & */
	int j = 0;
	int inword;
	while(*lineptr!= '\0')
	{
		//去除命令开头的空格
		while (*lineptr == ' ' || *lineptr == '\t')
			*lineptr++;// ??*
		cmd[i].args[j] = avptr;// avline
		while (*lineptr != '\0'
			&& *lineptr != '\t'
			&& *lineptr != '<'
			&& *lineptr != '>'
			&& *lineptr != '|'
			&& *lineptr != '&'
			&& *lineptr != '\n')
		{
			*avptr++ = *lineptr++;
			inword = 1;
		}
		*avptr++ = '\0';
		switch (*lineptr)
		{
			case ' ':
			case '\t':
				inword = 0;
				j++;
				break;
			case '>':
			case '<':
			case '|':
			case '&':
			case '\n':
				if (inword == 0)
					cmd[i].args[j] = NULL;
				return;
			default: //for'\0'
				return;
		}
	}
}
int check(const char *str)
{
	return 0;
}
void getname(char *name)
{

}
