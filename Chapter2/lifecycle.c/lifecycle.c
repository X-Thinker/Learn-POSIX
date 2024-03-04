#include <pthread.h>
#include "errors.h"

void *thread_routine(void *arg)
{
    return arg;
}

int main(int argc, char *argv[])
{
    pthread_t thread_id;
    void *thread_result;
    int status;

    status = pthread_create(&thread_id, NULL, thread_routine, NULL);
    if(status != 0)
        err_abort(status, "Create thread");

    //主线程等待新建线程结束，返回值存放在 thread_result 中
    status = pthread_join(thread_id, &thread_result);
    if(status != 0)
        err_abort(status, "Join thread");
    if(thread_result == NULL)
        return 0;
    else 
        return 1;
}