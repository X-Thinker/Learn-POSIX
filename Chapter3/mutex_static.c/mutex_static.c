#include <pthread.h>
#include <errors.h>

//声明一个结构后，静态初始化（赋值），效果相当于 pthread_mutex_init
typedef struct my_struct_tag
{
    pthread_mutex_t mutex;
    int value;
}my_struct_t;

my_struct_t data = {PTHREAD_MUTEX_INITIALIZER, 0};

int main(int argc, char *argv[])
{
    return 0;
}