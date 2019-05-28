/* GCache .
 * Luis Angel Trutie Ravelo, truravel@gmail.com
 * 
 * This code is licenced under the GPL.
 */


int create_elements(int id){
    mydb[id].gchache = NULL;
    mydb[id].gchache = (struct element *) malloc(MAX_ELEMENTS*sizeof(struct element));
    mydb[id].count = MAX_ELEMENTS;
    if( mydb[id].gchache != NULL ) return 0;
        else return 1;
}


int store_value(struct element *e,char *key, char *value) {

    memcpy( &(e->key), key, strlen(key)+1);
    void *pt = malloc( strlen(key) );
    memcpy( pt, value, strlen(value)+1);
    e->value = pt;
    printf("Value=%s", (char *)pt);
    return 0;
}


