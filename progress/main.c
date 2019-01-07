#include <stdio.h>
#include <uv.h>
#include <stdlib.h>
#include <unistd.h>

uv_loop_t *loop;

uv_async_t async;

void print_progress(uv_async_t* handle)
{
    double percentage = *(double *)handle->data;
    printf("Downloaded:%.2f%%\n",percentage);
}

void fake_download(uv_work_t* req)
{
    int size = *((int*) req->data);
    int downloaded = 0;
    double percentage;
    while (downloaded < size) 
    {
        percentage = downloaded*100.0/size;
        async.data = (void*) &percentage;
        uv_async_send(&async);

        sleep(1);
        downloaded += (200+random())%1000; // can only download max 1000bytes/sec,
                                           // but at least a 200;
    }
}

void after(uv_work_t* req, int status)
{
    printf("download done!");

    uv_close((uv_handle_t*)&async,NULL);
}

int main(void)
{
    loop = uv_default_loop();

    uv_work_t req;

    int size = 10240;

    req.data = (void *)&size;

    uv_async_init(loop,&async,print_progress);
    uv_queue_work(loop,&req,fake_download,after);

    return uv_run(loop,UV_RUN_DEFAULT);
}



                                        