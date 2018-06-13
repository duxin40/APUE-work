第一章

1.1  引言

所有操作系统都为他们所运行的程序提供服务。典型的服务包括：执行新程序、打开文件、读文件、分配存储区以及获取当前时间等，

本书集中阐述不同版本的UNIX操作系统所提供的服务。

本章会简要的介绍UNIX提供的各种服务，在以后的各章中将对这些概念做更详细的说明。 

1.2  UNIX体系结构

从严格意义上说，可将操作系统定义为一种软件，它控制计算机硬件资源，提供程序运行环境。我们通常将这种软件称为内核。

1.3  登录

1 登录名

用户在登录UNIX系统时，先键入登录名，然后键入口令。系统在其口令文件（通常是/etc/passwd文件）中查看用户名。

2 shell

用户登录后，系统通常先显示一些系统信息，然后用户就可以向shell程序键入命令。

shell是一个命令行解释器，它读取用户输入，然后执行命令。

1.4  文件和目录

1 文件系统

UNIX文件系统是目录和文件的一种层次结构，所有东西的起点都是根的目录，这个目录名称是一个字符“/”。

目录是一个包含目录项的文件，可以认为每个目录项都包含一个文件名，同时还包含该文件属性的信息(ls -l可以查看)。第四章会详细说明文件的各种属性。

2 文件名

目录中的各个名字成为文件名，创建目录时会自动创建两个文件名：.(当前目录)和..(父目录)   (可见为什么 cd .. 会回到父目录)

3 路径名

已斜线开头的路径名称成为绝对路径名（可以理解为以根目录开头的相对路径名），否则为相对路径名。

不难列出一个目录中所有文件的名字，下面是ls(1)命令的简要实现。

include "apue.h"
include <dirent.h>

int main(int argc,char *argv[])
{
    DIR *dp;
    struct dirent *dirp;
    if(argc!=2)
        err_quit("usage:ls directory_name");
    if((dp=opendir(argv[1]))==NULL)
        err_sys("can't open %s",argv[1]);
    while((dirp=readdir(dp))!=NULL)
        printf("%s\n",dirp->d_name);
    closedir(dp);
    exit(0);
}
复制代码
 

可以使用gcc编译该文件  执行命令cc myls.c进行编译   会生成a.out文件  执行./a.out命令来运行  (./代表当前目录，如果不指定，系统会在环境变量中设置的路径中找可执行文件)

4  工作目录

每个进程都有一个工作目录，有时称其为当前工作目录，进程可以用chdir函数更改其工作目录

5 起始目录

登录时，工作目录设置为起始目录，该起始目录从口令文件中相应用户的登录项中取得。

 

1.5  输入与输出

1 文件描述符

文件描述符通常是一个小的非负整数，用来标识一个特定进程正在访问的文件。当内核打开一个现有文件或创建一个新文件时，

它都返回一个文件描述符，可以通过这个文件描述符进行读写文件。

2  标准输入、标准输出和标准错误

每当运行一个新程序时，所有的shell都为其打开3个文件描述符，即标准输入(0)、标准输出(1)和标准错误(2)，如果不做特殊处理，

则这3个描述符都链接到终端。

3  不带缓冲的I/O

函数open、read、write、lseek以及close提供了不带缓冲的I/O。这些函数都使用文件描述符

下面程序展示了如何复制UNIX下的普通文件

复制代码
include "apue.h"

define BUFFSIZE 4096

int main(void)
{
    int n;
    char buf[BUFFSIZE];
    while((n=read(STDIN_FILENO,buf,BUFFSIZE))>0)
        if(write(STDOUT_FILENO,buf,n)!=n)
            err_sys("write error");
    if(n<0)
        err_sys("read error");
    exit(0);
}
复制代码
大多数shell都提供一种方法，使其中任何一个或所有这3个描述符都能重定向到某个文件。例如ls > file.list 可以将ls的输出(标准输出 文件描述符为1)重定向到名为file.list的文件。

如下执行上面的程序： ./a.out > data 可以把程序的标准输出重定向到文件data   ./a.out < infile > outfile 把程序的标准输入重定向到infile  把标准输出重定向到outfile，实现了文件的复制。

4  标准I/O

标准I/O 为那些不带缓冲的I/O函数提供了一个带缓冲的接口，下面演示使用标准I/O复制UNIX文件

