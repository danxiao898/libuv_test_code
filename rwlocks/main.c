#include <stdio.h>
#include <uv.h>
#include <unistd.h>
#include <stdbool.h>

int shared_num;
bool run = true;

void reader(void *p)
{
    uv_rwlock_t *lock = (uv_rwlock_t*)p;
    while(run)
    {
        uv_rwlock_rdlock(lock);
        sleep(1);
        printf("%s,%s,%u\n",__FUNCTION__,"lock",(unsigned int)pthread_self());
        printf("%s:%d\n",__FUNCTION__,shared_num);
        uv_rwlock_rdunlock(lock);
        printf("%s,%s,%u\n",__FUNCTION__,"unlock",(unsigned int)pthread_self());
    }

    printf("%s.....end.....\n",__FUNCTION__);
}

void writer(void *p)
{
    uv_rwlock_t *lock = (uv_rwlock_t*)p;
    while(run)
    {
        uv_rwlock_wrlock(lock);
        shared_num++;
        printf("%s:%d\n",__FUNCTION__,shared_num);
        uv_rwlock_wrunlock(lock);
    }
    printf("%s.....end.....\n",__FUNCTION__);
}

void end(void)
{
    char ch;
    while(1)
    {
        ch = getchar();
        if(ch == 'q')
            run = false;
    }
    
}

int main(void)
{
    uv_thread_t read_thread[2];
    uv_thread_t write_thread;
    uv_thread_t end_thread;

    uv_rwlock_t numlock;
    uv_rwlock_init(&numlock);

    for(int i = 0;i < 2;i++)
    {
        uv_thread_create(read_thread+i,(void (*)(void*))reader,(void *)&numlock);
    }
    uv_thread_create(&write_thread,(void (*)(void*))writer,(void *)&numlock);

    uv_thread_create(&end_thread,(void (*)(void*))end,NULL);

    for(int i = 0;i < 2;i++)
    {
        uv_thread_join(read_thread+i);
    }
    uv_thread_join(&write_thread);

    printf("%s\n","Bye,Bye");

    return 0;
}