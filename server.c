//#include<sys/socket.h>
#include<arpa/inet.h>
#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<errno.h>
#include<stdlib.h>
#include<pthread.h>

#define LISTEN_PORT 8080
#define SOCKET_BACKLOG 1024
#define MAX_SIZE_CLIENT_MSG 4096

void* conn_handler(void *);

int main(int argc, char* argv[])
{
  int socket_fd; // file descriptor
  struct sockaddr_in server, client;

  if ((socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
  {
    puts("Failed creating socket!");
    return 1;
  }

  // Stop EADDRINUSE errors upon immediate restart
  int yup = 1;
  setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yup, sizeof(yup));

  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons(LISTEN_PORT);

  if (bind(socket_fd, (struct sockaddr *)&server, sizeof(server)) < 0)
  {
    if (errno == EADDRINUSE)
      puts("EADDRINUSE");
    else
      puts("bind() error");
    return 1;
  }

  if (listen(socket_fd, SOCKET_BACKLOG) < 0)
  {
    puts("Socket listen() failed");
    return 1;
  }

  printf("Listening on port %d...\n", LISTEN_PORT);

  int client_fd, c;

  int addrlen = sizeof(struct sockaddr_in);
  int* new_sock;

  while ((client_fd = accept(socket_fd, (struct sockaddr *)&client, (socklen_t*)&addrlen)))
  {
    puts("Connection accepted");
    pthread_t sniffer_thread;
    *new_sock = client_fd;

    if (pthread_create(&sniffer_thread, NULL, conn_handler, (void*)new_sock) < 0)
    {
        puts("Error creating thread");
        return 1;
    }
    puts("created thread");
  }


  return 0;
}

void* conn_handler(void *socket_fd)
{
  int sock = *(int*)socket_fd;
  int read_size;
  char* message, client_in[MAX_SIZE_CLIENT_MSG];

  char *pong = "PONG";
//  message = "Message 1\n";
  //write(sock, message, strlen(message));


  // message = "Type something and I shall repeat\n";
  // printf("message len = %d\n", strlen(message));
  // write(sock, message, strlen(message));
  //char *message2 = "Message n\n";
  //write(sock, message2, strlen(message2));
   while((read_size = recv(sock, client_in, MAX_SIZE_CLIENT_MSG, 0)) > 0)
   {
     if (!strcmp(client_in, "PING"))
        write(sock, pong, strlen(pong));
   }

  if (read_size == 0)
  {
    puts("Client disconnected");
    fflush(stdout);
  } else if (read_size == -1)
  {
    puts("recv failed");
  }

  close(sock);
  //free(socket_fd);// seg fault!

  return 0;
}
