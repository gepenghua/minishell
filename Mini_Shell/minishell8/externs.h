#ifndef _EXTERNS_H_
#define _EXTERNS_H_

#include "def.h"

extern char avline[MAXLINE + 1];
extern char cmdline[MAXLINE + 1];
extern char *avptr;		//指向avline
extern char *lineptr;	//指向cmdline

extern char infile[MAXNAME + 1];	//输入重定向的存储的内容
extern char outfile[MAXNAME + 1];	 //存储输出重定向的内容

extern COMMAND cmd[PIPELINE];

extern int cmd_count;
extern int backgnd;
extern int append;		//if为1，则是“>>”以追加的方式输出重定向
extern int lastpid;

#endif /*_EXTERNS_H_*/ 
