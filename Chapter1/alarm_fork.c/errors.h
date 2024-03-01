#ifndef __errors_h
#define __errors_h

#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef DEBUG
# define DPRINTF(arg) printf arg
#else
# define DPRINTF(arg)
#endif

/*定义两个报错宏*/
//err_abort检测标准的Pthreads错误
#define err_abort(code, text) do { \
    fprintf(stderr, "%s at \"%s\":%d: %s\n", \
        text, __FILE__, __LINE__, strerror(code)); \
    abort(); \
    } while(0)
//errno_abort检测传统的errno错误变量方式
#define errno_abort(text) do { \
    fprintf(stderr, "%s at \"%s\":%d: %s\n", \
        text, __FILE__, __LINE__, strerror(errno)); \
    abort(); \
    } while(0)

#endif