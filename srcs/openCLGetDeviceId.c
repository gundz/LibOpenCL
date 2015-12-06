#include <OCL.h>
#include <unistd.h>
#include <stdio.h>

int
openCLGetDeviceId(cl_device_id *device_id)
{
	cl_platform_id		platform_id;
	cl_uint				num_of_platforms;
	cl_uint				num_of_devices;

	if (clGetPlatformIDs(1, &platform_id, &num_of_platforms) != CL_SUCCESS)
	{
		dprintf(STDERR_FILENO, "Unable to get platform_id\n");
		return (-1);
	}

	if (clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, 1, device_id, &num_of_devices) != CL_SUCCESS)
	{
		dprintf(STDERR_FILENO, "Unable to get device_id\n");
		return (-1);
	}
	return (0);
}