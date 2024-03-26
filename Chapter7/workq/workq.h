#include <pthread.h>

typedef struct workq_ele_tag
{
    struct workq_ele_tag    *next;
    void                    *data;
} workq_ele_t;

typedef struct workq_tag
{
    pthread_mutex_t     mutex;
    pthread_cond_t      cv;
    pthread_attr_t      attr;
    workq_ele_t         *first, *last;
    int                 valid;
    int                 quit;
    int                 parallelism;
    int                 counter;
    int                 idle;
    void                (*engine)(void *arg);
} workq_t;

#define WORKQ_VALID     0xdec1992

extern int workq_init(workq_t *wq, int threads, void (*engine)(void*));
extern int workq_destroy(workq_t *wq);
extern int workq_add(workq_t *wq, void *data);