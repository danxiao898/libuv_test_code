#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <uv.h>

uv_loop_t *loop;
uv_udp_t mysocket;
uv_udp_send_t send_req;
struct sockaddr_in send_addr;

typedef struct {
    uv_udp_send_t req;
    uv_buf_t buf;
} udp_send_req_t;

void alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf)
{
    buf->base = malloc(suggested_size);
    buf->len = suggested_size;
}

void on_read(uv_udp_t *req, ssize_t nread, const uv_buf_t *buf, const struct sockaddr *addr, unsigned flags)
{

    if (nread < 0)
    {
        fprintf(stderr, "Read error %s\n", uv_err_name(nread));
        uv_close((uv_handle_t *)req, NULL);
        free(buf->base);
        return;
    }

    if (nread > 0)
    {
        char sender[17] = {0};
        uv_ip4_name((const struct sockaddr_in *)addr, sender, 16);
        fprintf(stderr, "Recv from %s\n", sender);
        fprintf(stdout, "RECV:%s", buf->base);
    }
    free(buf->base);
}

void on_udp_send(uv_udp_send_t *req, int status)
{
    udp_send_req_t *ur = (udp_send_req_t*)req;
    if (status)
    {
        fprintf(stderr, "Send error %s\n", uv_strerror(status));
    }

    free(ur->buf.base);
    free(ur);

}

uv_pipe_t pipe_stdin;

void on_pipe_read(uv_stream_t *stream, ssize_t nread,const uv_buf_t *buf)
{
    if (nread < 0)
    {
        if (nread == UV_EOF)
        {
            // end of file
            uv_close((uv_handle_t *)&pipe_stdin, NULL);
        }
    }
    else if (nread > 0)
    {
        udp_send_req_t *req = (udp_send_req_t*) malloc(sizeof(udp_send_req_t));
        req->buf = uv_buf_init((char*) malloc(nread), nread);
        memcpy(req->buf.base, buf->base, nread);
        uv_udp_send((uv_udp_send_t*)req, &mysocket, &req->buf, 1, (const struct sockaddr *)&send_addr, on_udp_send);
    }

    // OK to free buffer as write_data copies it.
    if (buf->base)
        free(buf->base);
}

int main()
{
    loop = uv_default_loop();

    uv_udp_init(loop, &mysocket);
    //struct sockaddr_in broadcast_addr;
    //uv_ip4_addr("0.0.0.0", 0, &broadcast_addr);
    //uv_udp_bind(&mysocket, (const struct sockaddr *)&broadcast_addr, 0);
    uv_udp_recv_start(&mysocket, alloc_buffer, on_read);
    //uv_udp_set_broadcast(&mysocket, 1);

    uv_ip4_addr("127.0.0.1", 7000, &send_addr);
    uv_pipe_init(loop, &pipe_stdin, 0);
    uv_pipe_open(&pipe_stdin, 0);

    uv_read_start((uv_stream_t *)&pipe_stdin, alloc_buffer, on_pipe_read);

    return uv_run(loop, UV_RUN_DEFAULT);
}