复制代码
include "apue.h"

int main(void)
{
    int c;
    while((c=getc(stdin))!=EOF)
        if(putc(c,stdout)==EOF)
            err_sys("output error");
    if(ferror(stdin))
        err_sys("input error");
    exit(0);
}
复制代码
 

 

1.6 程序与进程

1 程序

程序是一个存储在磁盘上某个目录中的可执行文件

2 进程和进程ID

程序的执行实例被称为进程，UNIX系统确保每个进程都有一个唯一的数字标识符，称为进程ID。进程ID总是一个非负整数

下面程序用于打印进程ID

复制代码
include "apue.h"

int main(void)
{
    printf("hello world from process ID %ld\n",(long)getpid());
    exit(0);
}
复制代码
3 进程控制

有3个用于进程控制的主要函数：fork、exec和waitpid。

UNIX系统的进程控制功能可以用一个简单得程序说明，下面程序从标准输入读取命令，然后执行命令，类似于shell程序的基本实施部分。

复制代码
include "apue.h"
include <sys/wait.h>


int main(void)
{
    char buf[MAXLINE];
    pid_t pid;
    int status;
    printf("%% ");
    while(fgets(buf,MAXLINE,stdin)!=NULL)
    {
        if(buf[strlen(buf)-1]=='\n')
            buf[strlen(buf)-1]=0;
        if((pid=fork())<0)
            err_sys("fork error");
        else if(pid==0)
        {
            execlp(buf,buf,(char *)0);
            err_ret("couldn't execute:%s",buf);
            exit(127);
        }
        if((pid=waitpid(pid,&status,0))<0)
            err_sys("waitpid error");
        printf("%% ");
    }
    exit(0);
}
复制代码
4  线程和线程ID

与进程相同，线程也用ID标识。但是，线程ID只在它所属的进程内起作用。

 

1.7  出错处理

当UNIX系统函数出错时，通常会返回一个负值，而且整形变量errno通常被设置为具有特定信息的值。

文件<error.h>中定义了error以及可以赋予它的各种常量。

C标准定义了两个函数，用于打印出错信息

include <string.h>
char *strerror(int errnum);
strerror函数将errnm(通常就是error值)映射为一个出错消息字符串，并且返回此字符串的指针。

include <stdio.h>
void perror(const char *msg);
perror函数基于errno的当前值，在标准错误上产生一条出错信息，然后返回。

下面程序显示这两个出错函数的使用方法

复制代码
include "apue.h"
include <errno.h>

int main(int argc,char *argv[])
{
    fprintf(stderr,"EACCES:%s\n",strerror(EACCES));
    errno=ENOENT;
    perror(argv[0]);
    exit(0);
}
复制代码
 

1.8  用户标识

1 用户ID

用户ID是一个数值，用来确定一个用户。用户ID为0为根用户或超级用户，超级用户对系统有自由的支配权。

2 组ID

组ID是由系统管理员在指定用户登录名时分配的，可以把多个用户分成一组。

下面程序用于打印用户ID和组ID

复制代码
include "apue.h"

int main(void)
{
    printf("uid=%d,gid=%d\n",getuid(),getgid());
    exit(0);
}
复制代码
3 附属组ID

除了在口令文件中对一个登录名指定一个组ID之外，大多数UNIX系统版本还允许一个用户属于另外一些组。

 

1.9 信号

信号用于通知进程发生了某种情况。例如，若某一进程执行除法操作，其除数为0，则将名为SIGFPE(浮点异常)的信号发送给进程。

进程有以下3种处理信号的方式：

1 忽略信号

2 按系统默认方式处理

3 提供一个函数，信号发生时调用该函数，这被称为捕捉该信号。

终端键盘上有两种产生信号的方法，分别称为中断键(通常是Delete或Ctrl+C)和退出键(通常是Ctrl+\)

修改之前的shell实例，使程序可以捕获SIGINT信号，我们会在第10章详细的介绍信号。

复制代码
include "apue.h"
include <sys/wait.h>

static void sig_int(int);

