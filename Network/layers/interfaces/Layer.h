//
// Created by jrazek on 27.07.2021.
//

#ifndef NEURALNETLIBRARY_LAYER_H
#define NEURALNETLIBRARY_LAYER_H

#include <vector>
#include <optional>
#include "../../../Utils/Bitmap.h"
namespace cn {

    class Network;

    class Layer {
    protected:
        Network * network;
    public:
        std::optional<Bitmap<float>> output;

        const int id;
        Layer(int _id, Network * _network);

        Layer(Layer &layer) = delete;

        virtual void run(const Bitmap<float> &bitmap) = 0;
        virtual ~Layer() = default;
    };
}


#endif //NEURALNETLIBRARY_LAYER_H
