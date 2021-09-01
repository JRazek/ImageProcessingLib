//
// Created by jrazek on 19.08.2021.
//

#include "FlatteningLayer.h"
#include "../Network.h"

cn::FlatteningLayer::FlatteningLayer(int _id, Network &_network) : Layer(_id, _network) {
    inputSize = network->getInputSize(_id);
    int size = inputSize.multiplyContent();
    outputSize = Vector3<int>(size, 1, 1);
}

cn::Bitmap<double> cn::FlatteningLayer::run(const cn::Bitmap<double> &input) {
    if(input.size().multiplyContent() != inputSize.multiplyContent())
        throw std::logic_error("invalid input input for flattening layer!");
    return Bitmap<double>({inputSize.multiplyContent(), 1, 1}, input.data());
}

double cn::FlatteningLayer::getChain(const Vector3<int> &inputPos) {
    if(getMemoState(inputPos)){
        return getMemo(inputPos);
    }
    int outputIndex = network->getInput(__id)->getDataIndex(inputPos);
    double res = network->getChain(__id + 1, {outputIndex, 0, 0});
    setMemo(inputPos, res);
    return res;
}
