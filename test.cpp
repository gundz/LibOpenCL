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

const char *program_source = 
"__kernel void hello(__global float *input, __global float *output)\n"\
"{\n"\
"  size_t id = get_global_id(0);\n"\
"  output[id] = input[id] * input[id];\n"\
"}\n";

int
main(void)
{
	cl_device_id		device_id;
	cl_context			context;
	cl_command_queue	command_queue;

	cl_program			program;

	if (openCLGetDeviceId(&device_id) != 0)
		return (-1);

	if (openCLGetContext(&device_id, &context) != CL_SUCCESS)
		return (-1);

	if (openCLGetCommandQueue(context, device_id, &command_queue) != CL_SUCCESS)
		return (-1);

	if (openCLGetProgram(context, &program_source, &program) != CL_SUCCESS)
		return (-1);

	return (0);
}