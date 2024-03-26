#include <pthread.h>

typedef struct rwlock_tag
{
    pthread_mutex_t     mutex;
    pthread_cond_t      read;
    pthread_cond_t      write;
    int                 valid;
    int                 r_active;
    int                 w_active;
    int                 r_wait;
    int                 w_wait;
} rwlock_t;

#define RWLOCK_VALID    0xfacade

#define RWL_INITIALIZER {PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER, PTHREAD_COND_INITIALIZER, RWLOCK_VALID, 0, 0, 0, 0}

extern int rwl_init(rwlock_t *rwlock);
extern int rwl_destroy(rwlock_t *rwlock);

extern int rwl_readlock(rwlock_t *rwlock);
extern int rwl_readtrylock(rwlock_t *rwlock);
extern int rwl_readunlock(rwlock_t *rwlock);

extern int rwl_writelock(rwlock_t *rwlock);
extern int rwl_writetrylock(rwlock_t *rwlock);
extern int rwl_writeunlock(rwlock_t *rwlock);