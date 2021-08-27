//
// Created by jrazek on 27.07.2021.
//
#include "ConvolutionLayer.h"
#include "../Network.h"

cn::ConvolutionLayer::ConvolutionLayer(int _id, Network &_network, int _kernelSizeX, int _kernelSizeY,
                                       int _kernelsCount, const DifferentiableFunction &_activationFunction,
                                       int _paddingX, int _paddingY, int _strideX, int _strideY) :
        kernelSizeX(_kernelSizeX),
        kernelSizeY(_kernelSizeY),
        kernelSizeZ(_id == 0 ? _network.inputDataDepth :network->getLayers()->at(__id - 1)->getOutput()->d()),
        kernelsCount(_kernelsCount),
        activationFunction(_activationFunction),
        paddingX(_paddingX),
        paddingY(_paddingY),
        strideX(_strideX),
        strideY(_strideY),
        biases(kernelsCount),
        cn::Learnable(_id, _network, _kernelsCount) {

    int inputX, inputY;
    int sizeX, sizeY, sizeZ;
    if(__id == 0){
        inputX = network->inputDataWidth;
        inputY = network->inputDataHeight;
    }else{
        inputX = network->getLayers()->at(__id - 1)->getOutput()->w();
        inputY = network->getLayers()->at(__id - 1)->getOutput()->h();
    }

    kernels.reserve(_kernelsCount);

    for(int i = 0; i < _kernelsCount; i ++){
        kernels.emplace_back(kernelSizeX, kernelSizeY, kernelSizeZ);
    }

    sizeX = Utils::afterConvolutionSize(kernelSizeX, inputX, paddingX, strideX);
    sizeY = Utils::afterConvolutionSize(kernelSizeY, inputY, paddingY, strideY);
    sizeZ = kernelsCount;

    beforeActivation.emplace(Bitmap<float>(sizeX, sizeY, sizeZ));
    output.emplace(Bitmap<float>(sizeX, sizeY, sizeZ));
}

void cn::ConvolutionLayer::run(const Bitmap<float> &input) {
    _input = &input;
    //convolve input - must have correct sizes etc. Garbage in garbage out.
    int outW = Utils::afterConvolutionSize(kernelSizeX, input.w(), paddingX, strideX);
    int outH = Utils::afterConvolutionSize(kernelSizeY, input.h(), paddingY, strideY);

    for(int i = 0; i < kernelsCount; i ++){
        Bitmap<float> layer = Utils::sumBitmapLayers(Utils::convolve(kernels[i], input, paddingX, paddingY, strideX, strideY));
        beforeActivation->setLayer(i, layer.data());
    }
    for(auto it = beforeActivation->data(); it != beforeActivation->data() + beforeActivation->w() * beforeActivation->h() * beforeActivation->d(); ++it){
        int index = it - beforeActivation->data();
        *(output->data() + index) = activationFunction.func(*it);
    }
}

void cn::ConvolutionLayer::randomInit() {
    for(auto &k : kernels){
        for(auto it = k.data(); it != k.data() + k.w() * k.h() * k.d(); ++it){
            *it = network->getRandom(-1, 1);
        }
    }
    for(auto &b : biases){
        b = network->getRandom(-5, 5);
    }
}

float cn::ConvolutionLayer::getChain(const Vector3<int> &inputPos) {

    return 0;
}

float cn::ConvolutionLayer::diffWeight(int weightID) {
    int kSize = kernelSizeX * kernelSizeY * kernelSizeZ;
    Bitmap<float> paddedInput = Utils::addPadding(*_input, paddingX, paddingY);
    Vector3<int> weightPos = kernels[weightID / kSize].indexToVector(weightID % kSize);
    int kID = weightID / kSize;

    float result = 0;
    for(int y = 0; y < output->h() - kernelSizeY; y++){
        for(int x = 0; x < output->w() - kernelSizeX; x++){
            int inputX = x * strideX + weightPos.x;
            int inputY = y * strideY + weightPos.y;
            float inputValue = _input->getCell(inputX, inputY, weightPos.z);
            Vector3<int> nextPos (x, y, kID);
            result += inputValue * activationFunction.derive(beforeActivation->getCell(nextPos)) * network->getLayers()->at(__id + 1)->getChain(nextPos);
        }
    }
    return result;
}

int cn::ConvolutionLayer::weightsCount() const {
    return kernelSizeX * kernelSizeY * kernelSizeZ * kernelsCount;
}

std::vector<float> cn::ConvolutionLayer::getGradient() {
    std::vector<float> gradient(weightsCount());
    for(int i = 0; i < weightsCount(); i ++){
        gradient[i] = diffWeight(i);
    }
    return gradient;
}

void cn::ConvolutionLayer::setWeight(int weightID, float value) {

}

float cn::ConvolutionLayer::getWeight(int weightID) const {
    int kSize = kernelSizeX * kernelSizeY * kernelSizeZ;
    return *kernels[weightID / (kSize)].data() + (weightID % kSize);
}

