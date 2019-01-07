#include <stdio.h>
#include <uv.h>
#include <unistd.h>

int counter = 0;

void thread1_fun(void *mutex_lock_print)
{
    uv_mutex_t *lock = mutex_lock_print;
    while (1)
    {
        uv_mutex_lock(lock);
        sleep(1);
        printf("%s:%d\n", __FUNCTION__,counter);
        counter++;
        uv_mutex_unlock(lock);
        sleep(1);
    }
}

void thread2_fun(void *mutex_lock_print)
{
    uv_mutex_t *lock = mutex_lock_print;
    while (1)
    {
        uv_mutex_lock(lock);
        sleep(1);
        printf("%s:%d\n", __FUNCTION__,counter);
        counter++;
        uv_mutex_unlock(lock);
        sleep(1);
    }
}

int main(int argc, char **argv)
{
    uv_thread_t thread_printf1;
    uv_thread_t thread_printf2;
    uv_mutex_t mutex_lock_print;

    uv_mutex_init(&mutex_lock_print);

    uv_thread_create(&thread_printf1, (void (*)(void *))thread1_fun, (void*)&mutex_lock_print);
    uv_thread_create(&thread_printf2, (void (*)(void *))thread2_fun, (void*)&mutex_lock_print);

    uv_thread_join(&thread_printf1);
    uv_thread_join(&thread_printf2);
    uv_mutex_destroy(&mutex_lock_print);

    return 0;
}