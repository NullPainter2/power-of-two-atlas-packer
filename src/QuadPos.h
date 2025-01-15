#pragma once

struct QuadPos
{
    // Can be represented in binary too up to some level ... Hard to implement rn.
    std::vector<int> digits;

    int firstQuadSize = 0;
    int currentQuadSize = 0;

    int isValid = false;

    void Init(int size)
    {
        digits.clear();
        digits.push_back(0);

        // @fixme Maybe easy to handle starting at full-size?
        firstQuadSize = size / 2;
        currentQuadSize = size / 2;
        isValid = true;
    }

    void _DebugPrint( char * title)
    {
        Debug::PrintF("QUADPOS | %s = ( ", title);
        for (int digit : digits)
        {
            Debug::PrintF("%d ", digit);
        }
        Debug::PrintF(")\n");
    }

    void _MatchTheResolution( int newSize )
    {
        // The order of the boxes:
        // 
        //   +---+---+
        //   | 2 | 3 |
        //   +---+---+
        //   | 0 | 1 |
        //   +-------+

        // So. You can use 0,1,2 or 3 to index each.

        // Adding sub-quad means adding new dimension.

        // Which also indexes in this order.

        // So for example:

        //    first quad     second quad - we need new dimension now, because we split the quad

        //   +-----------+...............+
        //   |           |               .
        //   |           |               .
        //   |  (0)      +-------+       .
        //   |           | (1,0) |       .
        //   |           |       | <---------- we are on quad 1, and we added first subquad so 0
        //   +-----------+-------+.......+


        // Lets add few more ...

        //   +-----------+-------+
        //   |           | (1,2) |
        //   |           |       |
        //   |  (0)      +-------+-------+
        //   |           | (1,0) | (1,1) |
        //   |           |       |       |
        //   +-----------+-------+-------+

        // Anothe subquad ... again, add new dimension

        //   +-----------+-------+
        //   |           | (1,2) |--+          is on quad 1, subquad 3, and start new subquad 0
        //   |           |       |  |    <----  (1,3,0)
        //   |  (0)      +-------+-------+
        //   |           | (1,0) | (1,1) |
        //   |           |       |       |
        //   +-----------+-------+-------+
        // 
        //   +-----------+-------+
        //   |           |       |  (1,3,0)   (1,3,1)
        //   |           | (1,2) |---+---+ 
        //   |           |       |   |   | 
        //   |  (0)      +-------+-------+
        //   |           | (1,0) | (1,1) |
        //   |           |       |       |
        //   +-----------+-------+-------+            

        // Those number are like a path


        // Because this atlas only ever gets smaller, we never lose any "digits" of the dimension.

        assert( IsPowerOfTwo( newSize ));
        assert( newSize > 0 );
        assert( newSize <= currentQuadSize );

        // make sure the resolution is correct            
        while (newSize < currentQuadSize)
        {
            digits.push_back(0);
            currentQuadSize /= 2;
        }

        assert(newSize == currentQuadSize);
    }

    void _SetTilePosition( int * tileX, int * tileY )
    {
        // The counter is at the resolution of the added tile. Otherwise something is wrong.

        *tileX = 0;
        *tileY = 0;
        
        //
        //   ^ y
        //   |
        //   +---+---+
        //   | 2 | 3 |
        //   +---+---+
        //   | 0 | 1 |
        //   +-------+---> x
        //     ^
        //     |
        //   <quad size>
        //
        int size = firstQuadSize;
        for(int digit : digits)
        {
            if( digit == 1 || digit == 3 )
            {
                *tileX += size;
            }
            if( digit == 2 || digit == 3 )
            {
                *tileY += size;
            }

            size /= 2;
        }                        
    }

    void _Increment()
    {
        // It increments as regular digits. Only this one is in base 4.

        // It might be nice to implement it as regular i64, but it will limit atlas max size.

        assert(digits.size() > 0);

        int digitIndex = digits.size() - 1; // last digit
        while (true)
        {
            if (digitIndex < 0)
            {
                _DebugPrint("run out of digits");

                // @fixme Invalid state?
                assert(1 != 2);

                break;
            }

            int digit = digits[digitIndex];
            if (digit == 3)
            {
                if (digitIndex == 0)
                {
                    _DebugPrint("impossible to increment");
                    
                    isValid = false;
                    break;
                }

                digits[digitIndex] = 0;
                digitIndex -= 1; // bump previous digit by one too
            }
            else
            {
                digits[digitIndex] += 1;
                break;
            }
        }
    }
    
    bool Add(int addedSize, int * tileX, int * tileY )
    {
        assert(addedSize <= currentQuadSize);
        assert(addedSize > 0);

        if( !isValid )
        {
            _DebugPrint("Can't add, no more space");

            assert(0 == 0 && "Untested!");

            return false;
        }
        
        _DebugPrint("before matching resolution");
        
        _MatchTheResolution(addedSize);
        
        _DebugPrint("after correcting the resolution");

        // Current tile position is always correct, unless it is invalid.
        // There is space, because added tile is always <= than the resolution.
        _SetTilePosition( tileX, tileY ); 

        _Increment();
        
        _DebugPrint("after incrementing");

        return true;
    }    
};    
