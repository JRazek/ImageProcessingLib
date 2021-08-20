//
// Created by jrazek on 27.07.2021.
//

#ifndef NEURALNETLIBRARY_CONVOLUTIONLAYER_H
#define NEURALNETLIBRARY_CONVOLUTIONLAYER_H

#include "interfaces/Layer.h"
#include "interfaces/RandomInitiable.h"

template<typename T>
struct Vector3;
namespace cn {
    class ConvolutionLayer : public cn::Layer, public RandomInitiable{
    private:
        const int kernelSizeX;
        const int kernelSizeY;
        const int kernelSizeZ;
        const int kernelsCount;
        const int paddingX;
        const int paddingY;
        const int strideX;
        const int strideY;

        const DifferentiableFunction &activationFunction;

        std::vector<Bitmap<float>> kernels;
        std::vector<float> biases;//ith corresponds to ith kernel

    public:
        ConvolutionLayer(int _id, cn::Network *_network, int _kernelSizeX, int _kernelSizeY,
                         int _kernelsCount, const DifferentiableFunction &_activationFunction,
                         int _paddingX, int _paddingY, int _strideX, int _strideY);
        void randomInit() override;
        void run(const Bitmap<float> &bitmap) override;
    };
}


#endif //NEURALNETLIBRARY_CONVOLUTIONLAYER_H
