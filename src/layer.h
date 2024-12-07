#ifndef LAYER_H
#define LAYER_H

#include "tensor.h"
#include <cassert>
#include <vector>

template<typename T>
struct layer{
    
    const std::vector<uint64_t>  input_dims,output_dims;// basically for runtime type checking of tensor size. Could be largely comptime with C++ 20. Oh well.
    
    tensor<T> input;
    tensor<T> weights;
    tensor<T> bias;
    tensor<T> output;
    
    tensor<T>(*const forward_prop)(tensor<T>,tensor<T>,tensor<T>);
    tensor<T>(*backward_prop)(tensor<T>);
    
    layer()=delete;
    layer( std::vector<uint64_t> _input_dims, std::vector<uint64_t> _output_dims, tensor<T>(*_forward_prop)(tensor<T>,tensor<T>,tensor<T>), tensor<T>(*_backward_prop)(tensor<T>) ):input_dims(_input_dims), output_dims(_output_dims), forward_prop(_forward_prop), backward_prop(_backward_prop) {};
    
    tensor<T> operator () ( tensor<T> _input ){
        input = _input;
        output.data.clear;
        output.resize(output_dims);
        
        assert( input_dims.size() == input.dims.size() );
        for( int i = 0; i < input.dims.size(); i++)
            assert(input.dims[i] == input_dims[i]);
        
        output = forward_prop(input,weights,bias);
        
        assert( output_dims.size() == output.dims.size() );
        for( int i = 0; i < output.dims.size(); i++ )
            assert( output.dims[i] == output_dims[i] );
        
        return output;
    }
};

#endif//LAYER_H
