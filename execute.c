#include "def.h"
#include "externs.h"
#include "execute.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <linux/limits.h>
#include <fcntl.h>

void forkexec(int i)
{
	pid_t pid;
	pid = fork();
	if (pid == -1)
		ERR_EXIT("fork"); //??
	if (pid > 0)
	{
		if (backgnd == 1)
			printf("%d\n", pid);
		/* 父进程 */
		lastpid = pid;

	}
	else if (pid == 0)
	{
		//backgnd == 1时，将第一条简单命令的infd重定向至dev/null
		/* 当第一条命令试图从标准输入获取数据的时候立即返回EOF*/
		if (cmd[i].infd == 0 && backgnd == 1)
			cmd[i].infd = open("/dev/null", O_RDONLY);	//??

														//将第一个简单命令作为成一个进程组组长
		if (i == 0)
			setpgid(0, 0);
		/* 子进程 */
		if (cmd[i].infd != 0)		//outfd=0, 无输入重定向
		{
			close(0);		//??
			dup(cmd[i].infd);
		}
		if (cmd[i].outfd != 1)		//outfd=1, 无输出重定向
		{
			close(1);		//??
			dup(cmd[i].outfd);
		}
		int j;
		for (j = 3; j < FOPEN_MAX; j++)
			close(j);	//??
		if (backgnd == 0)
		{
			// 如果是前台作业， 信号SIGINT， 需要恢复成默认值 
			signal(SIGINT, SIG_DFL);
			//前台作业退出也需要改为默认值，需要ctr+C / ctr+‘/' 来退出当前前台进程
			signal(SIGQUIT, SIG_DFL);
		}
		execvp(cmd[i].args[0], cmd[i].args);
		exit(EXIT_FAILURE);
	}

}

int execute_disk_command(void)
{
	if (cmd_count == 0)
		return 0;

	/* 创建子进程 */
	/*
	pid_t pid = fork();
	if (pid == -1) //创建子进程失败
	ERR_EXIT("fork");

	if (pid == 0)
	execvp(cmd.args[0], cmd.args); //unistd.h 中int execvp(const char *file, char *const argv[]);
	wait(NULL);
	*/

	if (infile[0] != '\0')
	{
		cmd[0].infd = open(infile, O_RDONLY);
	}
	if (outfile[0] != '\0')
	{
		if (append)
			cmd[cmd_count - 1].outfd = open(outfile, O_WRONLY | O_CREAT | O_APPEND, 0666);	//以追加的方式打开输出重定向的文件???
		else
			cmd[cmd_count - 1].outfd = open(outfile, O_WRONLY | O_CREAT | O_APPEND, 0666);	//以清除的方式打开???
	}

	/* 因为后台作业，父进程不会调用wait() 来等待子进程退出， 为了避免产生僵死进程，
	可以忽略信号SIGCHLD,子进程退出时会给父进程发送一个SIGCHLD的信号，来忽略掉它*/
	if (backgnd == 1)
		signal(SIGCHLD, SIG_IGN);
	else
		signal(SIGCHLD, SIG_DFL);

	int i;
	int fd;
	int fds[2];
	for (i = 0; i < cmd_count; i++)
	{
		/* 如果不是最后一条命令， 则需要创建管道 */
		if (i < cmd_count - 1)
		{
			pipe(fds);
			cmd[i].outfd = fds[1];
			cmd[i + 1].infd = fds[0];
		}

		forkexec(i);

		if ((fd = cmd[i].infd) != 0)	//infd=0, 无输入重定向
			close(fd);
		if ((fd = cmd[i].outfd) != 1)	//outfd=1, 无输出重定向
			close(fd);
	}
	if (backgnd == 0)
	{
		/* 前台作业， 需要等待管道中最后一个命令退出 */
		while (wait(NULL) != lastpid)	//??
			;
	}
	return 0;
}