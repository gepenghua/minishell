#include "parse.h"
#include "externs.h"
#include "init.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


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
	/* 初始输入：ls -l */
	/* 解析达到的结果：ls\0-l\0 */
	char *cp = cmdline;
	char *avp = avline;
	int i = 0;

	while (*cp != '\0')
	{
		/* 去除命令中的空格 */
		while (*cp == ' ' || *cp == '\t')	//过滤命令中的空格
			cp++;
		/* 如果到了行尾退出循环 */
		if (*cp == '\0' || *cp == '\n') //if?
			break;
		cmd.args[i] = avp;

		/* 解析参数 */
		while (*cp != '\0' 
			&& *cp != ' ' 
			&& *cp != '\t' 
			&& *cp != '\n')
			*avp++ = *cp++;
		/* 在每个参数后加一个'\0' */
		*avp++ = '\0';
		printf("[%s]\n", cmd.args[i]);
		i++;
	}
	return 0;
}

/*
 *执行命令
 *成功返回0，失败返回-1
 */
int execute_command(void)
{
	/* 创建子进程 */
	pid_t pid = fork();
	if (pid == -1) //创建子进程失败
		ERR_EXIT("fork");

	if (pid == 0)
		execvp(cmd.args[0], cmd.args); /*unistd.h 中int execvp(const char *file, char *const argv[]);*/
	wait(NULL);
	return 0;
}
