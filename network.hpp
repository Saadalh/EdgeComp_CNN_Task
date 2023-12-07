#ifndef NETWORK_HPP
#define NETWORK_HPP

#include "tensor.hpp"

#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>

enum class LayerType : uint8_t {
    Conv2d = 0,
    Linear,
    MaxPool2d,
    ReLu,
    SoftMax,
    Flatten // reshapes from 2D to 1D array
};

std::ostream& operator<< (std::ostream& os, LayerType layer_type) {
    switch (layer_type) {
        case LayerType::Conv2d:     return os << "Conv2d";
        case LayerType::Linear:     return os << "Linear";
        case LayerType::MaxPool2d:  return os << "MaxPool2d";
        case LayerType::ReLu:       return os << "ReLu";
        case LayerType::SoftMax:    return os << "SoftMax";
        case LayerType::Flatten:    return os << "Flatten";
    };
    return os << static_cast<std::uint8_t>(layer_type);
}

class Layer {
    public:
        Layer(LayerType layer_type) : layer_type_(layer_type), input_(), weights_(), bias_(), output_() {}

        virtual void fwd() = 0;
        virtual void read_weights_bias(std::ifstream& is) = 0;

        void print() {
            std::cout << layer_type_ << std::endl;
            if (!input_.empty())   std::cout << "  input: "   << input_   << std::endl;
            if (!weights_.empty()) std::cout << "  weights: " << weights_ << std::endl;
            if (!bias_.empty())    std::cout << "  bias: "    << bias_    << std::endl;
            if (!output_.empty())  std::cout << "  output: "  << output_  << std::endl;
        }
        // TODO: additional required methods

    protected:
        const LayerType layer_type_;
        Tensor input_;
        Tensor weights_;
        Tensor bias_;
        Tensor output_;
};


class Conv2d : public Layer {
    public:
        Conv2d(size_t in_channels, size_t out_channels, size_t kernel_size, size_t stride=1, size_t pad=0) : Layer(LayerType::Conv2d) {
            in_channels = this->in_channels;
            out_channels = this->out_channels;
            kernel_size = this->kernel_size;
            stride = this->stride;
            pad = this->pad;
        }
    // TODO
    // inp = stride * out - pad + ker

    void fwd() {
        output_.H = input_.H - weights_.H + 1;
        output_.W = input_.W - weights_.W + 1;
        output_.C = input_.C;
        output_.N = input_.N;

        std::vector<size_t> input_rows, input_cols;

        for(int on = 0; on < output_.N; on++) {
            for(int oc = 0; oc < output_.C; oc++) {
                for(int oh = 0; oh < (output_.H); oh++) {
                    for(int ow = 0; ow < (output_.W); ow++) {
                        for(int kw = 0; kw < kernel_size; kw++) {
                            input_rows.push_back(stride * ow - pad + kw); 
                            input_cols.push_back(stride * oh - pad + kw); 
                        }
                        
                        for(int x = 0; x < input_rows.size(); x++) {
                            // convolute the elements and 
                            // write the result to output_(on,oc,oh,ow)
                            output_.write()
                        }
                    }
                }
            }
        }
        float out1 = input_(0,0,0,0) 
    }

    private:
    size_t in_channels, out_channels, kernel_size, stride, pad;
};


class Linear : public Layer {
    public:
        Linear(size_t in_features, size_t out_features) : Layer(LayerType::Linear) {}
    // TODO
};


class MaxPool2d : public Layer {
    public:
        MaxPool2d(size_t kernel_size, size_t stride=1, size_t pad=0) : Layer(LayerType::MaxPool2d) {}
    // TODO
};


class ReLu : public Layer {
    public:
        ReLu() : Layer(LayerType::ReLu) {}
    // TODO
};


class SoftMax : public Layer {
    public:
        SoftMax() : Layer(LayerType::SoftMax) {}
    // TODO
};


class Flatten : public Layer {
    public:
        Flatten() : Layer(LayerType::Flatten) {}
    // TODO
};


class NeuralNetwork {
    public:
        NeuralNetwork(bool debug=false) : debug_(debug) {}

        void add(Layer* layer) {
            // TODO
        }

        void load(std::string file) {
            // TODO
        }

        Tensor predict(Tensor input) {
            // TODO
        }

    private:
        bool debug_;
        // TODO: storage for layers
};

#endif // NETWORK_HPP
