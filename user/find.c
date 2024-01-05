#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

char *
fmtname(char *path)
{
    static char buf[DIRSIZ + 1];
    char *p;

    // Find first character after last slash.
    for (p = path + strlen(path); p >= path && *p != '/'; p--)
        ;
    p++;

    // Return blank-padded name.
    if (strlen(p) >= DIRSIZ)
        return p;
    memmove(buf, p, strlen(p));
    memset(buf + strlen(p), ' ', DIRSIZ - strlen(p));
    return buf;
}

void find(char *path, char *name)
{
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    if ((fd = open(path, 0)) < 0)
    {
        fprintf(2, "ls: cannot open %s\n", path);
        return;
    }

    if (fstat(fd, &st) < 0)
    {
        fprintf(2, "ls: cannot stat %s\n", path);
        close(fd);
        return;
    }

    while (read(fd, &de, sizeof(de)) == sizeof(de))
    {
        
        strcpy(buf, path);
        p = buf + strlen(buf);
        *p++ = '/';

        if (de.inum == 0)
            continue;
        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ] = 0;
        if (stat(buf, &st) < 0)
        {
            printf("ls: cannot stat %s\n", buf);
            // continue;
        }
        switch(st.type){
        case T_FILE:
            if (strcmp(de.name, name) == 0)
            {
                printf("%s\n", buf);
            }
            break;
        case T_DIR:
            if (strcmp(de.name, ".") == 0){}
            else if (strcmp(de.name, "..") == 0) {}
            else
            {
                find(buf, name);
            }
            break;
        }
    }

    close(fd);
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        // 在当前层 find
        // find(".", argv[1]);
        printf("argc 2 error\n");
        exit(1);
    }

    char *dir = argv[1];
    char *name = argv[2];
    find(dir, name);
    exit(0);
}
