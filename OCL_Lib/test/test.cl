int
getIndexX(void)
{
	return (get_global_id(0));
}

int
getIndexY(void)
{
	return (get_global_id(1));
}

int
getGridWidth(void)
{
	return (get_num_groups(0) * get_local_size((0)));
}

int
getIndex(void)
{
	return (getIndexY() * getGridWidth() + getIndexX());
}

__kernel
void
hello(__global float *input, __global float *output)
{
	int				index = getIndex();

	output[index] = 1 * 2 * 3 * 4 * 5 * 6 * 7 * 8 * 9;
}


