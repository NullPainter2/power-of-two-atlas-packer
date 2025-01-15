//#include <windows.h>
#include <stdio.h>
#include <iostream>
//#include <string>
#include <stdint.h>
#include <vector>
#include <assert.h>
#include <stdarg.h>

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

namespace Debug
{
    bool _debugPrintEnabled = false;

    void PrintF(const char* fmt, ...)
    {
        if (!_debugPrintEnabled)
        {
            return;
        }

        va_list list;
        va_start(list, fmt);
        vprintf(fmt, list);
        va_end(list);
    }

    void EnableDebugPrint(bool enabled)
    {
        _debugPrintEnabled = enabled;
    }
}

struct Tiles
{
    #define MAX_SIZE 256

    // to visualize it on console
    #define RESCALE 16

    struct Tile {
        int x = 0;
        int y = 0;
        int size = 0;
        char color = ' ';
    };
    
    std::vector<Tile> tiles;
    
    void AddTileTest( int x, int y, int size2n, char color )
    {
        Tile tile = {};
        tile.x=x * RESCALE;
        tile.y=y * RESCALE;
        tile.size=size2n;
        tile.color=color;
        
        tiles.push_back( tile );
    }
    
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

        void _SetTilePosition( Tile * tile )
        {
            // The counter is at the resolution of the added tile. Otherwise something is wrong.

            tile->x = 0;
            tile->y = 0;
            
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
                    tile->x += size; 
                }
                if( digit == 2 || digit == 3 )
                {
                    tile->y += size;
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
        
        bool Add(int addedSize, Tile * outTile )
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
            _SetTilePosition( outTile ); 

            _Increment();
            _DebugPrint("after incrementing");

            return true;
        }

        bool _debugPrintEnabled = false;

        void EnableDebugPrint(bool enabled)
        {
            _debugPrintEnabled = enabled;
        }        
    };    
    QuadPos _quadPos;   

    void AddBlock( int wantedSize, char color )
    {
        Debug::PrintF("Adding %dx%d\n", wantedSize, wantedSize);
        
        Tile tile = {};
        tile.color = color;
        tile.size = wantedSize;
        if(!_quadPos.Add(wantedSize, &tile))
        {
            Debug::PrintF("Failed to add the tile!\n");
        }
        else
        {
            Debug::PrintF("Added tile!\n");

            tiles.push_back(tile);
        }   
    }
    
    void Init()
    {
        _quadPos.Init(MAX_SIZE);
    }
    
    void Print()
    {
        
        int SIZE = MAX_SIZE / RESCALE;
        char texels[MAX_SIZE / RESCALE][MAX_SIZE / RESCALE];


        // init
        {
            // background

            for (int y = 0; y < SIZE; y++)
                for (int x = 0; x < SIZE; x++)
                {
                    texels[y][x] = ' ';
                }

            // set normal positions

            for (Tile& tile : tiles)
            {
                for (int y = 0; y < tile.size / RESCALE; y++)
                    for (int x = 0; x < tile.size / RESCALE; x++)
                    {
                        texels[y + tile.y / RESCALE][x + tile.x / RESCALE] = tile.color;
                    }
            }
        }

        printf("+-------------------+\n");
        printf("|                   |\n");
        printf("|       ATLAS       |\n");
        printf("|                   |\n");
        printf("+-------------------+\n");

        {
            // draw flipped

            printf("Resulting atlas:\n");

            printf("-----");
            for (int x = 0; x < SIZE; x++)
            {
                printf("-");
            }
            printf("\n");

            for (int y = SIZE - 1; y >= 0; y--)
            {
                printf("%3d |", y);
                for (int x = 0; x < SIZE; x++)
                {
                    printf("%c", texels[y][x]);
                }
                printf("\n");
            }

            printf("-----");
            for (int x = 0; x < SIZE; x++)
            {
                printf("-");
            }
            printf("\n");
        }

        {
            printf("Made by adding these tiles (resulting positions):\n");
            for (Tile& tile : tiles)
            {
                printf("'%c' %dx%d -> (%d %d)\n", tile.color, tile.size, tile.size, tile.x, tile.y);
            }
            printf("\n");
        }
    }
};

int main( int argc, char **argv )
{
    /* simple test to see if adding of rects works
    {
        Tiles tiles;
        
        tiles.Print();
        
        tiles.AddTileTest(0,0,128,'a');
        tiles.AddTileTest(2,4,16,'b');
        tiles.AddTileTest(2,7,32,'c');
        
        tiles.Print();
    }
    */

    Debug::EnableDebugPrint(false);

    // happy path
    {

        Tiles tiles;
        tiles.Init();
        tiles.AddBlock(128, 'a');
        tiles.AddBlock(64, 'b');
        tiles.AddBlock(64, 'c');
        tiles.AddBlock(64, 'd');
        tiles.AddBlock(32, 'e');
        tiles.AddBlock(32, 'f');
        tiles.AddBlock(32, 'g');
        tiles.AddBlock(32, 'h');
        tiles.AddBlock(32, 'i');
        tiles.AddBlock(32, 'j');
        tiles.AddBlock(32, 'k');
        tiles.AddBlock(16, 'l');
        tiles.AddBlock(16, 'm');
        tiles.AddBlock(16, 'n');
        tiles.AddBlock(16, 'o');
        tiles.AddBlock(16, 'p');
        tiles.AddBlock(16, 'q');
        tiles.AddBlock(16, 'r');
        tiles.Print();
    }

    // another example
    {
        Tiles tiles;
        tiles.Init();
        tiles.AddBlock(128, 'a');
        tiles.AddBlock(128, 'b');
        tiles.AddBlock(64, 'c');
        tiles.AddBlock(64, 'd');
        tiles.AddBlock(32, 'e');
        tiles.Print();
    }

    // too big to fit
    {
        Tiles tiles;
        tiles.Init();
        tiles.AddBlock(128, 'a');
        tiles.AddBlock(128, 'b');
        tiles.AddBlock(128, 'c');
        tiles.AddBlock(128, 'd');
        tiles.AddBlock(128, 'e');
        tiles.Print();
    }

    return 0;
}