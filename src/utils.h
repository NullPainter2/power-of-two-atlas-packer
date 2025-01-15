#pragma once

bool IsPowerOfTwo( int x )
{
    // mask around for power of 2
    //                           000100  for example this is the power of 2 number
    //  u - 1                    000011  bits to right of that 1
    //  (~(u-1)) << 1            111000  bits to left of that 1
    //  (u-1) | ((~(u-1))<<1)    left combined with right
    
    //   this_thing == ~tested_number    is what I assume holds    
    // - does it work for 0 ?
    // - does it work for < 0 ?
    
    unsigned int u = x;

    int oneCount = 0;

    for(int i=0;i<sizeof(x)*8;i++)
    {
        if(u & (1 << i))
        {
            oneCount++;
        }
    }    

    return oneCount == 1;
}
