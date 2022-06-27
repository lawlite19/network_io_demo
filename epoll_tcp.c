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
	char buf[128];
	struct sockaddr_in serverAddr, clientAddr;

	if (argc != 2) {
		printf("Usage: %s port \n", argv[0]);
		return 0;
	}

	short port;
	port = atoi(argv[1]);
	len = sizeof(serverAddr);
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
	memset(events, 0, 20 * sizeof(struct epoll_event));
	// 创建epoll的fd，红黑树
	epfd = epoll_create(1);
	
	event.data.fd = serverFd;
	// 填充事件类型，监听读事件
	event.events = EPOLLIN;
	// 把serverFd(监听FD)和事件添加到红黑树上
	epoll_ctl(epfd, EPOLL_CTL_ADD, serverFd, &event);

	while(1) {
		// 等待时间到来，阻塞模式，同时监听20个事件，返回就绪的事件个数
		epct = epoll_wait(epfd, events, 20, -1);
		// 根据epoll返回的值查询事件
		for (i = 0; i < epct; i++) {
				if (events[i].data.fd == serverFd) {
					socklen_t length = sizeof(clientAddr);
					clientFd = accept(events[i].data.fd, (struct sockaddr *)&clientAddr, &length);
					printf("new fd=%d ip %s\n", clientFd, inet_ntoa(clientAddr.sin_addr));
					event.data.fd = clientFd;
					event.events = EPOLLIN | EPOLLET;
					// event.events = EPOLLIN;
					epoll_ctl(epfd, EPOLL_CTL_ADD, clientFd, &event);
				} else {
					printf("new data arrive\n");
					// 如果不是serverFd, 就是clientFd的读事件
					memset(buf, 0, BUFLEN);
					rlen = read(events[i].data.fd, buf, BUFLEN);
					if (rlen <= 0) {
						// 客户端断开
						printf("fd %d disconnected\n", events[i].data.fd);
						close(events[i].data.fd);
						epoll_ctl(epfd, EPOLL_CTL_DEL, events[i].data.fd, &event);
						continue;
					}
					printf("fd: %d data: %s\n", events[i].data.fd, buf);
				}
		}
	}
}