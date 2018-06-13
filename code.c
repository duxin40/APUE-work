/***
该函数功能为：
　首先通过opendir函数打开一个目录，然后通过dirfd函数提取出目录流的文件描述符，然后再利用fcntl获取close-on-exec标志.接着再来通过open函数（增加了O_DIRECTORY标志就能打开目录了）打开同一个目录，然后再来通过fcntl函数来查看它的close-on-exec标志。
***/
/***
这里的err_exit()函数是我自己定义的，功能就是调用strerror()函数打印出错误信息，并且调用exit()函数退出！
***/

#include<dirent.h>
#include<errno.h>
#include<fcntl.h>
#include<string.h>
#include<stdlib.h>
#include<stdarg.h>
#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>
void err_exit(char *fmt,...);
int main(int argc,char *argv[])
{
    DIR *dirp;
    int dir_fd;
    int val;

    /*用opendir的方式打开目录，并且获取文件描述符，然后查看其close-on-exec标志*/
    if(NULL == (dirp=opendir("/")))
    err_exit("[opendir]: ");
    if(-1 == (dir_fd=dirfd(dirp)))  //获取打开目录流的文件描述符
    err_exit("[dirfd]: ");
    if(-1 == (val=fcntl(dir_fd,F_GETFD)))
    err_exit("[fcntl]: ");

    printf("%-9s: ","[opendir]");
    if(val & FD_CLOEXEC)
    printf("close-on-exec flag is on\n");
    else
    printf("close-on-exec flag is off\n");

    if(-1 == closedir(dirp))
    err_exit("[closedir]: ");

    /*用open的方式打开目录，然后查看其close-on-exec标志*/
    if(-1 == (dir_fd=open("/",O_DIRECTORY)))    //open函数加上O_DIRECTORY标志就能够打开目录了
    err_exit("[open]: ");
    if(-1 == (val=fcntl(dir_fd,F_GETFD)))
    err_exit("[fcntl]: ");

    printf("%-9s: ","[open]");
    if(val & FD_CLOEXEC)
    printf("close-on-exec flag is on\n");
    else
    printf("close-on-exec flag is off\n");

    if(-1 == close(dir_fd))
    err_exit("[close]: ");

    return 0;
}