int main(void)
{
    char buf[MAXLINE];
    pid_t pid;
    int status;
    if(signal(SIGINT,sig_int)==SIG_ERR);
                err_sys("signal error");
    printf("%% ");
    while(fgets(buf,MAXLINE,stdin)!=NULL)
    {
        if(buf[strlen(buf)-1]=='\n')
            buf[strlen(buf)-1]=0;
        if((pid=fork())<0)
            err_sys("fork error");
        else if(pid==0)
        {
            execlp(buf,buf,(char *)0);
            err_ret("couldn't execute:%s",buf);
            exit(127);
        }
        if((pid=waitpid(pid,&status,0))<0)
            err_sys("waitpid error");
        printf("%% ");
    }
    exit(0);
}
void sig_int(int signo)
{
        printf("interrupt\n%%");
}
    
复制代码
 

1.10  时间值

历史上，UNIX系统使用过两种不同的时间值

1 日历时间。该值是从1970年1月1日00:00:00这个特定时间以来所经过的秒数的累计值。

2 进程时间。也被称为CPU时间

   当度量一个进程的执行时间时，UNIX系统为一个进程维护了3个进程时间值

   时钟时间  即进程运行的时间总量，其值与系统中同时运行的进程数有关

   用户CPU时间  执行用户指令所用的时间量 

　系统CPU时间  执行系统调用的时间

 

1.11 系统调用和库函数

系统调用提供的函数如open, close, read, write, ioctl等，系统调用发生在内核空间

标准C库函数提供的文件操作函数如fopen, fread, fwrite, fclose, fflush, fseek等属于库函数，底层也是通过系统调用来实现的。

第七章 
 
7.1 引言
在介绍进程控制原句之前应了解进程的环境。本章便是介绍进程环境的。

7.2 main函数
main函数的原型是：

int main(int argc, *argv[]);
1
argc:命令行参数的数目
argv：指向参数的各个指针所组成的数组
在内核执行C程序时，启动例程从内核取得命令行参数和环境变量值，可执行文件将此启动例程指定为程序的起始地址——这是由连接编辑器设置的。

7.3 进程中止
有8种方式使进程终止，其中有5种为正常终止，它们是：

1）从main返回
2）调用exit
3）调用_exit或_Exit
4）最后一个线程从其启动例程返回
5）从最后一个线程调用pthread_exit
6）调用abort
7）接到一个信号
8）最后一个线程对取消请求作出响应
3个函数用于正常终止一个程序：_exit和_Exit立即进入内核，exit则是先执行一些清理处理，再返回内核：

include <stdlib.h>
void exit(int status);
void _Exit(int status);
include <unistd.h>
void _exit(int status);
status：终止状态。若main的返回类型是整型，而且main执行到最后一条语句时返回，那么该进程的终止状态为0
exit和return差不多，使用exit的话，可以通过UNIX的grep应用程序来找出程序中所有的exit调用，但是可能有些编译器会发出不必要的警告信息。

ISO C规定，一个进程至多登记32个函数，这些函数将由exit（或return）自动调用。我们称这些函数为终止处理程序，并调用atexit函数来登记这些函数：

#include <stdlib.h>
int atexit(void (*func)void);
                //若成功，返回0；若出错，返回非0
该参数为一个 无参数无返回值的函数的指针
exit调用atexit记录的函数的顺序与它们登记的顺序相反。若同一函数登记多次，也会被调用多次
若exit调用了exec函数族（没学到）中的任一函数，清除所有已安装的终止处理程序
7.4 命令行参数
当执行一个程序时，调用exec的进程可将命令行参数传递给该新程序。这是UNIX shell的常规操作。

ISO C和POSIX.1都要求argv[argc]是一个空指针，故可将参数处理循环写为：

for (i = 0; argv[i] != NULL; i++)
1
7.5 环境表
每个程序都接收到一张环境表，环境表是一个字符指针数组，其中每个指针包含一个以null结束的C字符串的地址。全局变量environ包含了该指针数组的地址：

extern char **environ;
1
我们称environ为环境指针，指针数组为环境表，其中各指针指向的字符串为环境字符串

通常用getenv和putenv来访问特定的环境变量（见7.9节），而不是用environ变量

7.6 C程序的存储空间布局
C程序一直由以下部分组成：

