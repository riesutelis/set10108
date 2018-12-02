#include <iostream>
#include <cuda_runtime.h>
#include <device_launch_parameters.h>
#include <vector>

using namespace std;

__global__ void simple_multiply(float *output_C, unsigned int width_A, unsigned int height_A, unsigned int width_B, unsigned int height_B, const float *input_A, const float *input_B)
{
	// Get global position in Y direction
	//unsigned int row = (blockIdx.y * 16) + threadIdx.y;
	// Get global position in X direction
	//unsigned int col = (blockIdx.x * 16) + threadIdx.x;
	unsigned int row = threadIdx.x;
	// Get global position in X direction
	unsigned int col = blockIdx.x;

	float sum = 0.0f;

	// Calculate result of one element of matrix C
	for (unsigned int i = 0; i < width_A; ++i)
		sum += input_A[row * width_A + i] * input_B[i * width_B + col];

	// Store result in matrix C
	output_C[row * width_B + col] = sum;
}

int main(int argc, char **argv)
{
	unsigned int A_Width = 16, B_Width = 16, A_Height = 16, B_Height = 16;
	auto size_A = sizeof(float) * A_Width * A_Height;
	auto size_B = sizeof(float) * B_Width * B_Height;
	auto size_C = sizeof(float) * B_Width * A_Height;
	vector<float> A(A_Width * A_Height);
	vector<float> B(B_Width * B_Height);
	vector<float> C(A_Height * B_Width);

	for (int i = 0; i < A_Width; i++)
		for (int j = 0; j < A_Height; j++)
			A[i * A_Width + j] = 1.0f;
	for (int i = 0; i < B_Width; i++)
		for (int j = 0; j < B_Height; j++)
			B[i * B_Width + j] = 1.0f;

	// Device memory
	float *d_A, *d_B, *d_C;
	cudaMalloc((void**)&d_A, size_A);
	cudaMalloc((void**)&d_B, size_B);
	cudaMalloc((void**)&d_C, size_C);

	cudaMemcpy(d_A, &A[0], size_A, cudaMemcpyHostToDevice);
	cudaMemcpy(d_B, &B[0], size_B, cudaMemcpyHostToDevice);

	simple_multiply <<<A_Height, B_Width >> > (d_C, A_Width, A_Height, B_Width, B_Height, d_A, d_B);
	cudaDeviceSynchronize();

	cudaMemcpy(&C[0], d_C, size_C, cudaMemcpyDeviceToHost);

	cudaFree(d_A);
	cudaFree(d_B);
	cudaFree(d_C);

	for (int i = 0; i < A_Height; i++)
	{
		for (int j = 0; j < B_Width; j++)
			cout << C[A_Height * i + j] << " ";
		cout << endl;
	}

	return 0;
}