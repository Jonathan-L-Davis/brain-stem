#include <iostream>

#include "tensor.h"
#include "z3.h"
#include "layer.h"
#include "model.h"

tensor<float> forward_prop_l1( tensor<float> input, tensor<float> weights, tensor<float> bias ){//identity function on float valued scalar
    tensor<float> output;
    output.data.resize({});
    
    assert( input.dims.size() == 0 && input.data.size() == 1 );
    assert( output.dims.size() == 0 && output.data.size() == 1 );
    
    output.data[0] = input.data[0];
    
    return output;
}

tensor<float> backward_prop_l1( tensor<float> error ){//identity function on float valued scalar
    tensor<float> output;
    
    return output;
}

int main(){
    
    layer<float> l1({},{},forward_prop_l1,backward_prop_l1);
    
    model<float> not_lenet_yet;
    not_lenet_yet.layers.push_back(l1);
    
    return 0;
}
