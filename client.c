#include<stdio.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<netdb.h>
#include<errno.h>
#include<time.h>

int get_host_ip(char *, char*);


int main(int argc , char *argv[])
{
    char* remote_host = "localhost";
    struct sockaddr_in server;
    int socket_desc;
    char server_reply[2048]; // accept 2kb response max

    char remote_host_ip[128];

    // 1. Lookup DNS and get the IP address
    if (get_host_ip(remote_host_ip, remote_host) < 0)
    {
      puts("Error getting IP");
      return 1;
    }

    printf("%s = %s\n", remote_host, remote_host_ip);

    // 2. Create the socket to communicate

    // af_inet = 2 = Internet Protocol
    // sock_stream = 1 = stream
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);

    if (socket_desc == -1)
    {
        puts("Could not create socket");
        return 1;
    }

    // 3. Specify connection properties and connect

    server.sin_family = AF_INET; // les internettes
    server.sin_addr.s_addr  = inet_addr("127.0.0.1");
    server.sin_port = htons(8080); // convert port num to network byte order

    puts("Connecting...");

    if (connect(socket_desc, (struct sockaddr*)&server, sizeof(server)) < 0)
    {
      puts("Connection error!");
      printf("Errno = %d\n", errno);
      return 1;
    }

    // 4. Send/receive a message

    puts("Connected! Saying hello...");
    int read_size;

    clock_t start = clock(), diff;

    for (int t=0; t< 50000; t++)
    {
      //char* message = "GET / HTTP/1.1\r\n\r\n";
      char *message = "PING";

      if (send(socket_desc, message, strlen(message), 0) < 0)
      {
        puts("Error sending message!");
        return 1;
      }

      // while((read_size = recv(socket_desc, server_reply, 2048, 0)) > 0)
      // {
      //   puts(server_reply);
      //   printf("read size = %d\n", read_size);
      // }

      if (recv(socket_desc, server_reply, 2048, 0) < 0)
      {
        puts("Error receiving data!");
        return 1;
      }

      //puts(server_reply);

      //puts(server_reply);
      //puts("Messaged received from server.");
    }

    diff = clock() - start;
    int msec = (diff * 1000) / CLOCKS_PER_SEC;
    printf("Time taken = %d milliseconds\n", msec%1000);



    close(socket_desc);

    return 0;
}

//dns lookup
int get_host_ip(char *tostring, char* hostname)
{
  //char ip[100];
  struct hostent* he;
  struct in_addr **addr_list;

  if ((he = gethostbyname(hostname)) == NULL)
  {
    herror("gethostbyname");
    return -1;
  }

  addr_list = (struct in_addr **)he->h_addr_list;

  // iterate our list of 32-bit 'long' IP addresses
  for (int i=0; addr_list[i] != NULL; i++)
  {
    // inet_ntoa() converts 32bit long in network byte order to string IP e.g.
    // "192.168.0.1"
    strcpy(tostring, inet_ntoa(*addr_list[i]));
    //printf("[%d], ip = %s\n", i, tostring);

    break; // Just accept the first IP we get
  }

  return 0;
}
