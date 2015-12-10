#include <OCL.h>

cl_int
OCLInit(t_cl_data *cl_data)
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

cl_int
OCLBuildPRogram(t_cl_data *cl_data, char *path)
{
	cl_int				err;

	if ((err = openCLGetProgramFromFile(cl_data->context, path, &cl_data->program)) != CL_SUCCESS)
		return (err);
	if ((err = openCLBuildProgram(cl_data->program, cl_data->device_id)) != CL_SUCCESS)
		return (err);
	return (CL_SUCCESS);
}

cl_int
OCLCreateKernel(t_cl_data *cl_data, char *name)
{
	cl_int				err;

	if ((err = openCLCreateKernel(cl_data->program, name, &cl_data->kernel)) != CL_SUCCESS)
		return (err);
	return (CL_SUCCESS);
}