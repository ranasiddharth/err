#include <bits/stdc++.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <iostream>
 
using namespace std;
 
#define MAXLINE 1024
#define PORT 8080
#define TIMEOUT 2
#define MAX_ATTEMPTS 5
 
int sockfd, server_fd;

string take_input() {
    cout<<"Type message: ";
    string msg;
    getline(cin,msg);
    return msg;
}
 
int send_msg(string msg, int data_no) {
    msg+=((char)data_no);
    char buffer[MAXLINE] = { 0 };
    strcpy(buffer, msg.c_str());
    int n = send(sockfd, buffer, msg.length(), 0);
    if(n==-1) {
        cout<<"Error sending msg\n";
        return -1;
    }
    return 0;
}
 
/// Return sequence number, or -1 for errors, or 0 for no message recieved
int rcv_ack(int data_no) {
    char buffer[MAXLINE] = { 0 };
    //check for data_no == ack_no here
    int len = recvfrom(sockfd, buffer, MAXLINE, 0, NULL, NULL);
 
    if(len == -1) {
        if(errno == EWOULDBLOCK || errno == EAGAIN) {
            cout<<"Timed out\n";
        } else cout<<"Couldn't recieve due to some error\n";
        return -1;
    }
    int ack_no = buffer[len-1];
    buffer[len-1] = '\0';
    printf("Recieved acknowledgement no: %d \n", ack_no);
    return ack_no;
}
 
struct timeval tv = {
    .tv_sec = TIMEOUT
};
 
int main() {
 
    struct sockaddr_in serv_addr;
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        cout<<"Socket creation error\n";
        return -1;
    } else cout<<"Socket created\n";
 
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    // allocate some buffer space, time control, and in the field of broadcasting.”
 
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
 
    // Convert IPv4 and IPv6 addresses from text to binary
    // form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)
        <= 0) {
        printf(
            "\nInvalid address/ Address not supported \n");
        return -1;
    }
    if ((server_fd
         = connect(sockfd, (struct sockaddr*)&serv_addr,
                   sizeof(serv_addr)))
        < 0) {
        printf("\nConnection Failed \n");
        return -1;
    } else cout<<"Connected to server\n";
 
    int status, attempt, ack_no;
    string msg;
    for(int data_no = 1; ; data_no++) {
        if(data_no>255) data_no = 0;
        msg = take_input();
        if(msg == "exit") goto exit;
        for(attempt=0; attempt<MAX_ATTEMPTS; attempt++) {
            cout<<"Attempt number "<<attempt<<'\n';
            status = send_msg(msg, data_no);
            if(status == -1) goto exit;
            cout<<"Message sent, waiting for acknowledgement\n";
            ack_no = rcv_ack(data_no);
            if(ack_no == data_no) break;
        }
        if(attempt == MAX_ATTEMPTS) {
            cout<<"Couldn't send message!\n";
        }
    }
    exit:
    cout<<"Exiting\n";
	close(sockfd);
	return 0;
}
