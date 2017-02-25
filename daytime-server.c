#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define CLR_ERR "\033[31m"
#define CLR_OK	"\033[32m"
#define CLR_DEF "\033[0m"

int sock_d;
int connect_d;

void error(const char *);
void socket_bind();

int open_socket();
void closed_socket();
void server_shutdown(int);

void gettime();


int main(){

	// catch signal Ctrl+C
	if(signal(SIGINT, server_shutdown) == SIG_ERR)
		error("can't catch signal(SIGINT)");

	// create socket
	sock_d = open_socket();

	// bind socket and listen
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(13);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	socket_bind(sock_d, addr);
	if(listen(sock_d, 10) == -1) 
		error("setup queue listen on socket");

	struct sockaddr_storage client;
	unsigned int client_size = sizeof(client);

	
	while(1){
		char buf[64];

		connect_d = accept(sock_d, (struct sockaddr*)&client, &client_size);
		if(connect_d == -1)
			error("connecing");
		
		gettime(buf, 63);
		send(connect_d, buf, strlen(buf), 0);

		close(connect_d);
	}
	
	
	close(sock_d);
	return 0;
}

void gettime(char *buf, int size){
	long int s_time;
	struct tm *m_time;

	s_time = time(NULL);
	m_time = localtime(&s_time);

	snprintf(buf, size, "%s", asctime(m_time));
}

void server_shutdown(int sig){
	closed_socket();
	fprintf(stderr, "\033[2Dserver:[%sshutdown%s]\n", CLR_OK, CLR_DEF);
	exit(0);
}

void closed_socket(){
	if(sock_d)
		close(sock_d);
	if(connect_d)
		close(connect_d);
}

void socket_bind(int sock, struct sockaddr_in addr){
	if(bind(sock, (struct sockaddr*)&addr,sizeof(addr)) == -1)
		error("bind socket with port");
}

int open_socket(){
	int sock  = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(sock_d == -1)
		error("open socket");

	int reuse = -1;
	if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char*)&reuse, sizeof(int)) == -1)
		error("setup option reuse port");

	return sock;
}

void error(const char *msg){
	fprintf(stderr, "[%serror%s]: %s\n", CLR_ERR, CLR_DEF, msg);
	exit(1);
}