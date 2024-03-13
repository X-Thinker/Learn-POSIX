#include <pthread.h>
#include "errors.h"

#define SIZE 10

static int matrixa[SIZE][SIZE];
static int matrixb[SIZE][SIZE];
static int matrixc[SIZE][SIZE];

void *thread_routine(void *arg)
{
    int cancel_type, status;
    int i, j, k, value = 1;

    for(i = 0; i < SIZE; i++)
        for(j = 0; j < SIZE; j++)
            matrixa[i][j] = i, matrixb[i][j] = j;

    while(1)
    {
        //设置为允许异步取消
        status = pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, &cancel_type);
        if(status != 0)
            err_abort(status, "Set cancel type");

        //矩阵乘法
        for(i = 0; i < SIZE; i++)
            for(j = 0; j < SIZE; j++)
            {
                matrixc[i][j] = 0;
                for(k = 0; k < SIZE; k++)
                    matrixc[i][j] += matrixa[i][k] * matrixb[k][j];
            }

        status = pthread_setcanceltype(cancel_type, &cancel_type);
        if(status != 0)
            err_abort(status, "Set cancel type");

        //更新计算结果
        for(i = 0; i < SIZE; i++)
            for(j = 0; j < SIZE; j++)
                matrixa[i][j] = matrixc[i][j];
    }
}

int main(int argc, char *argv[])
{
    pthread_t thread_id;
    void *result;
    int status;

#ifdef sun
    DPRINTF(("Setting concurrency level to 2\n"));
    thr_setconcurrency(2);
#endif

    status = pthread_create(&thread_id, NULL, thread_routine, NULL);
    if(status != 0)
        err_abort(status, "Create thread");
    
    sleep(1);

    status = pthread_cancel(thread_id);
    if(status != 0)
        err_abort(status, "Cancel thread");
    status = pthread_join(thread_id, &result);
    if(status != 0)
        err_abort(status, "Join thread");
    
    if(result == PTHREAD_CANCELED)
        printf("Thread canceled\n");
    else
        printf("Thread was not canceled\n");

    return 0;
}