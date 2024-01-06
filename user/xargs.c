#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/param.h"
#include "user/user.h"
#include "kernel/fs.h"


int 
main(int argc, char *argv[])
{ 
    // char *program = argv[1];
    // char *context = argv[4];

    char buf[26];
    // 拿到上一个命令的标准输出，即 xargs 的标准输入
    // 一直读，直到拿到为止
    int n = 0;
    int buf_idx = 0;
    while((n = read(0,&buf[buf_idx],sizeof(buf)) != 0)) {
        buf_idx += n;
    }

    // 存储每一个 Xargs 的参数
    char *xargv[MAXARG];
    int xargc = 0;
    for(int i = 1; i < argc; i ++,xargc ++) {
        xargv[xargc] = argv[i];
    }

    // 读 buf，碰到 \n 执行命令
    char *p = buf;
    for(int i = 0; i < 26; i ++) {
        if(buf[i] == '\n') {

            if(fork() > 0) {
                // 跳过上一个 \0
                p = &buf[i+1];
                wait(0);
            } else {
                buf[i] = '\0';
                // 把文件地址传进去
                xargv[xargc] = p;
                xargc ++;
                xargv[xargc] = "\0";
                xargc ++;
                exec(xargv[0],xargv);
            }
        }
    
    }
    
    exit(0);
}