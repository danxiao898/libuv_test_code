#include <stdio.h>
#include <uv.h>
#include <unistd.h>

uv_once_t once_only = UV_ONCE_INIT;

int shar_num;

void fun(void)
{
    sleep(10);
    shar_num++;
}

void run(void)
{
    uv_once(&once_only,fun);
    printf("%u,%s\n",(unsigned int)pthread_self(),"end");
}

int main(void)
{
    uv_thread_t worker[3];

    for(int i=0;i<3;i++)
    {
        uv_thread_create(worker+i,(void (*)(void *))run,NULL);
    }

    for(int i=0;i<3;i++)
    {
        uv_thread_join(worker+i);
    }

    printf("%d\n",shar_num);
    

    return 0;
}