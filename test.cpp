#include <OpenCL/opencl.h>

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

	*command_queue = clCreateCommandQueue(context,  device_id, 0, &err);
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
OpenCLBuildProgram(cl_program program)
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

int
main(void)
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

	size_t				size = 10;
	size_t				oSize = (sizeof(float) * size) * size;
	float				data[size * size];
	float				data_out[size * size];
	cl_mem				input;
	cl_mem				output;

	for (size_t i = 0; i < size; i++)
	{
		for (size_t j = 0; j < size; j++)
		{
			data[i * size + j] = 0;
			data_out[i * size + j] = 0;
		}
	}

	input = clCreateBuffer(context, CL_MEM_READ_ONLY, oSize, NULL, NULL);
	output = clCreateBuffer(context, CL_MEM_WRITE_ONLY, oSize, NULL, NULL);

	clEnqueueWriteBuffer(command_queue, input, CL_TRUE, 0, oSize, data, 0, NULL, NULL);

	clSetKernelArg(kernel, 0, sizeof(cl_mem), &input);
	clSetKernelArg(kernel, 1, sizeof(cl_mem), &output);

	for (size_t i = 0; i < size; i++)
	{
		std::cout << i << ": ";
		for (size_t j = 0; j < size; j++)
			std::cout << data[i * size + j] << "| ";
		std::cout << std::endl;
	}
	std::cout << std::endl;

	clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, &size, NULL, 0, NULL, NULL);
	clFinish(command_queue);

	clEnqueueReadBuffer(command_queue, output, CL_TRUE, 0, oSize, data_out, 0, NULL, NULL);

	for (size_t i = 0; i < size; i++)
	{
		std::cout << i << ": ";
		for (size_t j = 0; j < size; j++)
			std::cout << data[i * size + j] << "| ";
		std::cout << std::endl;
	}

	clReleaseKernel(kernel);
	clReleaseProgram(program);
	clReleaseCommandQueue(command_queue);
	clReleaseContext(context);

	return (0);
}
