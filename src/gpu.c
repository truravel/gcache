/* GCache .
 * Luis Angel Trutie Ravelo, truravel@gmail.com
 * 
 * This code is licenced under the GPL.
 */


struct element * search(struct db *mdb, char *key){ 
    cl_int ret;
    struct element *e =(struct element *) malloc(sizeof(struct element ));
    size_t global_item_size;

    cl_command_queue cq = clCreateCommandQueue(context, device_id, 0, &ret);  
    cl_kernel kernel = clCreateKernel(program, "g_cache", &ret);     

    mdb->nodes_objs = clCreateBuffer(context, CL_MEM_READ_WRITE, MAX_ELEMENTS*sizeof(struct element), NULL, &ret);
    mdb->out_objs = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(struct element), NULL, &ret);
    mdb->key_objs = clCreateBuffer(context, CL_MEM_READ_WRITE, 40*sizeof(char), NULL, &ret);

    ret = clEnqueueWriteBuffer(cq, mdb->out_objs, CL_TRUE, 0, sizeof(struct element), e, 0, NULL, NULL); 
    ret = clEnqueueWriteBuffer(cq, mdb->key_objs, CL_TRUE, 0,40*sizeof(char), key, 0, NULL, NULL); 
     ret = clEnqueueWriteBuffer(cq, mdb->nodes_objs, CL_TRUE, 0, MAX_ELEMENTS*sizeof(struct element), mdb->gchache, 0, NULL, NULL); 

    ret = clSetKernelArg( kernel, 0, sizeof(cl_mem), (void *)&mdb->nodes_objs );
    ret = clSetKernelArg( kernel, 1, sizeof(cl_mem), (void *)&mdb->key_objs );
    ret = clSetKernelArg( kernel, 2, sizeof(cl_mem), (void *)&mdb->out_objs );

    global_item_size = MAX_ELEMENTS;
    ret = clEnqueueNDRangeKernel(cq, kernel, 1, NULL, &global_item_size, NULL, 0, NULL, NULL );
        if( ret != CL_SUCCESS )  
            printf("Kernel Main Error %d\n", ret);
    
    ret = clEnqueueReadBuffer(cq, mdb->out_objs, CL_TRUE, 0, sizeof(struct element) , e, 0, NULL, NULL);
    ret = clReleaseMemObject(mdb->out_objs);
    ret = clReleaseMemObject( mdb->key_objs);
    ret = clReleaseMemObject(mdb->out_objs); 

    return e;
}


int gpu_load_kernel(){
	FILE *fp;
    char *source_str, configStr[2000];
    char fileName[] = "./src/kernel.cl";
    size_t source_size;
    fp = fopen(fileName, "r");
    if(!fp) {
        fprintf(stderr, "Failed to load kernel.\n");
        exit(1);
    }
    source_str = (char*) malloc (MAX_SOURCE_SIZE);
    source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);
    fclose(fp);
	ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
    printf("Number of GPU: %d\n",ret_num_platforms);
    ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_DEFAULT, 1, &device_id, &ret_num_devices);
    ////////////
    size_t valueSize;
    clGetDeviceInfo(device_id, CL_DEVICE_NAME, 0, NULL, &valueSize);
    gpu_name = (char*) malloc(valueSize);
    clGetDeviceInfo(device_id, CL_DEVICE_NAME, valueSize, gpu_name, NULL);
    printf("Device Name: %s\n",gpu_name);
    ////////////
    context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);
    program = clCreateProgramWithSource(context, 1, (const char **)&source_str, (const size_t *)&source_size, &ret); 
    ret = clBuildProgram(program, 1, &device_id, "-save-temps", NULL, NULL);   

    if(ret != CL_SUCCESS) {
        printf("clBuildProgram error: %d\n", ret);
        char buf[0x10000];
        clGetProgramBuildInfo(program,device_id,CL_PROGRAM_BUILD_LOG,0x10000,buf,NULL);
        printf("\n####################################################\n%s\n####################################################\n", buf);
        return(-1);
    }


    return 1;
}