1）正文段。 由CPU执行机器指令的部分。常常是可共享的和只读的。
2）初始化数据段（数据段）。 包含了程序中需明确地赋初值的变量。存放变量及其初值。
3）未初始化数据段（bss段）。 函数外的声明使此变量存放在非初始化数据段中。程序开始执行前，内核将此段中的数据初始化为0或空指针。
4）栈。自动变量以及每次函数调用时所需保存的信息都存放在此段中。每次函数调用时，其返回地址以及调用者的环境信息1都存放在栈中。
5）堆。通常在堆中进行动态内存分配。堆位于未初始化数据和栈之间。
需要存放在磁盘程序文件中的段只有正文段和初始化数据段。 
size命令（size 文件名）报告正文段、数据段和bss段的长度（字节）

7.7 共享库
共享库使得可执行文件中不在需要包含共用的库函数，而只需要在所有进程都可以引用的存储区中保存这种库例程的一个副本。可以显著地减小可执行文件的大小，但增加了一些运行开销时间（链接程序和共享库）。还有个好处是可以用库函数的新版本代替老版本而无需对使用该库的程序进行重新链接编辑。

7.8 存储空间分配
ISO C说明了3个用于存储空间动态分配的函数

1）malloc，分配指定字节数的存储区。初始值不确定。
2）calloc，为指定数量指定长度的对象分配存储空间。该空间每一位都初始为0.
3）realloc，增加或减少以前分配区的长度（可能需要移位来确保存得下）。新增区域内的初始值不确定。
include <stdlib.h>
void *malloc(size_t size);
void *calloc(size_t nobj, size_t size);
void *recalloc(void *ptr, size_t newsize);
                //若成功，返回非空指针；若出错，返回NULL
void free(void *ptr);
这3个分配函数所返回的指针一定是适当对齐的，使其可用于任何数据对象。
函数free释放ptr指向的存储空间
nobj：对象的数量
newsize：分配的新存储区的长度
这些分配例程通常用系统调用sbrk实现，该调用可扩充或缩小进程的堆（后面会讲）

大多数实现所分配的存储空间比要求的要大一些，用来记录管理信息——分配快的长度、指向下一分配快的指针等。（所以写操作溢出的结果是灾难性的）

分配内存、释放内存的操作很容易出错（都知道），所以有一些替代的存储空间分配程序： 
- 1. libmalloc 
- 2. vmalloc 
- 3. quick-fit 
- 4. jemalloc 
- 5. TCMalloc 
- 6. 函数alloca

我就不一一照抄具体内容了..有兴趣的可以查一下

7.9 环境变量
环境变量的形式是：name = value 
UNIX内核不查看这些字符串，它们的解释取决于各个应用程序。

函数getenv可以取得环境变量值：

include <stdlib.h>
char *getenv(const char *name);
                //返回与name关联的value指针；若未找到，返回
由POSIX.1和XSI扩展定义了如下环境变量

变量	说明
COLUMNS	终端宽度
DATEMSK2	getdate(3)模板文件路径名
HOME	home起始目录
LANG	语言（书上写错了吧）
LC_ALL	本地名
LC_COLLATE	本地排序名
LC_CTYPE	本地字符分类名
LC_MESSAGES	本地消息名
LC_MENETARY	本地货币编辑名
LC_NUMERICC	本地数字编辑名
LC_TIME	本地日期/时间格式名
LINES	终端高度
LOGNAME	登录名
MSGVERB3	fmtmsg(3)处理的消息组成部分
NLSPATH	消息类模板序列
PATH	搜索可执行文件的路径前缀列表
PWD	当前工作目录的绝对路径名
SHELL	用户首选的shell名
TERM	终端类型
TMPDIR	在其中创建临时文件的目录路径名
TZ	时区信息
有3个函数可以设置环境变量（并不是所有系统都支持）：

include <stdlib.h>
int putenv(char *str);
                //若成功，返回0；若出错，返回非0
int setenv(const char *name, const char *value, int rewrite);
int unsetenv(const char *name);
                //若成功，返回0；若出错，返回-1
putenv取形式为name=value的字符串。若name已存在，则删除原来的定义。
rewrite：若name已存在，那么rewrite为非0时才覆写；为0时不覆写。
unsetenv删除name的定义，即使name不存在也不算出错。
然后是关于如何修改环境表的具体实现，由于环境表在进程存储空间的顶部（栈之上），所以给他增加一个元素没那么容易，可以思考一下。

