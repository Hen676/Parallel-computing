// Parallel.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <chrono> 
#include <stdio.h>
#include "Main.h"

#ifdef __APPLE__
#include <OpenCL/opencl.hpp>
#else
#include <CL/cl.hpp>
#endif

#define MAX_SOURCE_SIZE (0x100000)

using namespace std;
vector<Data_st> Read();
vector<Data_st> Read_short();
vector<float> Avg(vector<float>);
vector<float> StdDiv(vector<float>,float);
vector<float> Min(vector<float>);
vector<float> Max(vector<float>);

int platform_id = 0;
int device_id = 0;

//utils to get openCL running
//
cl::Context GetContext(int platform_id, int device_id) {
	vector<cl::Platform> platforms;

	cl::Platform::get(&platforms);

	for (unsigned int i = 0; i < platforms.size(); i++)
	{
		vector<cl::Device> devices;
		platforms[i].getDevices((cl_device_type)CL_DEVICE_TYPE_ALL, &devices);

		for (unsigned int j = 0; j < devices.size(); j++)
		{
			if ((i == platform_id) && (j == device_id))
				return cl::Context({ devices[j] });
		}
	}

	return cl::Context();
}

string GetPlatformName(int platform_id) {
	vector<cl::Platform> platforms;
	cl::Platform::get(&platforms);
	return platforms[platform_id].getInfo<CL_PLATFORM_NAME>();
}

string GetDeviceName(int platform_id, int device_id) {
	vector<cl::Platform> platforms;
	cl::Platform::get(&platforms);
	vector<cl::Device> devices;
	platforms[platform_id].getDevices((cl_device_type)CL_DEVICE_TYPE_ALL, &devices);
	return devices[device_id].getInfo<CL_DEVICE_NAME>();
}

void AddSources(cl::Program::Sources& sources, const string& file_name) {
	ifstream file(file_name);
	string* source_code = new string(istreambuf_iterator<char>(file), (istreambuf_iterator<char>()));
	sources.push_back(make_pair((*source_code).c_str(), source_code->length() + 1));
}
//

