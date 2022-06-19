#include <stdio.h>
#include "utils.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

int configure_socket()
{
  int socket_client;
  struct sockaddr_in serv_addr;

  socket_client = socket(AF_INET, SOCK_STREAM, 0);
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(SERVER_PORT);

  inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr);
  if (connect(socket_client, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
  {
    printf("\nConnection Failed \n");
    return -1;
  }

  return socket_client;
}

int main(int argc, char **argv)
{
  int socket_client, valread;
  char *cmd = "ADD Lancelot padel lundi 13";
  char buffer[1024] = {0};
  char line[LINE_SIZE];

  while (1)
  {

    fgets(line, LINE_SIZE, stdin);
    line[strlen(line) - 1] = '\0';

    socket_client = configure_socket();

    pid_t pid = fork();

    if (pid == -1)
    {
      return -1;
    }
    if (pid == 0)
    {
      send(socket_client, line, strlen(line), 0);
      valread = read(socket_client, buffer, 1024);
      printf("%s\n", buffer);
    }
  }
  return 0;
}
