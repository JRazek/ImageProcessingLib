//
// Created by jrazek on 27.07.2021.
//

#ifndef NEURALNETLIBRARY_CONVOLUTIONLAYER_H
#define NEURALNETLIBRARY_CONVOLUTIONLAYER_H

#include "interfaces/Learnable.h"

template<typename T>
struct Vector3;
namespace cn {
    class ConvolutionLayer : public Learnable{
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

        float diffWeight(int weightID);

    public:
        ConvolutionLayer(int _id, Network &_network, int _kernelSizeX, int _kernelSizeY,
                         int _kernelsCount, const DifferentiableFunction &_activationFunction,
                         int _paddingX, int _paddingY, int _strideX, int _strideY);
        void randomInit() override;
        void run(const Bitmap<float> &bitmap) override;
        float getChain(const Vector3<int> &input) override;
        int weightsCount() const override;

        virtual std::vector<float> getGradient() override;

        void setWeight(int absoluteWeightID, float value) override;
        float getWeight(int absoluteWeightID) const override;
    };
}


#endif //NEURALNETLIBRARY_CONVOLUTIONLAYER_H
