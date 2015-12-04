#ifdef __APPLE__
	#include <OpenCL/opencl.h>
#else
	#include <CL/cl.h>
#endif

#include <iostream>

int
openCLGetDeviceId(cl_device_id *device_id)
{
	cl_platform_id		platform_id;
	cl_uint				num_of_platforms;
	cl_uint				num_of_devices;

	if (clGetPlatformIDs(1, &platform_id, &num_of_platforms) != CL_SUCCESS)
	{
		std::cerr << "Unable to get platform_id" << std::endl;
		return (-1);
	}

	if (clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, 1, device_id, &num_of_devices) != CL_SUCCESS)
	{
		std::cerr << "Unable to get device_id" << std::endl;
		return (-1);
	}
	return (0);
}

cl_int
openCLGetContext(cl_device_id *device_id, cl_context *context)
{
	cl_int			err;

	*context = clCreateContext(NULL, 1, device_id, NULL, NULL, &err);
	if (err != CL_SUCCESS)
		std::cerr << "clCreateContext error: " << err << std::endl;
	return (err);
}

cl_int
openCLGetCommandQueue(cl_context context, cl_device_id device_id, cl_command_queue *command_queue)
{
	cl_int			err;

	#ifndef __LINUX__
	*command_queue = clCreateCommandQueueWithProperties(context, device_id, NULL, &err);
	#elif __APPLE__
	*command_queue = clCreateCommandQueue(context,  device_id, 0, &err);
	#endif

	if (err != CL_SUCCESS)
		std::cerr << "clCreateCommandQueue error: " << err << std::endl;
	return (err);
}

cl_int
openCLGetProgram(cl_context context, const char **program_source, cl_program *program)
{
	cl_int			err;

	*program = clCreateProgramWithSource(context, 1, program_source, NULL, &err);
	if (err != CL_SUCCESS)
		std::cerr << "clCreateProgramWithSource error:" << err << std::endl;
	return (err);
}

#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

cl_int
openCLGetProgramFromFile(cl_context context, const char *path, cl_program *program)
{
	struct stat			buf;
	int					fd;
	char				*program_source;
	cl_int				err;

	if (stat(path, &buf) == -1)
		return (-1);	
	if ((fd = open(path, O_RDONLY)) == -1)
		return (-1);
	if (!(program_source = (char *)malloc(sizeof(char) * (buf.st_size + 1))))
	{
		close(fd);
		return (-1);
	}
	if ((err = read(fd, program_source, buf.st_size)) != -1)
	{
		program_source[buf.st_size] = '\0';
		err = openCLGetProgram(context, (const char **)&program_source, program);
	}
	free(program_source);
	close(fd);
	return (err);
}

cl_int
openCLBuildProgram(cl_program program)
{
	cl_int				err;

	err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
	if (err != CL_SUCCESS)
		std::cerr << "Error while building program" << std::endl;
	return (err);
}

cl_int
OpenCLCreateKernel(cl_program program, const char *kernel_name, cl_kernel *kernel)
{
	cl_int				err;

	*kernel = clCreateKernel(program, kernel_name, &err);
	if (err != CL_SUCCESS)
		std::cerr << "Error while creating kernel: " << err << std::endl; 
	return (err);
}

typedef struct			s_cl_data
{
	cl_device_id		device_id;
	cl_context			context;
	cl_command_queue	command_queue;
	cl_program			program;
	cl_kernel			kernel;
}						t_cl_data;

int
OpenCLInit(t_cl_data *cl_data)
{
	cl_int				err;

	if ((err = openCLGetDeviceId(&cl_data->device_id)) != 0)
		return (err);
	if ((err = openCLGetContext(&cl_data->device_id, &cl_data->context)) != CL_SUCCESS)
		return (err);
	if ((err = openCLGetCommandQueue(cl_data->context, cl_data->device_id, &cl_data->command_queue)) != CL_SUCCESS)
	{
		clReleaseContext(cl_data->context);
		return (err);
	}
	return (CL_SUCCESS);
}

void
OpenCLRelease(t_cl_data *cl_data)
{
	// clReleaseKernel(cl_data->kernel);
	// clReleaseProgram(cl_data->program);
	clReleaseCommandQueue(cl_data->command_queue);
	clReleaseContext(cl_data->context);
}

int
main(void)
{
	t_cl_data			cl_data;

	OpenCLInit(&cl_data);
	OpenCLRelease(&cl_data);
}

int
main2(void)
{
	cl_device_id		device_id;
	cl_context			context;
	cl_command_queue	command_queue;
	cl_program			program;
	cl_kernel			kernel;

	if (openCLGetDeviceId(&device_id) != 0)
		return (-1);

	if (openCLGetContext(&device_id, &context) != CL_SUCCESS)
		return (-1);

	if (openCLGetCommandQueue(context, device_id, &command_queue) != CL_SUCCESS)
		return (-1);

	if (openCLGetProgramFromFile(context, std::string("test.cl").c_str(), &program) != CL_SUCCESS)
		return (-1);

	if (OpenCLBuildProgram(program) != CL_SUCCESS)
		return (-1);

	if (OpenCLCreateKernel(program, std::string("hello").c_str(), &kernel) != CL_SUCCESS)
		return (-1);


	#define				NUM_ELEMENTS_X 1920
	#define				NUM_ELEMENTS_Y 1080
	#define				NUM_ELEMENTS (NUM_ELEMENTS_X * NUM_ELEMENTS_Y)

	float				data[NUM_ELEMENTS];
	cl_mem				input;
	cl_mem				output;

	for (size_t i = 0; i < NUM_ELEMENTS; i++)
	{
		data[i] = 0;
	}

	input = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(*data) * NUM_ELEMENTS, NULL, NULL);
	output = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(*data) * NUM_ELEMENTS, NULL, NULL);

	clEnqueueWriteBuffer(command_queue, input, CL_TRUE, 0, sizeof(*data) * NUM_ELEMENTS, data, 0, NULL, NULL);

	clSetKernelArg(kernel, 0, sizeof(cl_mem), &input);
	clSetKernelArg(kernel, 1, sizeof(cl_mem), &output);

	size_t				global_item_size[2] = {NUM_ELEMENTS_X, NUM_ELEMENTS_Y};
    // size_t				local_item_size[2] = {4, 4};

	clEnqueueNDRangeKernel(command_queue, kernel, 2, NULL, global_item_size, NULL, 0, NULL, NULL);
	clFinish(command_queue);

	clEnqueueReadBuffer(command_queue, output, CL_TRUE, 0, sizeof(*data) * NUM_ELEMENTS, data, 0, NULL, NULL);

	// for (size_t i = 0; i < NUM_ELEMENTS_Y; i++)
	// {
	// 	std::cout << i << ": ";
	// 	for (size_t j = 0; j < NUM_ELEMENTS_X; j++)
	// 		std::cout << data[i * NUM_ELEMENTS_X + j] << "| ";
	// 	std::cout << std::endl;
	// }
	// std::cout << std::endl;

	clReleaseMemObject(input);
	clReleaseMemObject(output);
	clReleaseKernel(kernel);
	clReleaseProgram(program);
	clReleaseCommandQueue(command_queue);
	clReleaseContext(context);

	return (0);
}
