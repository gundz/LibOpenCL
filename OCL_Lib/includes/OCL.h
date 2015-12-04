#ifndef		OCL_H
#define		OCL_H

#ifdef __APPLE__
	#include <OpenCL/opencl.h>
#else
	#include <CL/cl.h>
#endif

typedef struct			s_cl_data
{
	cl_device_id		device_id;
	cl_context			context;
	cl_command_queue	command_queue;
	cl_program			program;
	cl_kernel			kernel;
}						t_cl_data;

cl_int
OCLInit(t_cl_data *cl_data);

cl_int
OCLBuildPRogram(t_cl_data *cl_data, char *path);




int
openCLGetDeviceId(cl_device_id *device_id);

cl_int
openCLGetContext(cl_device_id *device_id, cl_context *context);

cl_int
openCLGetCommandQueue(cl_context context, cl_device_id device_id, cl_command_queue *command_queue);

cl_int
openCLGetProgram(cl_context context, const char **program_source, cl_program *program);

cl_int
openCLGetProgramFromFile(cl_context context, const char *path, cl_program *program);

cl_int
openCLBuildProgram(cl_program program);

cl_int
openCLCreateKernel(cl_program program, const char *kernel_name, cl_kernel *kernel);

#endif