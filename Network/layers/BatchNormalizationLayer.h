//
// Created by user on 20.08.2021.
//

#ifndef NEURALNETLIBRARY_BATCHNORMALIZATIONLAYER_H
#define NEURALNETLIBRARY_BATCHNORMALIZATIONLAYER_H

#include "interfaces/Layer.h"
namespace cn {
    class BatchNormalizationLayer : public Layer {

        double normalizationFactor;
    public:
        BatchNormalizationLayer(int _id, Network &_network);
        Bitmap<double> run(const Bitmap<double> &input) override;
        double getChain(const Vector3<int> &inputPos) override;
    };
}
#endif //NEURALNETLIBRARY_BATCHNORMALIZATIONLAYER_H
