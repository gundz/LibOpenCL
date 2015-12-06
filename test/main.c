#include <OCL.h>
#include <stdio.h>

int
main(void)
{
	t_cl_data			cl_data;

	if (OCLInit(&cl_data) == CL_SUCCESS)
	{
		if (OCLBuildPRogram(&cl_data, "test.cl") == CL_SUCCESS)
		{
			if (OCLCreateKernel(&cl_data, "hello") == CL_SUCCESS)
			{
				printf("Test de ouf !\n");
				clReleaseKernel(cl_data.kernel);
			}
			clReleaseProgram(cl_data.program);
		}
		clReleaseCommandQueue(cl_data.command_queue);
		clReleaseContext(cl_data.context);
	}
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

	if (openCLGetProgramFromFile(context, "test.cl", &program) != CL_SUCCESS)
		return (-1);

	if (openCLBuildProgram(program) != CL_SUCCESS)
		return (-1);

	if (openCLCreateKernel(program, "hello", &kernel) != CL_SUCCESS)
		return (-1);


	#define				NUM_ELEMENTS_X 1920
	#define				NUM_ELEMENTS_Y 1080
	#define				NUM_ELEMENTS (NUM_ELEMENTS_X * NUM_ELEMENTS_Y)

	float				data[NUM_ELEMENTS];
	cl_mem				input;
	cl_mem				output;

	size_t				i, j;
	for (i = 0; i < NUM_ELEMENTS; i++)
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
}