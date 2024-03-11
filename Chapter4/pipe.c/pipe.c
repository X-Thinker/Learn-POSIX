#include <pthread.h>
#include "errors.h"

typedef struct stage_tag
{
    pthread_mutex_t mutex;
    pthread_cond_t avail;
    pthread_cond_t ready;
    int data_ready;
    long data;
    pthread_t thread;
    struct stage_tag *next;
} stage_t;

typedef struct pipe_tag
{
    pthread_mutex_t mutex;
    stage_t *head;
    stage_t *tail;
    int stages;
    int active;
} pipe_t;

int pipe_send(stage_t *stage, long data)
{
    int status;

    status = pthread_mutex_lock(&stage->mutex);
    if(status != 0)
        return status;

    //当有新数据进入但是内部还有数据未处理时，阻塞等待数据处理结束
    while(stage->data_ready)
    {
        status = pthread_cond_wait(&stage->ready, &stage->mutex);
        if(status != 0)
        {
            pthread_mutex_unlock(&stage->mutex);
            return status;
        }
    }

    //处理数据，并通知数据可以用
    stage->data = data;
    stage->data_ready = 1;
    //通知线程有可处理数据
    status = pthread_cond_signal(&stage->avail);
    if(status != 0)
    {
        pthread_mutex_unlock(&stage->mutex);
        return status;
    }
    status = pthread_mutex_unlock(&stage->mutex);
    return status;
}

//流水线线程启动函数
void *pipe_stage(void *arg)
{
    stage_t *stage = (stage_t*)arg;
    stage_t *next_stage = stage->next;
    int status;

    status = pthread_mutex_lock(&stage->mutex);
    if(status != 0)
        err_abort(status, "Lock pipe stage");
    
    while (1)
    {
        //数据未到来时，阻塞等待数据到来
        while(stage->data_ready != 1)
        {
            status = pthread_cond_wait(&stage->avail, &stage->mutex);
            if(status != 0)
                err_abort(status ,"Wait for previous stage");
        }
        //将数据+1推入下一个流水线线程
        pipe_send(next_stage, stage->data + 1);
        stage->data_ready = 0;
        status = pthread_cond_signal(&stage->ready);
        if(status != 0)
            err_abort(status, "Wake next stage");
    }
    
}

//创建流水线，stages参数为流水线长度，pipe管理流水线
int pipe_create(pipe_t *pipe, int stages)
{
    int pipe_index;
    stage_t **link = &pipe->head, *new_stage, *stage;
    int status;

    status = pthread_mutex_init(&pipe->mutex, NULL);
    if(status != 0)
        err_abort(status, "Init pipe mutex");
    pipe->stages = stages;
    pipe->active = 0;

    for(pipe_index = 0; pipe_index <= stages; pipe_index++)
    {
        new_stage = (stage_t*)malloc(sizeof(stage_t));
        if(new_stage == NULL)
            errno_abort("Allocate stage");
        status = pthread_mutex_init(&new_stage->mutex, NULL);
        if(status != 0)
            err_abort(status, "Init stage mutex");
        status = pthread_cond_init(&new_stage->avail, NULL);
        if(status != 0)
            err_abort(status, "Init avail condition");
        status = pthread_cond_init(&new_stage->ready, NULL);
        if(status != 0)
            err_abort(status, "Init ready condition");
        new_stage->data_ready = 0;
        //尾接法连接新线程
        *link = new_stage;
        link = &new_stage->next;
    }

    //加上NULL尾部
    *link = (stage_t*)NULL;
    pipe->tail = new_stage;

    //创建流水线线程
    for(stage = pipe->head; stage->next != NULL; stage = stage->next)
    {
        status = pthread_create(&stage->thread, NULL, pipe_stage, (void*)stage);
        if(status != 0)
            err_abort(status, "Create pipe stage");
    }

    return 0;
}

//将从stdin获得的数字推入流水线开始工作
int pipe_start(pipe_t *pipe, long value)
{
    int status;

    status = pthread_mutex_lock(&pipe->mutex);
    if(status != 0)
        err_abort(status, "Lock pipe mutex");
    pipe->active++;
    status = pthread_mutex_unlock(&pipe->mutex);
    if(status != 0)
        err_abort(status, "Unlock pipe mutex");
    //推入数据
    pipe_send(pipe->head, value);
    return 0;
}

//获取流水线结果
int pipe_result(pipe_t *pipe, long *result)
{
    stage_t *tail = pipe->tail;
    long value;
    int empty = 0;
    int status;

    status = pthread_mutex_lock(&pipe->mutex);
    if(status != 0)
        err_abort(status, "Lock pipe mutex");
    if(pipe->active <= 0)
        empty = 1;
    else   
        pipe->active--;

    status = pthread_mutex_unlock(&pipe->mutex);
    if(status != 0)
        err_abort(status, "Unlock pipe mutex");
    if(empty)
        return 0;

    //锁住尾部互斥量，阻塞等待数据到来
    pthread_mutex_lock(&tail->mutex);
    while (!tail->data_ready)
        pthread_cond_wait(&tail->avail, &tail->mutex);
    
    *result = tail->data;
    tail->data_ready = 0;
    pthread_cond_signal(&tail->ready);
    pthread_mutex_unlock(&tail->mutex);
    return 1;
    
}

int main(int argc, char *argv[])
{
    pipe_t my_pipe;
    long value, result;
    int status;
    char line[128];

    pipe_create(&my_pipe, 10);
    printf("Enter integer values, or \"=\" for next result\n");

    while(1)
    {
        printf("Data> ");
        if(fgets(line, sizeof(line), stdin) == NULL)
            exit(0);
        if(strlen(line) <= 1)
            continue;
        //输入=符号，取出结果（如果有结果的话，无结果输出空）   
        if(strlen(line) <= 2 && line[0] == '=')
        {
            if(pipe_result(&my_pipe, &result))
                printf("Result is %ld\n", result);
            else 
                printf("Pipe is empty\n");
        }
        //输入数字则将数字推入流水线
        else
        {
            if(sscanf(line, "%ld", &value) < 1)
                fprintf(stderr, "Enter an integer value\n");
            else 
                pipe_start(&my_pipe, value);
        }
    }
}