#include <stdio.h>
#include <uv.h>
#include <unistd.h>

void thread1_fun(void)
{
    while (1)
    {
        sleep(1);
        printf("%s\n", __FUNCTION__);
    }
}

void thread2_fun(void)
{
    while (1)
    {
        sleep(1);
        printf("%s\n", __FUNCTION__);
    }
}

int main(int argc, char **argv)
{
    uv_thread_t thread_printf1;
    uv_thread_t thread_printf2;

    uv_thread_create(&thread_printf1, (void (*)(void *))thread1_fun, NULL);
    uv_thread_create(&thread_printf2, (void (*)(void *))thread2_fun, NULL);

    uv_thread_join(&thread_printf1);
    uv_thread_join(&thread_printf2);

    return 0;
}