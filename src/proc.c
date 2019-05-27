/* GCache .
 * Luis Angel Trutie Ravelo, truravel@gmail.com
 * 
 * This code is licenced under the GPL.
 * 
 */

int del(struct db *mydb,char *key){
    struct element *e = &mydb[0].gchache[0];
    printf("DEL\n");

    bzero(e->key,128 );
    free( e->value );
    e->value = NULL;
    return 1;
}


int set(struct db *mydb,char *key, char *value) {
    printf("SET key:%s and value %s\n", key, value );
    store_value( &mydb[0].gchache[0] ,key, value);
    return 1;
}


char* get(struct db *mydb,char *key){
    struct element *e = &mydb[0].gchache[0];
    printf("GET key:%s \n", key );

    return (char *)e->value ;
}


void* processor(void *arg ){
    int client_id = *((int *)arg);
    char *cmd, *key, *value, response[BUFFER_LENGTH];
    char tk[4] = " \n\t";
    char tkv[2] = "\n";
    char buffer[BUFFER_LENGTH];
    int n = 0, dbptr = 0;

    while((n = recv(client_id, buffer, BUFFER_LENGTH, 0)) > 0){

        cmd = strtok( buffer, tk );
        if (strcmp(cmd, "SET") == 0) {
            key = strtok( NULL, tk );
            value = strtok( NULL, tkv );
            if( set( &mydb[dbptr], key, value) )
                sprintf(response,"OK\n");    

        } else if (strcmp(cmd, "GET") == 0){
            key = strtok( NULL, tk );
            sprintf(response,"%s\n", get(&mydb[dbptr],key) );

        } else if (strcmp(cmd, "DEL") == 0){
            if(del(&mydb[dbptr], strtok( NULL, tk ))){
                sprintf(response,"OK\n");
            }
        }else if (strcmp(cmd, "END") == 0){
            break;
        } else {
            sprintf(response,"Invalid Command.\n");
        }
        

        pthread_mutex_lock(&lock);
        send(client_id, response, strlen(response), 0);
        pthread_mutex_unlock(&lock);
        bzero(response, sizeof(BUFFER_LENGTH));
        

    }
    
    close(client_id);
    pthread_exit(NULL);

}