int main()
{
	string selection;
	vector<Data_st> x; //main data set

	system("CLS");
	std::cout << "*******************************" << endl;
	std::cout << "Select data" << endl;
	std::cout << "*******************************" << endl;
	std::cout << "1:Short" << endl;
	std::cout << "2:All" << endl;
	getline(cin, selection);

	//short
	if (selection.compare("1") == 0)
	{
		x = Read_short();
	}
	//all
	else if (selection.compare("2") == 0)
	{
		x = Read();
	}

	system("CLS");
	std::cout << "*******************************" << endl;
	std::cout << "Select data manipulation" << endl;
	std::cout << "*******************************" << endl;
	std::cout << "1:Average" << endl;
	std::cout << "2:Std div" << endl;
	std::cout << "3:Min" << endl;
	std::cout << "4:Max" << endl;
	getline(cin, selection);
	system("CLS");

	//Average
	if (selection.compare("1") == 0)
	{
		vector<float> A; //temp data

		int i;
		for (i = 0; i < x.size(); i++) //push temp to A
			A.push_back(x[i].temp);

		vector<float> B(A.size()); //switch data

		auto start = chrono::high_resolution_clock::now();
		while (B.size() > 1) // keep reducing until output size = 1
		{
			B = Avg(A);

			vector<float> temp = A; A = B; B = temp;
		}

		float mean = A[0] / x.size(); //last calculation

		auto stop = chrono::high_resolution_clock::now();

		auto duration = chrono::duration_cast<chrono::milliseconds>(stop - start); //time calculations
		std::cout << "Program Execution time: " << duration.count() / 10 << " milliseconds" << endl;
		std::cout << "Bytes per seond: " << ((sizeof(double) * x.size()) * 10000) / duration.count() << " bytes per second" << endl;
		std::cout << "Avg: " << mean << endl;
	}
	//Std div
	else if (selection.compare("2") == 0)
	{
		vector<float> A; //temps

		int i;
		for (i = 0; i < x.size(); i++)
			A.push_back(x[i].temp);

		vector<float> B(A.size());

		auto start = chrono::high_resolution_clock::now();
		while (B.size() > 1)
		{
			B = Avg(A);

			vector<float> temp = A; A = B; B = temp;
		}

		float mean = A[0] / x.size(); //mean calculation

		vector<float> C; //temps
		for (i = 0; i < x.size(); i++)
			C.push_back(x[i].temp);

		vector<float> Std_div = StdDiv(C,mean);


		while (C.size() > 1)
		{
			C = Avg(Std_div);

			vector<float> temp = C; C = Std_div; Std_div = temp;
		}

		float std = sqrt(Std_div[0] / x.size()); //second mean calculation

		auto stop = chrono::high_resolution_clock::now();

		auto duration = chrono::duration_cast<chrono::milliseconds>(stop - start);
		std::cout << "Program Execution time: " << duration.count() / 10 << " milliseconds" << endl;
		std::cout << "Bytes per seond: " << (sizeof(double) * x.size() * 10000) / duration.count() << " bytes per second" << endl;
		std::cout << "Div: " << std << endl;
	}
	//Min
	else if (selection.compare("3") == 0)
	{
		vector<float> A;

		int i;
		for (i = 0; i < x.size(); i++)
			A.push_back(x[i].temp);

		vector<float> B(A.size());

		auto start = chrono::high_resolution_clock::now();
		while (B.size() > 1)
		{
			B = Min(A);

			vector<float> temp = A; A = B; B = temp;


		}

		float min = A[0];

		auto stop = chrono::high_resolution_clock::now();

		auto duration = chrono::duration_cast<chrono::milliseconds>(stop - start);
		std::cout << "Program Execution time: " << duration.count() / 10 << " milliseconds" << endl;
		std::cout << "Bytes per seond: " << (sizeof(double) * x.size() * 10000) / duration.count() << " bytes per second" << endl;
		std::cout << "Min: " << min << endl;
	}
	//Max
	else if (selection.compare("4") == 0)
	{
		vector<float> A;

		int i;
		for (i = 0; i < x.size(); i++)
			A.push_back(x[i].temp);

		vector<float> B(A.size());

		auto start = chrono::high_resolution_clock::now();
		while (B.size() > 1)
		{
			B = Max(A);

			vector<float> temp = A; A = B; B = temp;
		}

		float max = A[0];

		auto stop = chrono::high_resolution_clock::now();

		auto duration = chrono::duration_cast<chrono::milliseconds>(stop - start);
		std::cout << "Program Execution time: " << duration.count() / 10 << " milliseconds" << endl;
		std::cout << "Bytes per seond: " << (sizeof(double) * x.size() * 10000) / duration.count() << " bytes per second" << endl;
		std::cout << "Max: " << max << endl;
	}
	//Selection wrong
	else
	{
		std::cout << selection << ": not vaild input. try 1,2,3 etc." << endl;
	}

}

//read all data
vector<Data_st> Read()
{
	std::cout << "Loading: temp_lincolnshire.txt" << endl;

	ifstream data("temp_lincolnshire.txt");

	vector<Data_st> Data;

	//data structure
	string station;
	int year;
	int month;
	int day;
	int time;
	float temp;

	if (data.is_open())
	{
		while (data >> station >> year >> month >> day >> time >> temp)
		{
			Data_st tempPush;

			tempPush.station = station;
			tempPush.year = year;
			tempPush.month = month;
			tempPush.day = day;
			tempPush.time = time;
			tempPush.temp = temp;

			Data.push_back(tempPush);
		}
	}
	else
		std::cout << "file not found: temp_lincolnshire.txt" << endl;

	return Data;
}

//read short data
vector<Data_st> Read_short()
{
	std::cout << "Loading: temp_lincolnshire_short.txt" << endl;

	ifstream data("temp_lincolnshire_short.txt");

	vector<Data_st> Data;

	//data structure
	string station;
	int year;
	int month;
	int day;
	int time;
	float temp;

	if (data.is_open())
	{
		while (data >> station >> year >> month >> day >> time >> temp)
		{
			Data_st tempPush;

			tempPush.station = station;
			tempPush.year = year;
			tempPush.month = month;
			tempPush.day = day;
			tempPush.time = time;
			tempPush.temp = temp;

			Data.push_back(tempPush);
		}
	}
	else
		std::cout << "file not found: temp_lincolnshire_short.txt" << endl;

	return Data;
}

