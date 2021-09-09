//
// Created by user on 20.08.2021.
//

#include "MaxPoolingLayer.h"
#include "../Network.h"

cn::Bitmap<double> cn::MaxPoolingLayer::run(const cn::Bitmap<double> &input) {
    if(input.size() != inputSize){
        throw std::logic_error("invalid output size in max pool!");
    }

    std::fill(mapping->data(), mapping->data() + mapping->size().multiplyContent(), Vector2<int>(-1, -1));

    Bitmap<double> res(outputSize);

    for(int c = 0; c < input.d(); c++){
        for(int y = 0; y < input.h() - kernelSize.y + 1; y += kernelSize.y){
            for(int x = 0; x < input.w() - kernelSize.x + 1; x += kernelSize.x){
                double max = 0;
                Vector2<int> bestPoint;
                for(int kY = 0; kY < kernelSize.y; kY++){
                    for(int kX = 0; kX < kernelSize.x; kX++){
                        max = std::max(input.getCell(x + kX, y + kY, c), max);
                        bestPoint = Vector2<int>(x + kX, y + kY);
                    }
                }
                res.setCell(x / kernelSize.x, y / kernelSize.y, c, max);
                mapping->setCell(bestPoint.x, bestPoint.x, c, {x / kernelSize.x, y / kernelSize.y});
            }
        }
    }
    output.emplace(res);
    return res;
}

double cn::MaxPoolingLayer::getChain(const Vector3<int> &inputPos) {
    if(getMemoState(inputPos)){
        return getMemo(inputPos);
    }
    Vector2<int> mapped = mapping->getCell(inputPos);
    double res;
    if(mapped == Vector2<int>(-1, -1))
        res = 0;
    else
        res = nextLayer->getChain({mapped.x, mapped.y, inputPos.z});
    setMemo(inputPos, res);
    return res;
}

cn::JSON cn::MaxPoolingLayer::jsonEncode() const {
    JSON structure;
    structure["id"] = __id;
    structure["input_size"] = inputSize.jsonEncode();
    structure["type"] = "mpl";
    structure["kernel_size"] = kernelSize.jsonEncode();
    return structure;
}

cn::MaxPoolingLayer::MaxPoolingLayer(const cn::JSON &json) :
MaxPoolingLayer(json.at("id"), json.at("input_size"), json.at("kernel_size")) {}

cn::MaxPoolingLayer::MaxPoolingLayer(int _id, Vector3<int> _inputSize, cn::Vector2<int> _kernelSize) :
        Layer(_id, _inputSize),
kernelSize(_kernelSize){
    outputSize = Vector3<int>(Utils::afterMaxPoolSize(kernelSize.x, inputSize.x), Utils::afterMaxPoolSize(kernelSize.y, inputSize.y), inputSize.z);
    mapping.emplace(Bitmap<Vector2<int>>(inputSize));
}

std::unique_ptr<cn::Layer> cn::MaxPoolingLayer::getCopyAsUniquePtr() const {
    return std::make_unique<MaxPoolingLayer>(*this);
}
