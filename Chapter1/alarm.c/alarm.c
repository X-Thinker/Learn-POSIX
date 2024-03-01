#include "errors.h"

int main(int argc, char *argv[])
{
    int seconds;
    char line[128];
    char message[64];

    while(1)
    {
        printf("Alarm> ");
        //接受用户输入信息，信息错误结束程序
        if(fgets(line, sizeof(line), stdin) == NULL)
            exit(0);
        //回车继续下一次输入
        if(strlen(line) <= 1)
            continue;
        //读取输入中的时间和信息
        if(sscanf(line, "%d %64[^\n]", &seconds, message) < 2)
            fprintf(stderr, "Bad command\n");
        else 
        {
            //等待指定时间后输出内容
            sleep(seconds);
            printf("(%d) %s\n", seconds, message);
        }
    }
}
