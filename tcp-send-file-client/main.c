#include <stdio.h>
#include <uv.h>
#include <stdlib.h>

uv_loop_t *loop;
#define DEFAULT_PORT 7000

uv_tcp_t mysocket;

char *path = NULL;
uv_buf_t iov;
char buffer[128];

uv_fs_t read_req;
uv_fs_t open_req;
void on_read(uv_fs_t *req);
void on_write(uv_write_t* req, int status)
{
    if (status < 0) 
    {
        fprintf(stderr, "Write error: %s\n", uv_strerror(status));
    }
    else 
    {
        uv_fs_read(uv_default_loop(), &read_req, open_req.result, &iov, 1, -1, on_read);
    }
}

void on_read(uv_fs_t *req)
{
    if (req->result < 0) 
    {
        fprintf(stderr, "Read error: %s\n", uv_strerror(req->result));
    }
    else if (req->result == 0) 
    {
        uv_fs_t close_req;
        // synchronous
        uv_fs_close(uv_default_loop(), &close_req, open_req.result, NULL);
    }
    else
    {
        iov.len = req->result;
        uv_write((uv_write_t *)req,(uv_stream_t *)&mysocket,&iov,1,on_write);
    }
}

void on_open(uv_fs_t *req)
{
    if (req->result >= 0) 
    {
        iov = uv_buf_init(buffer, sizeof(buffer));
        uv_fs_read(uv_default_loop(), &read_req, req->result,&iov, 1, -1, on_read);
    }
    else 
    {
        fprintf(stderr, "error opening file: %s\n", uv_strerror((int)req->result));
    }
}

void on_connect(uv_connect_t* req, int status)
{
    if(status < 0)
    {
        fprintf(stderr,"Connection error %s\n",uv_strerror(status));

        return;
    }

    fprintf(stdout,"Connect ok\n");

    uv_fs_open(loop,&open_req,path,O_RDONLY,-1,on_open);


}

int main(int argc,char **argv)
{
    if(argc < 2)
    {
        fprintf(stderr,"Invaild argument!\n");

        exit(1);
    }
    loop = uv_default_loop();

    path = argv[1];

    uv_tcp_init(loop,&mysocket);

    struct sockaddr_in addr;

    uv_connect_t* connect = (uv_connect_t*)malloc(sizeof(uv_connect_t));


    uv_ip4_addr("192.168.65.205",DEFAULT_PORT,&addr);

    int r = uv_tcp_connect(connect,&mysocket,(const struct sockaddr *)&addr,on_connect);

    if(r)
    {
        fprintf(stderr, "connect error %s\n", uv_strerror(r));
        return 1;
    }

    return uv_run(loop,UV_RUN_DEFAULT);
}