7.10 函数setjmp和longjmp
C中，goto是不能跨越函数的，setjump和longjump可以执行这种跳转功能，对于处理很深层嵌套函数调用中的出错是很有用的。

include <setjmp.h>
int setjmp(jmp_buf env);
                //若直接调用，返回0；若从longjump返回，则为非0
void longjmp(jmp_buf env, int val);
在需要返回到的位置调用setjmp
jmp_buf是某种形式的数组，env变量通常为全局变量
longjmp可把值val传递给setjmp，以判断从何处跳回
回跳时不能确定其中自由变量和寄存器变量的值是否回滚（看系统），若不想使其值回滚，可定义其具有volatile属性。所以声明自动变量的函数已经返回后，不能再引用这些自动变量。
7.11 函数getrlimit和setrlimit
getrlimit和setrlimit可以查询和更改进程的资源组限制：

include <sys/resource.h>
int getrlimit(int resource, struct rlimit *rlptr);
int setrlimit(int resource, const struct rlimit *rlptr);
                //若成功，返回0；若出错，返回非0
struct rlimit {
    rlim_t rlim_cur;            //soft limit: current limit
    rlim_t rlim_max;            //hard limit: maximum value for rlim_cur
};

硬限制值必须大于或等于软限制值
只有超级用户才可以提高进程的硬限制值（也就是说普通用户可以降低硬限制值，但是是不可逆的）
常量RLIM_INFINITY指定了一个无限量的限制
resource的取值是一张大大的表。。。就不列出来了（各种限制）
子进程继承父进程的资源限制

第十一章

线程概念

每个线程都包含有表示执行环境所必须的信息：线程ID、一组寄存器值、栈、调度优先级和策略、信号屏蔽字、errno变量以及线程私有数据。

一个进程的所有信息对该进程的所有线程都是共享的，包括可执行程序的代码、程序的全局内存和堆内存、栈以及文件描述符。

线程标识

每个线程都有一个线程ID，线程ID只有在它所属的进程上下文中才有意义。

可以使用下面函数来对两个线程ID进行比较

include <pthread.h>
int pthread_equal(pthread_t tid1,pthread_t tid2);
可以通过pthread_self函数获得自身的线程ID

include <pthread.h>
pthread_t pthread_self(void);
 

线程创建

include <pthread.h>
int pthread_create(pthread_t *restrict tidp,const pthread_attr_t *restrict attr,void *(*start_rtn)(void *),void *restrict arg);
当pthread_create成功返回时，新创建线程的线程ID会被设置成tidp指向的内存空间。

attr属性用于定制各种不同的线程属性。

新创建的线程从start_rtn函数的地址开始运行，该函数只有一个无类型指针参数arg。

下面程序将演示线程的创建，打印出进程ID、新线程的线程ID以及初始线程的线程ID：


线程终止

如果进程中任意线程调用了exit、_Exit或者_exit，那么整个进程就会终止。

单个线程可以通过3种方式退出，因此可以在不终止整个进程的情况下，停止它的控制流。

1 线程可以简单地从启动例程中返回，返回值的线程的退出码。

2 线程可以被同一进程中的其他线程取消。

3 线程调用pthread_exit。

include <pthread.h>
void pthread_exit(void *rval_ptr);
rval_ptr参数是一个无类型指针，进程中的其他线程也可以通过调用pthread_join函数访问到这个指针

include <pthread.h>
int pthread_join(pthread_t thread,void **rval_ptr);
调用pthread_join后，调用线程将一直阻塞，直到指定的线程退出。

如果线程简单地从它的启动例程返回，rval_ptr将包含返回码。如果线程被取消，由rval_ptr指定的内存单元就设置成PTHREAD_CANCELED。

线程可以通过调用pthread_cancel函数来请求取消同一进程中的其他进程。

include <pthread.h>
int pthread_cancel(pthread_t tid);
pthread_cancel并不等待线程终止，它仅仅提出请求，线程可以选择忽略取消或者控制如何被取消。

线程可以安排它退出时需要调用的函数，这与进程在退出时可以用atexit函数安排退出时类似的。

如果线程是通过从它的启动例程中退出返回而终止的话，它的清理处理程序就不会被调用。

include <pthread.h>
void pthread_cleanup_push(void (*rtn)(void *),void *arg);
void pthread_cleanup_pop(int execute);
如果execute参数设置为非0，则调用并删除上次pthread_cleanup_push调用建立的清理处理程序。

