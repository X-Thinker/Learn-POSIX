#include <pthread.h>
#include "errors.h"

typedef struct alarm_tag
{
    int seconds;
    char message[64];
}alarm_t;

void *alarm_thread(void *arg)
{
    alarm_t *alarm = (alarm_t*)arg;
    int status;
    //分离线程，线程例程结束后自动回收资源
    status = pthread_detach(pthread_self());
    if(status != 0)
        err_abort(status, "Detach thread");
    sleep(alarm->seconds);
    printf("(%d) %s\n", alarm->seconds, alarm->message);
    free(alarm);
    return NULL;
}

int main(int argc, char *argv[])
{
    int status;
    char line[128];
    alarm_t *alarm;
    pthread_t thread;

    while(1)
    {
        printf("Alarm> ");
        if(fgets(line, sizeof(line), stdin) == NULL)
            exit(0);
        if(strlen(line) <= 1)
            continue;
        //分配内存，不使用全局数据防止线程竞争
        alarm = (alarm_t*)malloc(sizeof(alarm_t));
        if(alarm == NULL)
            errno_abort("Allocate alarm");
        
        if(sscanf(line, "%d %64[^\n]", &alarm->seconds, alarm->message) < 2)
        {
            fprintf(stderr, "Bad command\n");
            free(alarm);
        }
        else 
        {
            //创建线程
            status = pthread_create(&thread, NULL, alarm_thread, alarm);
            if(status != 0)
                err_abort(status, "Create alarm thread");
        }
    }
}