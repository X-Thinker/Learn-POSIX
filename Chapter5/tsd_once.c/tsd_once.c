#include <pthread.h>
#include "errors.h"

typedef struct tsd_tag
{
    pthread_t   thread_id;
    char        *string;
} tsd_t;

pthread_key_t tsd_key;
pthread_once_t key_once = PTHREAD_ONCE_INIT;

void once_routine(void)
{
    int status;

    printf("initializing key\n");
    status = pthread_key_create(&tsd_key, NULL);
    if(status != 0)
        err_abort(status, "Create key");
}

void *thread_routine(void *arg)
{
    tsd_t *value;
    int status;
    
    //保证key只被初始化一次
    status = pthread_once(&key_once, once_routine);
    if(status != 0)
        err_abort(status, "Once init");

    value = (tsd_t*)malloc(sizeof(tsd_t));
    if(value == NULL)
        errno_abort("Allocate key value");

    status = pthread_setspecific(tsd_key, value);
    if(status != 0)
        err_abort(status, "Set tsd");

    printf("%s set tsd value %p\n", arg, value);
    
    value->thread_id = pthread_self();
    value->string = (char*)arg;
    value = (tsd_t*)pthread_getspecific(tsd_key);
    
    printf("%s starting...\n", value->string);
    sleep(2);

    value = (tsd_t*)pthread_getspecific(tsd_key);
    printf("%s done...\n", value->string);
    return NULL;
}

void main(int argc, char *argv[])
{
    pthread_t thread1, thread2;
    int status;

    status = pthread_create(&thread1, NULL, thread_routine, "thread 1");
    if(status != 0)
        err_abort(status, "Create thread 1");
    status = pthread_create(&thread2, NULL, thread_routine, "thread 2");
    if(status != 0)
        err_abort(status, "Create thread 2");

    pthread_exit(NULL);
}