如果execute参数为0，则清理函数将不被调用（只删除）。

我们可以调用pthread_detach分离线程。

include <pthread.h>
int pthread_detach(pthread_t tid);

线程同步

当一个线程可以修改的变量，其他线程可以读取或者修改的时候，我们就需要对这些线程进行同步，确保他们在访问变量的存储内容时不会访问到无效的值。

为了解决这个问题，线程不得不使用锁，同一时间只允许一个线程访问该变量。

互斥量

可以使用pthread的互斥接口来保护数据，确保同一时间只有一个线程访问数据。

互斥量从本质上说是一把锁，在访问共享资源前对互斥量进行设置（加锁），在访问完成后释放（解锁）互斥量。

互斥变量使用pthread_mutex_t数据类型表示的。在使用之前，必须对它进行初始化，如果动态分配互斥量，在释放内存前需要调用pthread_mutex_destroy。

include <pthread.h>
int pthread_mutex_init(pthread_mutex_t *restrict mutex,const pthread_mutexattr_t *restrict attr);
int pthread_mutex_destroy(pthread_mutex_t *mutex);
要用默认的属性初始化互斥量，只需把attr设为NULL，也可以把互斥量设置为常量PTHREAD_MUTEX_INITIALIZER（只适用于静态分配的互斥量）进行初始化。

互斥量有以下3种功能

include <pthread.h>
int pthread_mutex_lock(pthread_mutex_t *mutex); 
int pthread_mutex_trylock(pthread_mutex_t *mutex);
int pthread_mutex_unlock(pthread_mutex_t *mutex);
可以使用pthread_mutex_lock对互斥量进行加锁，如果互斥量已经上锁，调用线程将阻塞直到互斥量被解锁。

可以使用pthread_mutex_unlock对互斥量解锁。

如果不希望被阻塞，可以使用pthread_mutex_trylock尝试对互斥量进行加锁。如果互斥量处于未锁住状态，则锁住互斥量，否则返回EBUSY。

避免死锁

如果线程试图对同一个互斥量加锁两次，那么它自身就会陷入死锁状态。

如果两个线程以相反的顺序锁住两个互斥量，也会导致死锁，两个线程都无法向前运行。

在同时需要两个互斥量时，让他们以相同的顺序加锁，这样可以避免死锁。


函数pthread_mutex_timedlock

与pthread_mutex_lock不同的是，pthread_mutex_timedlock允许绑定线程阻塞时间，如果超过时间值，pthread_mutex_timedlock不会对互斥量进行加锁，而是返回错误码ETIMEDOUT。

include <pthread.h>
include <time.h>
int pthread_mutex_timedlock(pthread_mutex_t *restrict mutex,const struct timespec *restrict tsptr);
下面给出如何用pthread_mutex_timedlock避免永久阻塞

 View Code
这个程序对已有的互斥量加锁，演示了pthread_mutex_timedlock是如何工作的。


读写锁

读写锁与互斥量类似，不过读写锁允许更高的并行性。

读写锁可以有3种状态：读模式下加锁状态，写模式下加锁状态，不加锁状态。

一次只有一个线程可以占有写模式的读写锁，但是多个线程可以同时占有读模式的读写锁。

1. 当读写锁是写加锁状态时，在这个锁被解锁之前，所有试图对这个所加锁的线程都会被阻塞。

2. 当读写锁是读加锁状态时，所有试图以读模式对它进行加锁的线程都可以得到访问权，但是任何希望以写模式对此进行加锁的线程都会阻塞，知道所有的线程释放它们的读锁为止。

读写锁在使用之前必须初始化，在释放他们底层的内存之前必须销毁。

include <pthread.h>
int pthread_rwlock_init(pthread_rwlock_t *restrict rwlock,const pthread_rwlockattr_t *restrict attr);
int pthread_rwlock_destroy(pthread_rwlock_t *rwlock);
下面是读写锁的3种用法

include <pthread.h>
int pthread_rwlock_rdlock(pthread_rwlock_t *rwlock);
int pthread_rwlock_wrlock(pthread_rwlock_t *rwlock);
int pthread_rwlock_unlock(pthread_rwlock_t *rwlock);
与互斥量一样，读写锁定义了下面两个函数

