//
// Created by user on 12.09.2021.
//

#include "CUDAUtils.cuh"
#include "../Utils/dataStructures/Bitmap.h"

namespace cn {
    __device__
    int getDataIndex(dim3 bitmapSize, dim3 pos){
        if(pos.x >= bitmapSize.x) {
            printf("x");
        }
        if(pos.y >= bitmapSize.y) {
            printf("y");
        }
        if(pos.z >= bitmapSize.z) {
            printf("z");
        }
        return pos.z * bitmapSize.x * bitmapSize.y + pos.y * bitmapSize.x + pos.x;
    }
    __device__
    dim3 getDataPos(dim3 bitmapSize, int index){
        if(index >= bitmapSize.x * bitmapSize.y * bitmapSize.z)
            printf("zły arg2 :P");
        return dim3(index % bitmapSize.x, (index / bitmapSize.x) % bitmapSize.x, index / (bitmapSize.x * bitmapSize.y));
    }
    __device__
    int afterConvolutionSize(int kernelSize, int inputSize, int padding, int stride) {
        return (inputSize + 2 * padding - kernelSize) / stride + 1;
    }
    __global__
    void cudaConvolveKernel(double *input, double *kernel, double *result, dim3 inputSize, dim3 outputSize, dim3 kernelSize) {
        u_int strideX = inputSize.x / outputSize.x;
        u_int strideY = inputSize.y / outputSize.y;

        u_int index = blockIdx.x * blockDim.x + threadIdx.x;
        u_int posXOutput = index % outputSize.x;
        u_int posYOutput = (index % (outputSize.x * outputSize.y)) / outputSize.x;
        u_int posZOutput = (index % (outputSize.x * outputSize.y * outputSize.z)) / (outputSize.x * outputSize.y);

        printf("%d\n", posZOutput);

        u_int kID = index / (inputSize.x * inputSize.y * inputSize.z);
        u_int kPosX = posXOutput * strideX;
        u_int kPosY = posYOutput * strideY;
        u_int kPosZ = posZOutput;

        double *kernelStart = kernel + kID * (kernelSize.x * kernelSize.y * kernelSize.z);

        double sum = 0;
        for(u_int ky = 0; ky < kernelSize.y; ky++){
            for(u_int kx = 0; kx < kernelSize.x; kx++){
                sum += kernelStart[getDataIndex(kernelSize, {kx, ky, kPosZ})] * input[getDataIndex(inputSize, {kPosX + kx, kPosY + ky, kPosZ})];
            }
        }
        result[index] = sum;
    }
}

cn::Bitmap<double> cn::CUDAUtils::cudaConvolve(const std::vector<cn::Bitmap<double>> &kernels, const cn::Bitmap<double> &input, int paddingX, int paddingY, int strideX, int strideY) {
    double *kernelDev, *dataDev, *resDev;

    Bitmap<double> paddedInput = cn::Utils::addPadding(input, paddingX, paddingY);

    int sX = cn::Utils::afterConvolutionSize(kernels[0].w(), paddedInput.w(), paddingX, strideX);
    int sY = cn::Utils::afterConvolutionSize(kernels[0].h(), paddedInput.h(), paddingY, strideY);

    u_int kerSize = kernels[0].size().multiplyContent() * sizeof(double) * kernels.size();
    u_int dataSize = paddedInput.size().multiplyContent() * sizeof(double);
    u_int resultSize = sX * sY * kernels.size() * sizeof(double);

    kernelDev = (double *) fixedCudaMalloc(kerSize);
    dataDev = (double *) fixedCudaMalloc(dataSize);
    resDev = (double *) fixedCudaMalloc(resultSize);

    for(int i = 0; i < kernels.size(); i ++){
        cudaMemcpy(kernelDev + sX * sY * i, kernels[i].dataConst(), kernels[i].size().multiplyContent() * sizeof(double), cudaMemcpyHostToDevice);
    }

    cudaMemcpy(dataDev, paddedInput.dataConst(), dataSize, cudaMemcpyHostToDevice);

    Bitmap<double> result(sX, sY, kernels.size());

    int threadsCount = result.w() * result.h() * kernels.size() * paddedInput.d();

    constexpr int threadsPerBlock = 1024;


    dim3 inputSize = {static_cast<u_int>(paddedInput.w()), static_cast<u_int>(paddedInput.h()), static_cast<u_int>(paddedInput.d())};
    dim3 outputSize = {static_cast<u_int>(result.w()), static_cast<u_int>(result.h()), static_cast<u_int>(result.d())};
    dim3 kernelSize = {static_cast<u_int>(kernels[0].w()), static_cast<u_int>(kernels[0].h()), static_cast<u_int>(kernels[0].d())};

    cudaConvolveKernel<<<threadsCount/threadsPerBlock + 1, std::min(threadsCount, threadsPerBlock)>>>
    (
        dataDev,
        kernelDev,
        resDev,
        inputSize,
        outputSize,
        kernelSize
    );



    double *hostRes = new double[sX * sY * kernels.size()];
    cudaMemcpy(hostRes, resDev, resultSize, cudaMemcpyDeviceToHost);

    result.setData(hostRes);

    delete[] hostRes;

    cudaFree(kernelDev);
    cudaFree(dataDev);
    cudaFree(resDev);

    return result;
}



void *cn::CUDAUtils::fixedCudaMalloc(size_t size) {
    void* tmp;
    return cudaMalloc(&tmp, size) == cudaError::cudaSuccess ? tmp : nullptr;
}
