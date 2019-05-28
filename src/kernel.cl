
#pragma OPENCL EXTENSION cl_amd_printf : enable

typedef struct gcache {
	char key[40];
	void *value;
} GPU_cache;


__kernel void g_cache(global char* input,  global char* keys, global char* output ) {
	unsigned int len = sizeof(GPU_cache);
	unsigned int id = get_global_id(0);
	unsigned int i, flag = 0, gid = len*id; 
	GPU_cache g;
	char *element = (char *)&g;

	for(i =0; i < len; i++) element[i] = input[gid+i];
	for(i =0; i < 40; i++) {
		if( keys[i] == g.key[ i ] ) flag = 1;
				else { 
                    flag = 0; 
                    break; 
                }
	}
    if(flag == 1) {
            for(i =0; i < len; i++) output[i] =  element[i] ;
    }
}

