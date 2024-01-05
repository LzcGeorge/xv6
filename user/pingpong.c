#include <kernel/types.h>
#include <kernel/stat.h>
#include <user/user.h>

int 
main(int argc, char *argv[])
{
    int p_child[2];
    int p_parent[2];
    char buffer[8]; // 一个字节
    pipe(p_child);
    pipe(p_parent);
    // p[0] is read,p[1] is write
    
    int pid = fork();

    if(pid == 0) { // child
   
        // close(p[1]);
        read(p_child[0],buffer,8);
        printf("%d: received ping\n",getpid());
        close(p_child[0]);

        // write the byte to the parents
        write(p_parent[1], "child", sizeof("child"));
        close(p_parent[1]);
        exit(0);
    } else { // parents
        // write the byte to the child
        write(p_child[1], "parent", sizeof("parent"));
        close(p_child[1]);

        read(p_parent[0],buffer,8);
        wait(0);
        printf("%d: received pong\n",getpid());
        
        close(p_parent[0]);
        exit(0);
    }
   
}