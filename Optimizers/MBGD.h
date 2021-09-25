//
// Created by user on 21.08.2021.
//

#ifndef NEURALNETLIBRARY_MBGD_H
#define NEURALNETLIBRARY_MBGD_H
#include <vector>
#include "interfaces/Optimizer.h"
namespace cn {
    template<typename T>
    class Tensor;
    class Network;

    class MBGD : public Optimizer{
        int miniBatchSize;
        std::vector<std::vector<double>> memorizedWeights;
        std::vector<std::vector<double>> memorizedBiases;
    public:
        MBGD(Network &_network, double _learningRate, int _miniBatchSize);
        void propagate(const Tensor<double> &target, bool CUDAAccelerate = false) override;
    };

}
#endif //NEURALNETLIBRARY_MBGD_H
