__kernel void hello(__global float *input, __global float *output)
{
	size_t id;

	id = get_global_id(0);
	output[id] = id;
}