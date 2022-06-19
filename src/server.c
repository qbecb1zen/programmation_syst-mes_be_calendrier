#include <string.h>
#include "utils.h"
#include <sys/wait.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>

char *bdd_bin = "./output/bdd";

//On prépare les arguments qui seront envoyés à bdd
//ADD toto poney lundi 3 -> { "./bdd", "ADD", "toto", "poney", lundi", "3", NULL }
char **parse(char *line)
{
  char **res = malloc(7 * sizeof(char *));
  res[0] = bdd_bin;

  char *arg1 = strtok(line, " ");
  res[1] = arg1;

  char *arg2 = strtok(NULL, " ");
  res[2] = arg2;
  if (arg2 == NULL)
  {
    return res;
  }

  char *arg3 = strtok(NULL, " ");
  res[3] = arg3;
  if (arg3 == NULL)
  {
    arg2[strlen(arg2) - 1] = '\0';
    return res;
  }

  char *arg4 = strtok(NULL, " ");
  res[4] = arg4;
  if (arg4 == NULL)
  {
    arg3[strlen(arg3) - 1] = '\0';
    return res;
  }

  char *arg5 = strtok(NULL, "\n");
  res[5] = arg5;
  res[6] = NULL;
  return res;
}

//Configuration de la socket réseau, retourne le file descriptor de la socket
int configure_socket()
{
  // creation of the socket
  int socket_desc = socket(AF_INET, SOCK_STREAM, 0);

  struct sockaddr_in address;

  setsockopt(socket_desc, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int));
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(SERVER_PORT);
  // bind the socket to connect to client
  // Forcefully attaching socket to the port 8080
  if (bind(socket_desc, (struct sockaddr *)&address,
           sizeof(address)) < 0)
  {
    perror("bind failed");
    exit(EXIT_FAILURE);
  }
  // listen to sockets connexions
  if (listen(socket_desc, 3) < 0)
  {
    perror("listen");
    exit(EXIT_FAILURE);
  }
  return socket_desc;
}

// Passage des commandes à la base de données par un pipe
// Renvoi des réponses au client par la socket réseau
int process_communication(int socket_desc)
{
  int new_socket, valread;
  char buffer[1024] = {0};
  struct sockaddr_in address;
  char *hello = "Hello from server";
  int addrlen = sizeof(address);
  int is_command_ok;
  if ((new_socket = accept(socket_desc, (struct sockaddr *)&address,
                           (socklen_t *)&addrlen)) < 0)
  {
    perror("accept");
    exit(EXIT_FAILURE);
  }
  valread = read(new_socket, buffer, 1024);

  pid_t pid = fork();
  if (pid == -1)
  {
    return -1;
  }
  if (pid == 0)
  {
    is_command_ok = execv(bdd_bin, parse(buffer));
    send(new_socket, hello, strlen(hello), 0);
    return 0;
  }
  return 0;
}

int main(int argc, char **argv)
{
  int socket_desc, new_socket, valread;
  char line[LINE_SIZE];
  FILE *socket_file;
  char buffer[1024] = {0};
  // Configuration de la socket serveur
  socket_desc = configure_socket();

  while (1)
  {
    process_communication(socket_desc);
  }

  /*end hide*/
  return 0;
}