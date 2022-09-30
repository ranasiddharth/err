// Server side C/C++ program to demonstrate Socket
// programming
#include <bits/stdc++.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
 
using namespace std;
#define PORT 8080
#define MAXLINE 1024
 
int server_fd, client_fd;
 
/// Return sequence number, or -1 for errors, or 0 for no message recieved
int read_msg() {
    char buffer[MAXLINE] = { 0 };
    int len = recvfrom(client_fd, buffer, MAXLINE, 0, NULL, NULL);
    if(len == -1) {
        cout<<"Couldn't recv\n";
        return -1;
    } else if (len == 0) {
        cout<<"Client closed the socket\n";
        return 0;
    }
	int data_no = buffer[len-1];
	buffer[len-1] = '\0';
	printf("Message from client: %s\n", buffer);
    return data_no;
}
 
 
int send_ack(int data_no) {
    char buffer[1] = { data_no };
    int n = send(client_fd, buffer, 1, 0);
    if(n==-1) {
        cout<<"Error sending msg\n";
        return -1;
    }
    printf("Acknowledgement sent for data_no: %d\n", data_no);
    return 0;
}
 
int main(int argc, char const* argv[])
{
	struct sockaddr_in address;
	int opt = 1;
	int addrlen = sizeof(address);
	char buffer[MAXLINE] = { 0 };
	char* hello = "packet recieved";
 
	// Creating socket file descriptor
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0))
		== 0) {
		perror("socket failed");
		exit(EXIT_FAILURE);
	}
 
	// Forcefully attaching socket to the port 8080
	if (setsockopt(server_fd, SOL_SOCKET,
				SO_REUSEADDR | SO_REUSEPORT, &opt,
				sizeof(opt))) {
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);
 
	// Forcefully attaching socket to the port 8080
	if (bind(server_fd, (struct sockaddr*)&address,
			sizeof(address))
		< 0) {
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	if (listen(server_fd, 3) < 0) {
		perror("listen");
		exit(EXIT_FAILURE);
	}
	cout<<"server started and listening\n";
	if ((client_fd
		= accept(server_fd, (struct sockaddr*)&address,
				(socklen_t*)&addrlen))
		< 0) {
		perror("accept");
		exit(EXIT_FAILURE);
	}
    cout<<"client accepted\n";
    while(true) {
		int data_no = read_msg();
		if(data_no == 0 || data_no == -1) break;
		int status = send_ack(data_no);
		if(status == -1) break;
    }
 
// closing the connected socket
	close(client_fd);
// closing the listening socket
	shutdown(server_fd, SHUT_RDWR);
	return 0;
}
