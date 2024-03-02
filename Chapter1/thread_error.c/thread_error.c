#include <pthread.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

int main(int argc, char *argv[])
{
    pthread_t thread;
    int status;

    //尝试等待一个未初始化的线程ID，大多数情况下返回错误代码ESRCH
    //有一种极小的可能是未初始化线程ID为初始线程
    //如果Pthreads实现了自死锁则返回错误代码EDEADLK，否则自死锁
    status = pthread_join(thread, NULL);
    if(status != 0)
        fprintf(stderr, "error %d: %s\n", status, strerror(status));
    return status;
}