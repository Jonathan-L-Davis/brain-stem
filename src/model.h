#ifndef MODEL_H
#define MODEL_H

template<typename T>// Pretty angry with templates Right now. Should be easy to specify each layer at comptime in C++20, but I can't do that right now. Just doing the awful thing and doing everything at runtime for now.
struct model{
    
    std::vector<layer<T>> layers;
    
    tensor<T> operator () ( tensor<T> input ){
        tensor<T> retMe;
        
        for( int i = 0; i < layers.size(); i++ ){
            retMe = layers[i](retMe);
        }
        
        return retMe;
    }
};

#endif//MODEL_H

