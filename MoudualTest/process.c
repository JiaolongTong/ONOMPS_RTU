#include <stdio.h>
    #include <stdlib.h>
    #include <unistd.h>
    #include <string.h>
    #include <sys/types.h>
    #include <dirent.h>
    #include <ctype.h>
    #include <errno.h>
    char *basename(const char *path)
    {
        register const char *s;
        register const char *p;

        p = s = path;

        while (*s) {
            if (*s++ == '/') {
                p = s;
            }
        }

        return (char *) p;
    }

     /* 根据进程名称获取PID, 比较 base name of pid_name
      * pid_list: 获取PID列表
      * list_size: 获取PID列表长度
      * RETURN值说明:
      *              < 0: 
      *              >=0: 发现多少PID, pid_list 将保存已发现的PID
      */
    int get_pid_by_name(const char* process_name, pid_t pid_list[], int list_size)
    {
    #define  MAX_BUF_SIZE       256

        DIR *dir;
        struct dirent *next;
        int count=0;
        pid_t pid;
        FILE *fp;
        char *base_pname = NULL;
        char *base_fname = NULL;
        char cmdline[MAX_BUF_SIZE];
        char path[MAX_BUF_SIZE];

        if(process_name == NULL || pid_list == NULL)
            return -EINVAL;

        base_pname = basename(process_name);
        if(strlen(base_pname) <= 0)
            return -EINVAL;

        dir = opendir("/proc");
        if (!dir)
        {
            return -EIO;
        }
        while ((next = readdir(dir)) != NULL) {
            /* skip non-number */
            if (!isdigit(*next->d_name))
                continue;

            pid = strtol(next->d_name, NULL, 0);
            sprintf(path, "/proc/%u/cmdline", pid);
            fp = fopen(path, "r");
            if(fp == NULL)
                continue;

            memset(cmdline, 0, sizeof(cmdline));
            if(fread(cmdline, MAX_BUF_SIZE - 1, 1, fp) < 0){
                fclose(fp);
                continue;
            }
            fclose(fp);
            base_fname = basename(cmdline);

            if (strcmp(base_fname, base_pname) == 0 )
            {
                if(count >= list_size){
                    break;
                }else{
                    pid_list[count] = pid;
                    count++;
                }
            }
        }
        closedir(dir) ;
        return count;
    }

    /* 如果进程已经存在, return true */
    int is_process_exist(const char* process_name)
    {
        pid_t pid;

        return (get_pid_by_name(process_name, &pid, 1) > 0);
    }
