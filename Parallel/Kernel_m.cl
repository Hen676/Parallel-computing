kernel void reduction_vector(global float* data, global float* output, local float* partial_data)
{
	int id = get_local_id(0);
	int group_size = get_local_size(0);

	//save to local
	partial_data[id] = data[get_global_id(0)];

	barrier(CLK_LOCAL_MEM_FENCE); //sync

	for (int i = group_size / 2; i > 0; i >>= 1)
	{
		if (id < i)
		{
			partial_data[id] += partial_data[id + i]; //add 1st value to 2nd value
		}
		barrier(CLK_LOCAL_MEM_FENCE); //sync
	}

	if (id == 0) 
	{
		output[get_group_id(0)] = dot(partial_data[0], (float)(1.0f)); //output value
	}
}



kernel void minus_square(global const float* data, global float* output, global const float* mean)
{
	int id = get_global_id(0);

	output[id] = (data[id] - mean[id])*(data[id] - mean[id]);

	barrier(CLK_GLOBAL_MEM_FENCE); //sync
}

kernel void min_(global float* data, global float* output, local float* partial_data)
{
	int id = get_local_id(0);
	int group_size = get_local_size(0);

	//save to local
	partial_data[id] = data[get_global_id(0)];

	barrier(CLK_LOCAL_MEM_FENCE); //sync

	for (int i = group_size / 2; i > 0; i >>= 1)
	{
		if (id < i)
		{
			if (partial_data[id] > partial_data[id + i]) //chech if 2nd value is smaller than 1st
			{
				partial_data[id] = partial_data[id + i]; //if yes, replace 1st with second
			}
		}

		barrier(CLK_LOCAL_MEM_FENCE); //sync
	}

	if (id == 0)
	{
		output[get_group_id(0)] = partial_data[0]; //output value
	}
}

kernel void max_(global float* data, global float* output, local float* partial_data)
{
	int id = get_local_id(0);
	int group_size = get_local_size(0);

	//save to local
	partial_data[id] = data[get_global_id(0)];

	barrier(CLK_LOCAL_MEM_FENCE); //sync

	for (int i = group_size / 2; i > 0; i >>= 1)
	{
		if (id < i)
		{
			if (partial_data[id] < partial_data[id + i]) //chech if 2nd value is larger than 1st
			{
				partial_data[id] = partial_data[id + i]; //if yes, replace 1st with second
			}
		}

		barrier(CLK_LOCAL_MEM_FENCE); //sync
	}

	if (id == 0)
	{
		output[get_group_id(0)] = partial_data[0]; //output value
	}
}