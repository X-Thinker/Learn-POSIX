#include <limits.h>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include "errors.h"

#ifndef TTY_NAME_MAX
#define TTY_NAME_MAX 128
#endif

#ifndef LOGIN_NAME_MAX
#define LOGIN_NAME_MAX 32
#endif

int main(int argc, char *argv[])
{
    char login_str[LOGIN_NAME_MAX];
    char stdin_str[TTY_NAME_MAX];
    char cterm_str[L_ctermid], *cterm_str_ptr;
    int status;

    status = getlogin_r(login_str, sizeof(login_str));
    if(status != 0)
        err_abort(status, "Get login");
    cterm_str_ptr = ctermid(cterm_str);
    if(cterm_str_ptr == NULL)
        errno_abort("Get cterm");
    status = ttyname_r(0, stdin_str, sizeof(stdin_str));
    if(status != 0)
        err_abort(status, "Get stdin");
    printf("User: %s, cterm: %s, fd 0: %s\n", login_str, cterm_str, stdin_str);
    return 0;
}