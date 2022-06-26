#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/epoll.h>

#define BUFLEN 128

int main(int argc, char ** argv) {
	int serverFd, clientFd;
	int len, ret, rlen;
	struct sockaddr_in serverAddr, clientAddr;

	if (argc != 2) {
		printf("Usage: %s port \n", argv[0]);
		return 0;
	}

	short port;
	port = atoi(argv[1]);
	len = sizeof(serverAddr)
	serverFd = socket(AF_INET, SOCK_STREAM, 0);
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	serverAddr.sin_addr.s_addr = 0;
	int reuse = 1;
	setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int));

	ret = bind(serverFd, (struct sockaddr *)&serverAddr, len);
	if (ret < 0) {
		perror("Failed to bind");
		return -1;
	}
	ret = listen(serverFd, 10);
	if (ret < 0) {
		perror("Failed to bind");
		return -1;
	}
	int epfd, epct, i;
	// 定义epoll事件
	struct epoll_event event;
	// 定义epoll事件集合
	struct epoll_event events[20];
	memset(events, 0, 20, *sizeof(struct epoll_event))
	epfd = epoll_create(1);
	
}