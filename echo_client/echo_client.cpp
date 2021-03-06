// echo_client.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "uv.h"
#include <string>
#define BUFSIZE 1024
#define MYVERSION "0.1.0"

uv_loop_t* g_loop = nullptr;
uv_work_t uv_work;

typedef struct _send_work_context
{
	struct sockaddr_in remote_address;
	std::string type;
}send_work_context;
send_work_context g_send_work_context;


void send_cb(uv_udp_send_t* req, int status)
{
	
	if (status != 0)
	{
		printf("send_cb error status=%u\n", status);
	}
}

void alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf) {
	buf->base = (char*)malloc(suggested_size);
	buf->len = suggested_size;
}

void read_cb(uv_udp_t* handle,
	ssize_t nread, const uv_buf_t* rcvbuf, const struct sockaddr* addr, unsigned flags)
{
	if (nread < 0)
	{
		printf("read_cb error %u\n", nread);
	}
	else
	{
		rcvbuf->base[nread] = 0;
		printf("%s\n", rcvbuf->base);
	}
}

void work_send(uv_work_t* req)
{
	char buf[BUFSIZE];
	send_work_context* context = (send_work_context*)req->data;

	if (context->type == "t")
	{
		uv_tcp_t server;
		uv_tcp_init(g_loop, &server);
		uv_tcp_bind(&server, (const struct sockaddr*)&context->remote_address, 0);
		// 		int r = uv_listen((uv_stream_t*)&server, 128, on_new_connection);
		// 		if (r) {
		// 			fprintf(stdout, "Listen error %s\n", uv_strerror(r));
		// 			return 1;
		// 		}
	}
	else if (context->type == "u")
	{
		uv_udp_t server_udp;
		uv_udp_init(g_loop, &server_udp);

		memset(buf, 0, BUFSIZE);
		fgets(buf, BUFSIZE, stdin);
		uv_udp_send_t req;
		uv_buf_t uvbuf;
		uvbuf = uv_buf_init(buf, strlen(buf));
		int r = uv_udp_send(&req, &server_udp, &uvbuf, 1, (const struct sockaddr*)&context->remote_address, send_cb);

		r = uv_udp_recv_start(&server_udp, alloc_buffer, read_cb);
		if (r) 
		{
			fprintf(stdout, "Listen error %s\n", uv_strerror(r));
		}

		while (TRUE)
		{
			memset(buf, 0, BUFSIZE);
			fgets(buf, BUFSIZE, stdin);

			//		uv_ip4_addr("0.0.0.0", UV_UDP_PORT, &addr);
			//		uv_udp_bind(&server_udp, (const struct sockaddr*)&addr, 0);

			uv_udp_send_t req;
			uv_buf_t uvbuf;
			uvbuf = uv_buf_init(buf, strlen(buf));
			r = uv_udp_send(&req, &server_udp, &uvbuf, 1, (const struct sockaddr*)&context->remote_address, send_cb);
			if(r)
			{
				fprintf(stdout, "Listen error %s\n", uv_strerror(r));
			}
		}
	}
}

void work_send_end(uv_work_t* req, int status)
{

}

int main(int argc, char **argv)
{
	printf("echo client %s\n", MYVERSION);

	if (argc < 4)
	{
		printf("parameter like  u 192.168.123.123 1800");
		return 0;
	}

	std::string type = argv[1];
	std::string address = argv[2];
	UINT16	port = atoi(argv[3]);
	struct sockaddr_in remote_addr;
	g_loop = uv_default_loop();

	if (uv_ip4_addr(address.c_str(), port, &remote_addr) != 0)
	{
		printf("remote ip not valide, parameter like  u 192.168.123.123 1800");
		return 0;
	}

	memcpy(&g_send_work_context.remote_address, &remote_addr, sizeof(remote_addr));
	g_send_work_context.type = type;

	uv_work.data = (void*)&g_send_work_context;
	uv_queue_work(g_loop, &uv_work, work_send, work_send_end);

	
	
	uv_run(g_loop, UV_RUN_DEFAULT);

	//	udpServer();

	//	udp
	// 	uv_udp_t server_udp;
	// 	uv_udp_init(g_loop, &server_udp);
	// 	uv_ip4_addr("0.0.0.0", UV_UDP_PORT, &addr);
	// 	uv_udp_bind(&server_udp, (const struct sockaddr*)&addr, 0);
	// 	r = uv_udp_recv_start(&server_udp, alloc_buffer, on_server_read_end_udp);
	// 	if (r) {
	// 		fprintf(stdout, "Listen error %s\n", uv_strerror(r));
	// 		return 1;
	// 	}
}