include <pthread.h>
int pthread_rwlock_tryrdlock(pthread_rwlock_t *rwlock);
int pthread_rwlock_tryrwrock(pthread_rwlock_t *rwlock);
 

带有超时的读写锁

与互斥量一样，有两个带有超时的速写锁加锁函数

include <pthread.h>
include <time.h>
int pthread_rwlock_timedrdlock(pthread_rwlock_t *restrict rwlock,const struct timespec *restrict tsptr);
int pthread_rwlock_timedwrlock(pthread_rwlock_t *restrict rwlock,const struct timespec *restrict tsptr);
 


条件变量

在使用条件变量之前，必须对它进行初始化，在释放底层的内存空间之前，可以使用pthread_cond_destroy函数对条件变量进行反初始化

include <pthread.h>
int pthread_cond_init(pthread_cond_t *restrict cond,const pthread_condattr_t *restrict attr);
int pthread_cond_destroy(pthread_cond_t *cond);
条件本身是由互斥量保护的。线程在改变条件状态之前必须首先锁住互斥量，然后调用下面函数等待条件变量为真。

include <pthread.h>
int pthread_cond_wait(pthread_cond_t *restrict cond,pthread_mutex_t *restrict mutex);
int pthread_cond_timedwait(pthread_cond_t *restrict cond,pthread_mutex_t *restrict mutex,const struct timespec *restrict tsptr);
调用者把锁住的互斥量传给函数，函数自动把调用线程放到等待条件的线程列表上，对互斥量解锁。pthread_cond_wati返回时，互斥量再次被锁住。

pthread_cond_timedwait则添加了一个超时值，如果超过到期时条件还是没有出现，则函数重新获取互斥量，然后返回ETIMEDOUT。

两个函数调用成功返回时，线程需要重新计算条件，因为另一个线程可能已经在运行并改变条件。

下面函数用于通知线程条件已经满足：

include <pthread.h>
int pthread_cond_signal(pthread_cond_t *cond);
int pthread_cond_broadcast(pthread_cond_t *cond);
phread_cond_signal函数至少能唤醒一个等待该条件的线程，而pthread_cond_broadcast函数则能唤醒等待该条件的所有线程。

自旋锁

自旋锁与互斥量类似，但它不是通过休眠使进程阻塞，而是在获取锁之前一直处于忙等（自旋）阻塞状态。

自旋锁可用于以下情况：锁被持有的时间短，而且线程并不希望在重新调度上花费太多的成本。

自旋锁的接口与互斥量的接口类似，提供了以下的5个函数。

复制代码
include <pthread.h>
int pthread_spin_init(pthread_spinlock_t *lock,int pshared);
int pthread_spin_destroy(pthread_spinlock_t *lock);

int pthread_spin_lock(pthread_spinlock_t *lock);
int pthread_spin_trylock(pthread_spinlock_t *lock);
int pthread_spin_unlock(pthread_spinlock_t *lock);
屏障是用户协调多个线程并行工作的同步机制。

屏障允许每个线程等待，直到有的合作线程到达某一点，然后从该点继续执行。pthread_join函数就是一种屏障，允许一个线程等待，直到另一个线程退出。

可以使用下面函数对屏障进行初始化跟反初始化

#include <pthread.h>
int pthread_barrier_init(pthread_barrier_t *restrict barrier,const pthread_barrierattr_t *restrict attr,unsigned int count);
int pthread_barrier_destroy(pthread_barrier_t *barrier);
count参数可以用来指定在允许所有线程继续运行之前，必须到达屏障的线程数目。

可以使用pthread_barrier_wait函数来表明，线程已经完成工作，准备等所有其他线程赶上来

include <pthread.h>
int pthread_barrier_wait(pthread_barrier_t *barrier);
调用pthread_barrier_wait的线程在屏障计数（调用pthread_barrier_init时设定）未满足条件时，会进入休眠状态。

如果该线程是最后一个调用pthread_barrier_wait的线程，就满足了屏障计数，所有的线程都被唤醒。

下面给出在一个任务上合作的多个线程之间如何用屏障进行同步

 View Code
在这个实例中，使用8个线程分解了800万个数的排序工作。每个线程用堆排序算法对100万个数进行排序，然后主线程调用一个函数对这些结果进行合并。

