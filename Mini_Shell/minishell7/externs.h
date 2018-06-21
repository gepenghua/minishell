#ifndef _EXTERNS_H_
#define _EXTERNS_H_

#include "def.h"

extern char avline[MAXLINE + 1];
extern char cmdline[MAXLINE + 1];
extern char *avptr;		//ָ��avline
extern char *lineptr;	//ָ��cmdline

extern char infile[MAXNAME + 1];	//�����ض���Ĵ洢������
extern char outfile[MAXNAME + 1];	 //�洢����ض��������

extern COMMAND cmd[PIPELINE];

extern int cmd_count;
extern int backgnd;
extern int append;		//ifΪ1�����ǡ�>>����׷�ӵķ�ʽ����ض���
extern int lastpid;

#endif /*_EXTERNS_H_*/ 
