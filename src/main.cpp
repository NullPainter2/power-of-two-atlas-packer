//#include <windows.h>
#include <stdio.h>
#include <iostream>
//#include <string>
#include <stdint.h>
#include <vector>
#include <assert.h>

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

struct Tiles
{
    #define MAX_SIZE 256
    // to visualize it on console
    #define RESCALE 16

    struct Tile {
        int x = 0;
        int y = 0;
        int size = 0;
        //int w = 0;
        //int h = 0;
        char color = ' ';
    };
    
    std::vector<Tile> tiles;
    
    void AddTile( int x, int y, int size2n, char color )
    {
        Tile tile = {};
        tile.x=x;
        tile.y=y;
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
            firstQuadSize = size/2;
            currentQuadSize = size/2;
            isValid = true;
        }

        void _Print()
        {
            printf("quad pos:");
            for (int digit : digits)
            {
                printf("%d ", digit);
            }
            printf("\n");
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
        
        bool Add(int addedSize, Tile * outTile )
        {
            printf("QUADPOS: Current quad pos:\n");
            _Print();

            // Simplifies the algo
            assert( addedSize <= currentQuadSize );
            assert( addedSize > 0 );
            
            if( !isValid )
            {
                printf("No more space for %dx%d\n",addedSize,addedSize);
                return false;
            }
            
            // make sure the resolution is correct            
            while( addedSize < currentQuadSize )
            {
                digits.push_back(0);
                currentQuadSize /= 2;
            }          

            assert( addedSize == currentQuadSize );
             
            printf("QUADPOS: After correcting the resolution\n");
            _Print();

            outTile->size = addedSize; // @cleanup Maybe add outside of this function?
            _SetTilePosition( outTile ); 

            // next number
            
            assert(digits.size() > 0);

            int digitIndex = digits.size()-1; // last digit
            while( true )
            {
                if( digitIndex < 0 )
                {
                    break;
                }
                
                int digit = digits[ digitIndex ];
                if( digit == 3 )
                {
                    if( digitIndex == 0 )
                    {
                        printf("Last value added by %dx%d, impossible to continue\n", addedSize, addedSize );
                        isValid = false;
                        break;
                    }
                    
                    digits[ digitIndex ] = 0;
                    digitIndex -= 1; // bump previous digit by one too
                }
                else
                {
                    digits[ digitIndex ] += 1;
                    break;
                }
            }

            printf("QUADPOS: After incrementing\n");
            _Print();

            return true;
        }
        
    };    
    QuadPos _quadPos;
    
    void AddBlock( int size2n, char color )
    {
        printf("adding %dx%d",size2n,size2n);
        
        int wantedSize = size2n; //
        
        Tile tile = {};
        tile.color = color;
        
        if(!_quadPos.Add(wantedSize, &tile))
        {
            printf("Failed to add the tile!\n");
        }
        else
        {
            printf("Added tile!\n");
            tiles.push_back(tile);
        }
        
/*        if(_quadPos.CanFit(wantedSize))
        {
            _quadPos.Add(wantedSize);
        }
        else
        {
            printf(" ... impossible to add this rect!\n");
        }
*/    
    }
    
    void Init()
    {
        _quadPos.Init(MAX_SIZE);
    }
    
    void Print()
    {
        int SIZE = MAX_SIZE/RESCALE;
        char texels[MAX_SIZE/RESCALE][MAX_SIZE/RESCALE];

        // background
        
        for(int y=0;y<SIZE;y++)
        for(int x=0;x<SIZE;x++)
        {
            texels[y][x] = ' ';           
        }
        
        // set normal positions
        
        for(Tile &tile: tiles)
        {
            for(int y=0;y<tile.size/RESCALE;y++)
            for(int x=0;x<tile.size/RESCALE;x++)
            {
                texels[y+tile.y/RESCALE][x+tile.x/ RESCALE] = tile.color;
            }            
        }
        

        // draw flipped

        printf("-----");
        for(int x=0;x<SIZE;x++)
        {
            printf("-");
        }
        printf("\n");
        
        for(int y=SIZE-1; y>=0;y--)
        {
            printf("%3d |",y);
            for(int x=0;x<SIZE;x++)
            {
                printf("%c",texels[y][x]);
            }
            printf("\n");
        }

        printf("-----");
        for(int x=0;x<SIZE;x++)
        {
            printf("-");
        }
        printf("\n");

        printf("Tiles:\n");
        for (Tile& tile : tiles)
        {
            printf("('%c' %d %d %dx%d)\n", tile.color, tile.x, tile.y, tile.size, tile.size);
        }
        printf("\n");
    }
};

int main( int argc, char **argv )
{
    if(0)
    {
        Tiles tiles;
        
        tiles.Print();
        
        tiles.AddTile(0,0,32,'a');
        tiles.AddTile(2,4,16,'b');
        tiles.AddTile(2,7,32,'c');
        
        tiles.Print();
    }
    
    Tiles tiles;
    tiles.Init();
    tiles.AddBlock(128,'a');
    tiles.AddBlock(64,'b');
    tiles.AddBlock(64,'c');
    tiles.AddBlock(64,'d');
    tiles.AddBlock(32,'e');
    tiles.AddBlock(32,'f');
    tiles.AddBlock(32,'g');
    tiles.AddBlock(32,'h');
    tiles.AddBlock(32,'i');
    tiles.AddBlock(32,'j');
    tiles.AddBlock(32,'k');
    tiles.AddBlock(16,'l');
    tiles.AddBlock(16,'m');
    tiles.AddBlock(16,'n');
    tiles.Print();
/*
    Pos pos = 0;    
    pos = Add( pos, 128 );
    pos = Add( pos, 64 );
    pos = Add( pos, 64 );
    pos = Add( pos, 64 );
    pos = Add( pos, 64 );
    pos = Add( pos, 32 );
    pos = Add( pos, 16 );
*/
    return 0;
}