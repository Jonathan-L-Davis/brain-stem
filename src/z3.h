#ifndef Z3_H
#define Z3_H

#include <cstdint>

// This is the field Z3, except we interpret 2 as -1.
// Some necessary optimizations before this goes on the Tenstorrent card is to bit pack, & to then implement + & * with a small number of bitwise ops (less than 6-8 preferably).
struct Z3{
    uint8_t data;
    
    Z3():data(0){};
    Z3(uint8_t a) { data = a; assert( (a&0xFC) == 0) ; assert( a != 2 ); }
    
    friend Z3 operator + (Z3 a, Z3 b){
        Z3 c;
        
        assert( (a.data&0xFC) == 0) ;
        assert( a.data != 2 );
        assert( (b.data&0xFC) == 0);
        assert( b.data != 2 );
        
        //std::cout << (int) (a.data<<2|b.data) << std::endl;
        
        switch(a.data<<2|b.data){
            case 0x0: c.data = 0x0; break;
            case 0x1: c.data = 0x1; break;
            case 0x3: c.data = 0x3; break;
            case 0x4: c.data = 0x1; break;
            case 0x5: c.data = 0x3; break;
            case 0x7: c.data = 0x0; break;
            case 0xC: c.data = 0x3; break;
            case 0xD: c.data = 0x0; break;
            case 0xF: c.data = 0x1; break;
        }
        
        assert( (c.data&0xFC) == 0);
        assert( c.data != 2 );
        
        return c;
    }
    
    friend Z3 operator * (Z3 a, Z3 b){
        Z3 c;
        
        assert( (a.data&0xFC) == 0) ;
        assert( a.data != 2 );
        assert( (b.data&0xFC) == 0);
        assert( b.data != 2 );
        
        switch(a.data<<2|b.data){
            case 0x0: c.data = 0x0; break;
            case 0x1: c.data = 0x0; break;
            case 0x3: c.data = 0x0; break;
            case 0x4: c.data = 0x0; break;
            case 0x5: c.data = 0x1; break;
            case 0x7: c.data = 0x3; break;
            case 0xC: c.data = 0x0; break;
            case 0xD: c.data = 0x3; break;
            case 0xF: c.data = 0x1; break;
        }
        
        assert( (c.data&0xFC) == 0);
        assert( c.data != 2 );
        
        return c;
    }
    
    Z3 operator = (const Z3 b){
        data = b.data;
        return b;
    }
    
    friend Z3 operator += (Z3 &a,const Z3 b){
        a = a+b;
        return a;
    }
    
    //this is responsible for the mixed precision floating point operations. I'll need to re-implement this repeatedly. Once again for doubles. And again with f16's to run on my tenstorrent card. This'll be a good first implementation.
    friend float operator * (Z3 a, float b){
        float retMe;
        
        union{
            float f;
            uint32_t raw_float;
        } punster;
        
        punster.f = -0.0f;
        
        uint32_t sign_mask = punster.raw_float;
        uint32_t bits_mask = ~sign_mask;
        
        punster.f = b;
        
        uint32_t sign = sign_mask & ( (punster.raw_float) ^ (0xFFFFFFFF*( (a.data>>1)&0x1 ) ) );
        uint32_t bits = bits_mask & ( (punster.raw_float) & (0xFFFFFFFF*( (a.data>>0)&0x1 ) ) );
        
        punster.raw_float = sign|bits;
        
        retMe = punster.f;
        
        return retMe;
    }
    
    friend float operator * (float a, Z3 b){
        float retMe;
        
        union{
            float f;
            uint32_t raw_float;
        } punster;
        
        punster.f = -0.0f;
        
        uint32_t sign_mask = punster.raw_float;
        uint32_t bits_mask = ~sign_mask;
        
        punster.f = a;
        
        uint32_t sign = sign_mask & ( (punster.raw_float) ^ (0xFFFFFFFF*( (b.data>>1)&0x1 ) ) );
        uint32_t bits = bits_mask & ( (punster.raw_float) & (0xFFFFFFFF*( (b.data>>0)&0x1 ) ) );
        
        punster.raw_float = sign|bits;
        
        retMe = punster.f;
        
        return retMe;
    }
    
};

#endif//Z3_H
