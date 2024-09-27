#ifndef TENSOR_H
#define TENSOR_H

#include <assert.h>
#include <type_traits>
#include <vector>

template<typename T>
concept addition = requires(T a, T b){
    a + b;
    a += b;
};

template<typename T>
concept multiplication = requires(T a, T b){
    a * b;
};

template<typename T>
concept assignment = requires(T a, T b){
    a = b;
};

template<typename T>
requires addition<T> && multiplication<T> && assignment<T>
struct tensor{
    //indices is for when you swap the order of indices to keep track of that without moving tons of memory around. Most simple example is transposing a matrix.
    
    //do not need right now. I can rethink this part when I do more sophisticated reshaping. This is for a memcpy optimization. Not important.;std::vector<uint64_t> indices;// honestly, you could really get away with 8 bits for this. Who the hell is gonna use more than 256 dimensions for a tensor? That's either a ton of length one indices (which is retarded, and a waste of space), or at least 2^256 * sizeof(T) bytes of memory. No one has that much ram or storage. The world doesn't have that much ram or storage.
    std::vector<uint64_t> dims;// Really unlikely you're gonna use more than 4 gigs for a single tensor, but not unreasonable or impossible. Tempted to add asserts for that. John Carmack and Lex Fridman talked about intentionally making software break as you pass the limits of your expected values, because you likely need to rethink your base assumptions. https://www.youtube.com/watch?v=I845O57ZSy4 not rewatching to find the time stamp, it's 5 hours long. Good watch though, the whole thing is worth while.
    
    std::vector<T> data;//The actual data, which can be reinterpreted as a variable dimension array.
    
    T& operator [] (const std::vector<uint64_t> &index) {
        
        //assert( index.size() == indices.size() );
        assert( index.size() == dims.size() );
        
        for( unsigned int i = 0; i < index.size(); i++)
            assert( index[i] < dims[i] );//bounds checking
        
        uint64_t raw_index = 0;
        
        int jump = 1;
        for( int i = dims.size()-1; i > -1; i--){
            raw_index += index[i]*jump;
            jump *= dims[i];
        }
        
        return data[raw_index];//should check for null ptr
    };
    
    friend tensor<T> operator + ( const tensor<T> &a, const tensor<T> &b ){
        tensor<T> c;
        
        assert( a.data.size() == b.data.size() );
        
        c.data.resize( a.data.size() );
        c.dims = a.dims;
        
        for( int i = 0; i < a.data.size(); i++ )
            c.data[i] = a.data[i] + b.data[i];
        
        return c;
    };
    
    friend tensor<T> operator * ( const T &a, const tensor<T> &b ){
        tensor<T> c;
        
        c.data.resize( b.data.size() );
        
        for( unsigned int i = 0; i < b.data.size(); i++ )
            c.data[i] = a * b.data[i];
        
        return c;
    };
    
    friend tensor<T> operator * ( const tensor<T> &a, const T& b ){
        tensor<T> c;
        
        c.data.resize( a.data.size() );
        
        for( int i = 0; i < a.data.size(); i++ )
            c.data[i] = a.data[i] * b;
        
        return c;
    };
    
    tensor<T> operator = ( const tensor<T> b ){
        dims = b.dims;
        data = b.data;
    }
    
    tensor<T> dot (tensor<T> &b) {
        tensor<T> retMe;
        
        if( b.dims.size() == 0 ){
            assert( b.data.size() == 1 );
            return b.data[0]*(*this);
        }
        
        //sanity checks & assertions
        assert( dims.size() >= b.data.size() );
        
        for( int i = dims.size()-1; i >= (signed)b.dims.size(); i-- )
            assert( dims[i] == b.dims[ i-(dims.size()-b.dims.size()) ] );
        
        assert( data.size() >= b.data.size() );
        
        {
            unsigned int expected_size = 1;
            for( unsigned int i = 0; i < dims.size(); i++ )
                expected_size *= dims[i];
            assert( data.size() == expected_size );
        }
        
        {
            unsigned int expected_size = 1;
            for( unsigned int i = 0; i < b.dims.size(); i++ )
                expected_size *= dims[i];
            assert( b.data.size() == expected_size );
        }
        
        // allocate memory
        retMe.dims.resize( dims.size()-b.dims.size() );
        
        {
            uint64_t final_size = 1;
            for( unsigned int i = 0; i < dims.size()-b.dims.size(); i++ )
                final_size *= dims[i];
            retMe.data.resize(final_size);
        }
        
        //calculate the dot product.
        
        for( unsigned int i = 0; i < dims.size(); i++ )
            retMe.dims[i] = dims[i];
        
        //*
        std::vector<uint64_t> index(retMe.dims.size(),0);
        
        do{//hate that I'm using a do while for this, but it doesn't really work in a for loop or a while loop...
            T value{};// T ought to initialize to it's equivalent 0 value. 
            
            std::vector<uint64_t> inner_index(retMe.dims.size(),0);
            do{
                std::vector<uint64_t> idx = index;
                idx.insert(idx.end(),inner_index.begin(),inner_index.end());
                
                value += (*this)[idx]*b[inner_index];
                //std::cout << (int) value.data << std::endl;
            }while( !b.next_index(inner_index) );
            
            retMe[index] = value;
            
        }while( !retMe.next_index(index) );//*/
        
        return retMe;
    }
    
    bool next_index (std::vector<uint64_t> &index){
        bool rolled_over_to_0 = false;
        
        // these two check that the index is valid
        assert( index.size() == dims.size() );
        
        for( unsigned int i = 0; i < dims.size(); i++ )
            assert( index[i] >= 0 && index[i] < dims[i]);
        
        // do the incrementing
        for( int i = dims.size()-1; i > -1; i-- ){
            index[i]++;
            if( index[i] != dims[i] )
                goto end_increment;
            
            index[i] = 0;
            if( i == 0 ) rolled_over_to_0 = true;
        }end_increment:;
        
        return rolled_over_to_0;
    }
    
};

#endif//TENSOR_H
