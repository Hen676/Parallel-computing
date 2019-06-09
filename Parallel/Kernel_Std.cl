kernel void min__(global float* data, global float* output, local float* partial_data)
{
	int id = get_local_id(0);
	int group_size = get_local_size(0);
	partial_data[id] = data[get_global_id(0)];

	barrier(CLK_LOCAL_MEM_FENCE);

	for (int i = group_size / 2; i > 0; i >>= 1)
	{
		if (id < i)
		{
			if (partial_data[id] > partial_data[id + i])
			{
				partial_data[id] = partial_data[id + i];
			}
		}

		barrier(CLK_LOCAL_MEM_FENCE);
	}

	if (id == 0)
	{
		output[get_group_id(0)] = partial_data[0];
	}
}
