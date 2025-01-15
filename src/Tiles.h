#pragma once

struct Tiles
{
    #define MAX_SIZE 256

    // to visualize it on console
    #define RESCALE 16
    
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
    
    QuadPos _quadPos;   

    void AddBlock( int wantedSize, char color )
    {
        Debug::PrintF("Adding %dx%d\n", wantedSize, wantedSize);
        
        Tile tile = {};
        tile.color = color;
        tile.size = wantedSize;
        if(!_quadPos.Add(wantedSize, &( tile.x ), &( tile.y )))
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
