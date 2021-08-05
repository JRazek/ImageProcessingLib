//
// Created by jrazek on 27.07.2021.
//

#ifndef NEURALNETLIBRARY_LAYER_H
#define NEURALNETLIBRARY_LAYER_H

#include <vector>
#include "../Bitmap.h"
namespace cn {
    typedef unsigned char byte;

    class Network;

    class Layer {
    protected:
        Network * network;
    public:
        const int id;
        Layer(int id, Network * network);
        virtual void run(Bitmap<float> *bitmap) = 0;
        virtual ~Layer();
    };
}


#endif //NEURALNETLIBRARY_LAYER_H
