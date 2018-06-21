#include "parse.h"
#include "externs.h"
#include "init.h"
#include "execute.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <linux/limits.h>
#include <fcntl.h>


int execute_command(void);
void get_command(int i);
int check(const char *str);
void getname(char *name);
void print_command(); 

/*
 * shell主循环
 */
void shell_loop(void)
{
	while (1)
	{
		/* 初始化环境 */
		init();
		/* 读取命令 */
		if (read_command() == -1)
			break;
		/* 解析命令 */
		parse_command();
			//print_command();		//打印命令
		/* 执行命令 */
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
	//从文件流读取一行，送到缓冲区,遇到换行符或者缓冲区已满，fgets就会停止，返回读到的数据, 每次调用，fgets都会把缓冲区的最后一个字符设为null
	/* 按行读取命令，cmdline中包含\n字符 */
	if (fgets(cmdline, MAXLINE, stdin) == NULL)		//如果读取为空，即cmdline仍为空，则返回-1；
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
	if (check("\n"))	//如果开始为换行符，则命令为空
		return 0;

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
	{
		if (check(">"))		//如果是“>>”则是以追加的方式
			append = 1;
		getname(outfile);
	}
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
	/* 先判定是否内部命令 */
	execute_disk_command();

	return 0;
}

void print_command()
{
	int i;
	int j;
	printf("cmd_count = %d\n", cmd_count);
	if (infile[0] != '\0')
		printf("infile = [%s]\n", infile);
	if (outfile[0] != '\0')
		printf("outfile = [%s]\n", outfile);

	for ( i = 0; i < cmd_count; i++)
	{
		j = 0;
		while (cmd[i].args[j] != NULL)
		{
			printf("[%s] ", cmd[i].args[j]);
			j++;
		}
		printf("\n");
	}
}

/*解析简单命令至 cmd[i] 
 *提取命令参数到avline数组中
 *并且将COMMAND结构中的args[]中的每个指针指向这些字符串
 */
void get_command(int i)
{
	/* cat < test.txt | grep -n public > test2.txt & */
	int j = 0;
	int inword;		//if为1， 表示‘<’与前面命令没有空格，直接相连
	while(*lineptr != '\0')
	{
		//去除命令开头的空格
		while (*lineptr == ' ' || *lineptr == '\t')
			lineptr++;		// ??*
		/* 将当前第i条命令的当前第j个参数指向avptr */
		cmd[i].args[j] = avptr;// avline
		/* 提取参数 */
		while (*lineptr != '\0'
			&& *lineptr != ' '
			&& *lineptr != '\t'
			&& *lineptr != '<'
			&& *lineptr != '>'
			&& *lineptr != '|'
			&& *lineptr != '&'
			&& *lineptr != '\n')
		{
			/* 将指针lineptr指向的（存放输入的原始命令行）的数组cmdline的参数，
			提取到 avptr 指针指向的数组 avline */
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

/*
 * 将lineptr中的字符串与str进行匹配
 * 成功返回 1，lineptr 移过所匹配的字符
 * 失败返回 0，lineptr 保持不变
 */
int check(const char *str)
{
	char *p;
	while (*lineptr == ' ' || *lineptr == '\t')
		lineptr++;
	p = lineptr;
	while (*str != '\0' && *str == *p)
	{
		str++;
		p++;
	}

	if (*str == '\0')
	{
		lineptr = p;	/* lineptr 移过所匹配的字符 */
		return 1;
	}
	return 0;
}
void getname(char *name)
{
	while (*lineptr == ' ' || *lineptr == 't')
		lineptr++;
	while (*lineptr != '\0'
		&& *lineptr != ' '
		&& *lineptr != '\t'
		&& *lineptr != '<'
		&& *lineptr != '>'
		&& *lineptr != '|'
		&& *lineptr != '&'
		&& *lineptr != '\n')
	{
		*name++ = *lineptr++;
	}
	*name = '\0';
}


