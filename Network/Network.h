//
// Created by jrazek on 27.07.2021.
//

#ifndef NEURALNETLIBRARY_NETWORK_H
#define NEURALNETLIBRARY_NETWORK_H

#include <vector>
#include "Bitmap.h"

class Layer;
class FFLayer;
class ConvolutionLayer;

typedef unsigned char byte;

class Network {
protected:
    std::vector<Layer *> layers;
    friend class Layer;

    /**
     * if the first layer is image - set all the properties.
     * In case of using only FFLayers - set height and depth to 1.
     */
    const byte * data;
    int dataWidth;
    int dataHeight;
    int dataDepth;
    friend ConvolutionLayer;
public:

    void appendConvolutionLayer(int kernelX, int kernelY, int kernelZ, int kernelsCount);

    void appendLayer(FFLayer  * layer);
    void appendLayer(ConvolutionLayer * layer);

    /**
     * if first layer is convolution - use this feed method
     * @param data - data to convolve
     * @param w - width of tensor
     * @param h - height of tensor
     * @param d - depth of tensor
     */

    void feed(const byte *input, int w, int h, int d);

    /**
     * use this if and only if first layer is convolution
     * @param data
     */


    void feed(const Bitmap * const bitmap);


    const std::vector<Layer *> * getLayers();

    ~Network();
};


#endif //NEURALNETLIBRARY_NETWORK_H