//average values, sequential reduction
vector<float> Avg(vector<float> data)
{
	int platform_id = 0;
	int device_id = 0;

	//create queue
	cl::Context context = GetContext(platform_id, device_id);
	cl::CommandQueue queue(context, CL_QUEUE_PROFILING_ENABLE);
	cl::Program::Sources sources;
	AddSources(sources, "Kernel_m.cl");
	cl::Program program(context, sources);

	vector<cl::Device> devices = context.getInfo<CL_CONTEXT_DEVICES>();
	
	typedef float mytype;

	try {
		program.build();
	}
	catch (...) {
		std::cout << "Build Status: " << program.getBuildInfo<CL_PROGRAM_BUILD_STATUS>(context.getInfo<CL_CONTEXT_DEVICES>()[0]) << endl;
		std::cout << "Build Options:\t" << program.getBuildInfo<CL_PROGRAM_BUILD_OPTIONS>(context.getInfo<CL_CONTEXT_DEVICES>()[0]) << endl;
		std::cout << "Build Log:\t " << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(context.getInfo<CL_CONTEXT_DEVICES>()[0]) << endl;
	}

	size_t local_size = 16;
	size_t padding_size = data.size() % local_size;

	if (padding_size) {
		vector<mytype> A_ext(local_size - padding_size, 1);
		data.insert(data.end(), A_ext.begin(), A_ext.end());
	}

	size_t input_elements = data.size();
	size_t input_size = data.size() * sizeof(mytype);
	size_t nr_groups = input_elements / local_size;

	vector<mytype> temp(input_elements);
	size_t output_size = temp.size() * sizeof(mytype);

	cl::Buffer buffer_A(context, CL_MEM_READ_ONLY, input_size);
	cl::Buffer buffer_B(context, CL_MEM_READ_WRITE, output_size);

	queue.enqueueWriteBuffer(buffer_A, CL_TRUE, 0, input_size, data.data());
	queue.enqueueFillBuffer(buffer_B, 0, 0, output_size);

	cl::Kernel kernel_1 = cl::Kernel(program, "reduction_vector");
	kernel_1.setArg(0, buffer_A);
	kernel_1.setArg(1, buffer_B);
	kernel_1.setArg(2, cl::Local(local_size * sizeof(mytype)));

	cl_int err;
	cl::Event event;
	err = queue.enqueueNDRangeKernel(kernel_1, cl::NullRange, cl::NDRange(input_elements), cl::NDRange(local_size), NULL, &event);
	//std::cout << err << endl;

	queue.finish();

	queue.enqueueReadBuffer(buffer_B, CL_TRUE, 0, output_size, temp.data());

	temp.erase(remove(temp.begin(), temp.end(), 0), temp.end());

	cl_ulong time_start = event.getProfilingInfo<CL_PROFILING_COMMAND_START>();
	cl_ulong time_end = event.getProfilingInfo<CL_PROFILING_COMMAND_END>();

	double seconds = time_end - time_start;
	std::cout << "Kernal Execution time: " << seconds / 1000 << " milliseconds" << endl;

	return temp;
}

