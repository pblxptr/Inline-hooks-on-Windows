#include <WinSock2.h>
#include <stdio.h>
#include <ws2tcpip.h>

#pragma comment (lib, "Ws2_32.lib")
void error(const char* msg)
{
	printf("Error: %s\n", msg);

	getchar();

	ExitProcess(EXIT_FAILURE);
}

int main()
{
	WSADATA wsdata;
	int iResult;

	getchar();

	iResult = WSAStartup(MAKEWORD(2, 2), &wsdata);

	if (iResult != NO_ERROR) {
		error("WSAStartup");
	}

	SOCKET s;
	
	s = socket(AF_INET, SOCK_STREAM, 0);

	if (s == INVALID_SOCKET) {
		error("socket");
	}

	printf("Socket: %d\n", s);

	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;


	struct addrinfo* res = NULL;
	struct addrinfo* p = NULL;
	if (getaddrinfo("127.0.0.1", "3490", &hints, &res) != 0) {
		error("getaddrinfo");
	}

	char ipaddress[INET6_ADDRSTRLEN];
	short port;

	for (p = res ; p != NULL; p = p->ai_next) {
		void* addr = NULL;
		const char *ipver;

		if (p->ai_family == AF_INET) {
			struct sockaddr_in* ipv4 = (struct sockaddr_in *)p->ai_addr;
			addr = &(ipv4->sin_addr);
			ipver = "IPv4";
			inet_ntop(p->ai_family, addr, (PSTR)ipaddress, sizeof(ipaddress)); 
		}

		break;
	}

	if (bind(s, (SOCKADDR*)p->ai_addr, sizeof(SOCKADDR)) == SOCKET_ERROR) {
		printf("%d", GetLastError());
		error("bind");
	}

	if (listen(s, 10) != 0) {
		error("listen");
	}

	
	port = ntohs(((struct sockaddr_in*)p->ai_addr)->sin_port);


	printf("IP: %s Port: %d\n", ipaddress, port);

	SOCKET new_s;
	struct sockaddr_in client_addr;
	int client_addr_len = sizeof(sockaddr_in);

	new_s = accept(s, (struct sockaddr *)&client_addr, &client_addr_len);
	printf("accepted!\n");
	
	char client_address[INET6_ADDRSTRLEN];
	inet_ntop(client_addr.sin_family, (void *)&client_addr.sin_addr, client_address, client_addr_len);

	printf("Client IP address: %s\n", client_address);

	char msg[] = "Hello World";
	SIZE_T msg_len = strlen(msg) + 1;
	SIZE_T sent_bytes = 0;

	while (sent_bytes != msg_len) {
		sent_bytes = send(new_s, msg + sent_bytes, msg_len - sent_bytes, 0);

		if (sent_bytes == -1) {
			break;
		}
	}
	printf("Sent!\n");

	char buffer[512];
	while (recv(new_s, buffer, sizeof(buffer), 0) != 0);
	printf("Reveived\n");
	
	getchar();

	return 1;
}