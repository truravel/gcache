/* GCache .
 * Luis Angel Trutie Ravelo, truravel@gmail.com
 * 
 * This code is licenced under the GPL.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <pthread.h>

#define PORT 3990
#define BUFFER_LENGTH 2048
#define MAX_PROCESS 50

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

struct element{
    unsigned int id;
    char key[128];
    void *value;
};

struct db {
    unsigned long count;
    struct element *gchache;
};

struct db mydb[1];

#include "db.c"
#include "proc.c"


int main(){

	int sock_fd,client_sock,ret;
	struct sockaddr_in server,client;
	socklen_t addr_size;


	sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(sock_fd < 0){
		printf("Connection Error\n");
		exit(1);
	}

	memset(&server, '\0', sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(PORT);
	server.sin_addr.s_addr = inet_addr("127.0.0.1");

	ret = bind(sock_fd, (struct sockaddr*)&server, sizeof(server));
	if(ret < 0){
		printf("Error in binding.\n");
		exit(1);
	}
	
	if(listen(sock_fd, MAX_PROCESS) != 0){
		printf("Listen Error.\n");
        exit(1);
	}

    if(create_elements(0))   
        printf("Memory Error\n");
 
    printf("GCache Server Ready %ld\n", sizeof(struct element) );
    int i = 0;
    pthread_t tid[MAX_PROCESS];
    

	while(1){
		client_sock = accept(sock_fd, (struct sockaddr*)&client, &addr_size);
		if(client_sock < 0) 
            exit(1);

        if( pthread_create(&tid[i], NULL, processor, &client_sock) != 0 )
           printf("Failed to create thread\n");
        if( i >= MAX_PROCESS){
          i = 0;
          while(i < MAX_PROCESS){
            pthread_join(tid[i++],NULL);
          }
          i = 0;
        }

	}
    close(sock_fd);
	return 0;
}