//minus values from mean and square them. basic parallel
vector<float> StdDiv(vector<float> data,float x)
{
	int platform_id = 0;
	int device_id = 0;

	//create queue
	cl::Context context = GetContext(platform_id, device_id);
	cl::CommandQueue queue(context, CL_QUEUE_PROFILING_ENABLE);
	cl::Program::Sources sources;
	AddSources(sources, "Kernel_m.cl");
	cl::Program program(context, sources);

	vector<cl::Device> devices = context.getInfo<CL_CONTEXT_DEVICES>();

	typedef float mytype;

	try {
		program.build();
	}
	catch (...) {
		std::cout << "Build Status: " << program.getBuildInfo<CL_PROGRAM_BUILD_STATUS>(context.getInfo<CL_CONTEXT_DEVICES>()[0]) << endl;
		std::cout << "Build Options:\t" << program.getBuildInfo<CL_PROGRAM_BUILD_OPTIONS>(context.getInfo<CL_CONTEXT_DEVICES>()[0]) << endl;
		std::cout << "Build Log:\t " << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(context.getInfo<CL_CONTEXT_DEVICES>()[0]) << endl;
	}

	size_t input_elements = data.size();
	size_t input_size = data.size() * sizeof(mytype);

	vector<mytype> temp(input_elements);
	size_t output_size = temp.size() * sizeof(mytype);
	vector<mytype> mean(input_elements, x);
	size_t mean_size = mean.size() * sizeof(mytype);

	cl::Buffer buffer_A(context, CL_MEM_READ_ONLY, input_size);
	cl::Buffer buffer_B(context, CL_MEM_READ_WRITE, output_size);
	cl::Buffer buffer_C(context, CL_MEM_READ_WRITE, mean_size);

	queue.enqueueWriteBuffer(buffer_A, CL_TRUE, 0, input_size, data.data());
	queue.enqueueFillBuffer(buffer_B, 0, 0, output_size);
	queue.enqueueWriteBuffer(buffer_C, CL_TRUE, 0, mean_size, mean.data());

	cl::Kernel kernel_1 = cl::Kernel(program, "minus_square");
	kernel_1.setArg(0, buffer_A);
	kernel_1.setArg(1, buffer_B);
	kernel_1.setArg(2, buffer_C);

	cl_int err;
	cl::Event event;
	err = queue.enqueueNDRangeKernel(kernel_1, cl::NullRange, cl::NDRange(input_elements), cl::NullRange, NULL, &event);
	//std::cout << err << endl;

	queue.finish();

	queue.enqueueReadBuffer(buffer_B, CL_TRUE, 0, output_size, temp.data());

	cl_ulong time_start = event.getProfilingInfo<CL_PROFILING_COMMAND_START>();
	cl_ulong time_end = event.getProfilingInfo<CL_PROFILING_COMMAND_END>();

	double seconds = time_end - time_start;
	std::cout << "Kernal Execution time: " << seconds / 1000 << " milliseconds" << endl;

	return temp;
}

//find min, sequential reduction
vector<float> Min(vector<float> data)
{
	int platform_id = 0;
	int device_id = 0;

	cl::Context context = GetContext(platform_id, device_id);
	cl::CommandQueue queue(context, CL_QUEUE_PROFILING_ENABLE);
	cl::Program::Sources sources;
	AddSources(sources, "Kernel_m.cl");
	cl::Program program(context, sources);

	vector<cl::Device> devices = context.getInfo<CL_CONTEXT_DEVICES>();

	typedef float mytype;

	try {
		program.build();
	}
	catch (...) {
		std::cout << "Build Status: " << program.getBuildInfo<CL_PROGRAM_BUILD_STATUS>(context.getInfo<CL_CONTEXT_DEVICES>()[0]) << endl;
		std::cout << "Build Options:\t" << program.getBuildInfo<CL_PROGRAM_BUILD_OPTIONS>(context.getInfo<CL_CONTEXT_DEVICES>()[0]) << endl;
		std::cout << "Build Log:\t " << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(context.getInfo<CL_CONTEXT_DEVICES>()[0]) << endl;
	}

	size_t local_size = 16;
	size_t padding_size = data.size() % local_size;

	if (padding_size) {
		vector<mytype> A_ext(local_size - padding_size, 1);
		data.insert(data.end(), A_ext.begin(), A_ext.end());
	}

	size_t input_elements = data.size();
	size_t input_size = data.size() * sizeof(mytype);
	size_t nr_groups = input_elements / local_size;

	vector<mytype> temp(input_elements);
	size_t output_size = temp.size() * sizeof(mytype);

	cl::Buffer buffer_A(context, CL_MEM_READ_ONLY, input_size);
	cl::Buffer buffer_B(context, CL_MEM_READ_WRITE, output_size);

	queue.enqueueWriteBuffer(buffer_A, CL_TRUE, 0, input_size, data.data());
	queue.enqueueFillBuffer(buffer_B, 0, 0, output_size);

	cl::Kernel kernel_1 = cl::Kernel(program, "min_");
	kernel_1.setArg(0, buffer_A);
	kernel_1.setArg(1, buffer_B);
	kernel_1.setArg(2, cl::Local(local_size * sizeof(mytype)));

	cl_int err;
	cl::Event event;
	err = queue.enqueueNDRangeKernel(kernel_1, cl::NullRange, cl::NDRange(input_elements), cl::NDRange(local_size), NULL, &event);
	//std::cout << err << endl;

	queue.finish();

	queue.enqueueReadBuffer(buffer_B, CL_TRUE, 0, output_size, temp.data());

	temp.erase(remove(temp.begin(), temp.end(), 0), temp.end());

	cl_ulong time_start = event.getProfilingInfo<CL_PROFILING_COMMAND_START>();
	cl_ulong time_end = event.getProfilingInfo<CL_PROFILING_COMMAND_END>();

	double seconds = time_end - time_start;
	std::cout << "Kernal Execution time: " << seconds / 1000 << " milliseconds" << endl;

	return temp;
}

