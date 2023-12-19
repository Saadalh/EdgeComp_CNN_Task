#ifndef NETWORK_HPP
#define NETWORK_HPP

#include "tensor.hpp"

#include <cmath>
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
            in_channels_ = in_channels;
            output_.C = out_channels;
            kernel_size_ = kernel_size;
            stride_ = stride;
            pad_ = pad;
        }
    // TODO
    // inp = stride * out - pad + ker
    
        void fwd() override {
            float product_sum = 0;

            // What happens when there is padding? The padding should be probably added to the input tensor. For-loops?

            output_.H = (input_.H + (2*pad_) - weights_.H)/stride_ + 1;
            output_.W = (input_.W + (2*pad_) - weights_.W)/stride_ + 1;
            output_.N = input_.N;

                for(int n = 0; n < input_.N; n++) {
                    for(int c = 0; c < input_.C; c++) {
                        for(int h = 0; h < input_.H; h + stride_) {
                            for(int w = 0; w < input_.W; w + stride_) {
                                product_sum = 0;

                                for(int kh = 0; kh < kernel_size_; kh++) {
                                    for(int kw = 0; kw < kernel_size_; kw++) {
                                        product_sum =+ input_(n, c, h+kh, w+kw) * weights_(n, c, kh, kw); 
                                    }
                                }

                                output_(n, c, (h/stride_), (w/stride_)) = product_sum;
                            }
                        }
                    }
                }
        }

    private:
    size_t in_channels_, out_channels_, kernel_size_, stride_, pad_;
};


class Linear : public Layer {
    public:
        Linear(size_t in_features, size_t out_features) : Layer(LayerType::Linear) {}
    // TODO
        void fwd() {
            output_.C = input_.C;
            output_.N = input_.N;
            output_.H = input_.H;
            output_.W = input_.W;

            for(int n = 0; n < input_.N; n++) {
                for(int c = 0; c < input_.C; c++) {
                    for(int h = 0; h < input_.H; h++) {
                        for(int w = 0; w < input_.W; w++) {
                            output_(n, c, h, w) = input_(n, c, h, w) * weights_(n, c, h, w) + bias_(n, c, h, w);
                        }
                    }
                }
            }
        }
};


class MaxPool2d : public Layer {
    public:
        MaxPool2d(size_t kernel_size, size_t stride=1, size_t pad=0) : Layer(LayerType::MaxPool2d) {
            pad_ = pad;
            kernel_size_ = kernel_size;
            stride_ = stride;
        }
    // TODO
        void fwd() override {
            float max_value = 0;

            output_.H = ((input_.H + (2*pad_) - kernel_size_) / stride_) + 1;
            output_.W = ((input_.W + (2*pad_) - kernel_size_) / stride_) + 1;
            output_.C = input_.C;
            output_.N = input_.N;

            // What happens when there is padding? The padding should be probably added to the input tensor. For-loops?

            for(int n = 0; n < input_.N; n++) {
                for(int c = 0; c < input_.C; c++) {
                    for(int h = 0; h < input_.H; h + stride_) {
                        for(int w = 0; w < input_.W; w + stride_) {

                            for(int kh = 0; kh < kernel_size_; kh++) {
                                for(int kw = 0; kw < kernel_size_; kw++) {
                                    if(input_(n, c, h+kh, w+kw) > max_value) {
                                        max_value = input_(n, c, h+kh, w+kw);
                                    }
                                }
                            }

                            output_(n, c, (h/stride_), (w/stride_)) = max_value;

                        }
                    }
                }
            }
        }

    private:
    size_t pad_;
    size_t kernel_size_;
    size_t stride_;
};


class ReLu : public Layer {
    public:
        ReLu() : Layer(LayerType::ReLu) {}
    // TODO
        void fwd() override {
            output_.C = input_.C;
            output_.N = input_.N;
            output_.H = input_.H;
            output_.W = input_.W;
            
            for(int n = 0; n < input_.N; n++) {
                for(int c = 0; c < input_.C; c++) {
                    for(int h = 0; h < input_.H; h++) {
                        for(int w = 0; w < input_.W; w++) {
                            if(input_(n, c, h, w) > 0) {
                                output_(n, c, h, w) = input_(n, c, h, w);
                            }
                            else {
                                output_(n, c, h, w) = 0;
                            }
                        }
                    }
                }
            }
        }
};


class SoftMax : public Layer {
    public:
        SoftMax() : Layer(LayerType::SoftMax) {}
    // TODO
        void fwd() override {
            float exp_sum = 0;
            output_.N = 1;
            output_.C = 1;
            output_.H = input_.H;
            output_.W = 1;

            for(int h = 0; h < input_.H; h++) {
                exp_sum =+ exp(input_(0,0,h,0)) ;
            }

            for(int h = 0; h < output_.H; h++) {
                output_(0,0,h,0) = exp(input_(0,0,h,0)) / exp_sum;
            }
        }
};


class Flatten : public Layer {
    public:
        Flatten() : Layer(LayerType::Flatten) {}
    // TODO
        void fwd() override {
            output_.N = 1;
            output_.C = 1;
            output_.H = input_.C * input_.H * input_.W;
            output_.W = 1;

            for(int n = 0; n < input_.N; n++) {
                for(int c = 0; c < input_.C; c++) {
                    for(int h = 0; h < input_.H; h++) {
                        for(int w = 0; w < input_.W; w++) {
                            output_(0, 0, ((n * input_.C * input_.H * input_.W) + (c * input_.H * input_.W) + (h * input_.W) + w), 0);
                        }
                    }
                }
            }
        }
};


class NeuralNetwork {
    public:
        NeuralNetwork(bool debug=false) : debug_(debug) {}

        void add(Layer* layer) {
            // TODO
            layers_.push_back(layer);
        }

        void load(std::string file) {
            // TODO
            // Will this function load the parameters of all the NN layers? Or just for one layer?
            // Read the raw file and assign each layer parameter list to the corresponding layer.
        }

        Tensor predict(Tensor input) {
            // TODO
            for(int i = 0; i < layers_.size(); i++) {
                layers_[i]->fwd();
            }
        }

    private:
        bool debug_;
        // TODO: storage for layers
        std::vector<Layer*> layers_;
};

#endif // NETWORK_HPP
