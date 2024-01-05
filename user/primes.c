#include <kernel/types.h>
#include <kernel/stat.h>
#include <user/user.h>

int is_prime(int x) {
    for(int i = 2; i <= x/i; i ++) {
        if(x%i == 0) return 0;
    }
    return 1;
}
int 
main(int argc, char *argv[])
{
    
    int p_child[2];
    int p_parent[2];
    char buffer[32]; // 四个字节
    
    pipe(p_child);
    pipe(p_parent);

    if(fork() > 0) { // parents
        close(p_child[0]); 
        close(p_parent[1]); 

        for(int i = 2; i <= 35; i ++) {
            buffer[0] = i;
            write(p_child[1],buffer,1);
        }
        close(p_child[1]); 

        while(read(p_parent[0],buffer,1) != 0){
            printf("prime %d\n", (int)buffer[0]);
        }
        close(p_parent[0]);
        wait(0); // 等待所有子进程都完成
        exit(0); // 好习惯，最后return 0；

    } else { // child
        close(p_child[1]);
        close(p_parent[0]);

        while(read(p_child[0],buffer,1) != 0) {
            int n = (int)buffer[0];
            if(is_prime(n) == 1) {
                if(fork() == 0) {
                    close(p_parent[0]); 
                    write(p_parent[1],buffer,1);
                    close(p_parent[1]);
                    exit(0);
                } 
                wait(0); // 等待child 的子进程执行完毕。
            }
            if(n == 35) break;
        }
        close(p_child[0]);
        close(p_parent[1]);
        exit(0);
    }
   
}