//find max, sequential reduction
vector<float> Max(vector<float> data)
{
	int platform_id = 0;
	int device_id = 0;

	cl::Context context = GetContext(platform_id, device_id);
	cl::CommandQueue queue(context, CL_QUEUE_PROFILING_ENABLE);
	cl::Program::Sources sources;
	AddSources(sources, "Kernel_m.cl");
	cl::Program program(context, sources);

	vector<cl::Device> devices = context.getInfo<CL_CONTEXT_DEVICES>();

	typedef float mytype;

	try {
		program.build();
	}
	catch (...) {
		std::cout << "Build Status: " << program.getBuildInfo<CL_PROGRAM_BUILD_STATUS>(context.getInfo<CL_CONTEXT_DEVICES>()[0]) << endl;
		std::cout << "Build Options:\t" << program.getBuildInfo<CL_PROGRAM_BUILD_OPTIONS>(context.getInfo<CL_CONTEXT_DEVICES>()[0]) << endl;
		std::cout << "Build Log:\t " << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(context.getInfo<CL_CONTEXT_DEVICES>()[0]) << endl;
	}

	size_t local_size = 16;
	size_t padding_size = data.size() % local_size;

	if (padding_size) {
		vector<mytype> A_ext(local_size - padding_size, 1);
		data.insert(data.end(), A_ext.begin(), A_ext.end());
	}

	size_t input_elements = data.size();
	size_t input_size = data.size() * sizeof(mytype);
	size_t nr_groups = input_elements / local_size;

	vector<mytype> temp(input_elements);
	size_t output_size = temp.size() * sizeof(mytype);

	cl::Buffer buffer_A(context, CL_MEM_READ_ONLY, input_size);
	cl::Buffer buffer_B(context, CL_MEM_READ_WRITE, output_size);

	queue.enqueueWriteBuffer(buffer_A, CL_TRUE, 0, input_size, data.data());
	queue.enqueueFillBuffer(buffer_B, 0, 0, output_size);

	cl::Kernel kernel_1 = cl::Kernel(program, "max_");
	kernel_1.setArg(0, buffer_A);
	kernel_1.setArg(1, buffer_B);
	kernel_1.setArg(2, cl::Local(local_size * sizeof(mytype)));

	cl_int err;
	cl::Event event;
	err = queue.enqueueNDRangeKernel(kernel_1, cl::NullRange, cl::NDRange(input_elements), cl::NDRange(local_size), NULL, &event);
	//std::cout << err << endl;

	queue.finish();

	queue.enqueueReadBuffer(buffer_B, CL_TRUE, 0, output_size, temp.data());

	temp.erase(remove(temp.begin(), temp.end(), 0), temp.end());

	cl_ulong time_start = event.getProfilingInfo<CL_PROFILING_COMMAND_START>();
	cl_ulong time_end = event.getProfilingInfo<CL_PROFILING_COMMAND_END>();

	double seconds = time_end - time_start;
	std::cout << "Kernal Execution time: " << seconds / 1000 << " milliseconds" << endl;

	return temp;
}