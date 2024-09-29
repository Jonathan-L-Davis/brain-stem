#include <iostream>

#include "tensor.h"

#include "z3.h"

int main(){
    
    std::cout << "brain-stem architecture" << std::endl;
    
    tensor<float> a;
    
    a.dims = {2,2};
    a.data = {1,-2,
              3,1}
    ;
    
    a.reshape({4});
    
    //tensor<float> b = a.swap_indices(0,1).reshape({4});
    tensor<float> b = a.swap_indices(0,0).reshape({4});
    
    tensor<float> c = {{4},{1,-1,1,-1}};
    
    std::cout << b.dot(c)[{}] << std::endl;//love this indexing method
    
    return 0;
}
