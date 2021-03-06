/* GCache .
 * Luis Angel Trutie Ravelo, truravel@gmail.com
 * 
 * This code is licenced under the GPL.
 * 
 */

int del(struct db *mdb,  char *key){
    struct element *e =  search(mdb,  key); 
    printf("DEL\n");

    bzero(e->key,40 );
    if(e->value != NULL) free( e->value );
    e->value = NULL;
    free(e);
    return 1;
}


int set(struct db *mdb, char *key, char *value) {
    printf("SET key:%s and value %s\n", key, value );
    int i;
    struct element *e = search(mdb, key);

    if( e->value == NULL ) {
        printf("Creando nuevo\n");
        for (i = 0; i < MAX_ELEMENTS; i++) {
            if(  mdb->gchache[i].value == NULL  )    {
                    store_value( &mdb->gchache[i] ,key, value);
                    break;
            }
        }
    } else {
        free( e->value );
        store_value( e ,key, value);
    }
    free(e);
    return 1;
}


char* get(struct db *mdb, char *key){
    struct element *e = search(mdb, key);
    free(e);
    return (char *)e->value ;
}


char * hash(char *key){
    int i = 0;
    unsigned char temp[SHA_DIGEST_LENGTH];
    //char buf[SHA_DIGEST_LENGTH*2];
    char *buf =(char *) malloc( SHA_DIGEST_LENGTH*2*sizeof(char));

    memset(buf, 0x0, SHA_DIGEST_LENGTH*2);
    memset(temp, 0x0, SHA_DIGEST_LENGTH);
    SHA1((unsigned char *)key , strlen(key), temp);
 
    for (i=0; i < SHA_DIGEST_LENGTH; i++) {
        sprintf((char*)&(buf[i*2]), "%02x", temp[i]);
    }
    
            printf("HASH key:%s: and hash: %s\n", key, buf );

    return buf;
}



void* processor(void *arg ){
    int client_id = *((int *)arg);
    char *cmd, *key, *value, response[BUFFER_LENGTH];
    char tk[4] = " \n\t";
    char tkv[2] = " \n";
    char buffer[BUFFER_LENGTH];
    int n = 0, dbptr = 0;
    
    
    while((n = recv(client_id, buffer, BUFFER_LENGTH, 0)) > 0){

        sprintf( buffer, "%s\n", buffer );

        cmd = strtok( buffer, tk );
        if (strcmp(cmd, "SET") == 0) {
            key = strtok( NULL, tk );
            value = strtok( NULL, tkv );
            if( set( &mydb[dbptr],  hash( key), value) )
                sprintf(response,"OK\n");    

        } else if (strcmp(cmd, "GET") == 0){
            key = strtok( NULL, tk );
            printf("GET:%s:\n", key);
            sprintf(response,"%s\n", get(&mydb[dbptr],   hash(key)) );

        } else if (strcmp(cmd, "DEL") == 0){
            if(del(&mydb[dbptr], hash( strtok( NULL, tk ) ) )){
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
