//#include <windows.h>
#include <stdio.h>
#include <iostream>
//#include <string>
#include <stdint.h>
#include <vector>
#include <assert.h>
#include <stdarg.h>

#include "utils.h"
#include "Debug.h"
#include "Tile.h"
#include "QuadPos.h"
#include "Tiles.h"

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