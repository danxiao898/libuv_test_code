#include <stdio.h>
#include <uv.h>
#include <stdlib.h>
#include <unistd.h>

uv_loop_t *loop;
#define FIB_UNTIL 25
uv_work_t req[FIB_UNTIL];


long fib_(long t) 
{
    if (t == 0 || t == 1)
        return 1;
    else
        return fib_(t-1) + fib_(t-2);
}
void after_fib(uv_work_t *req,int status)
{
    if (status == UV_ECANCELED)
        fprintf(stderr, "Calculation of %d cancelled.\n", *(int *) req->data);
    //else
        //fprintf(stderr, "Done calculating %dth fibonacci\n", *(int *) req->data);
}

void fib(uv_work_t *req)
{
    int n = *(int*)(req->data);

    if(random() %2 == 0)
        sleep(1);
    else
        sleep(3);
    
    long fib = fib_(n);
    fprintf(stderr, "%dth fibonacci is %lu\n", n, fib);
}

void signal_hand(uv_signal_t* handle, int signum)
{
    if(signum == SIGINT)
        fprintf(stdout,"------CTRL+C------\n");
    
    for(int i=0;i<FIB_UNTIL;i++)
    {
        uv_cancel((uv_req_t*)req+i);
    }

    uv_signal_stop(handle);
}

int main(void)
{
    loop = uv_default_loop();

    int data[FIB_UNTIL];

    for(int i=0;i<FIB_UNTIL;i++)
    {
        data[i] = i;
        req[i].data = (void*)(data+i);
        uv_queue_work(loop,req+i,fib,after_fib);
    }

    uv_signal_t sig;
    uv_signal_init(loop,&sig);
    uv_signal_start(&sig,signal_hand,SIGINT);

    return uv_run(loop,UV_RUN_DEFAULT);
}
