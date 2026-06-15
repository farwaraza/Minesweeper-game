#include "raylib.h"
#include <stdlib.h>

#define WIDTH 600
#define HEIGHT 600

#define ROWS 12
#define COLUMNS 12

const int tile_width=WIDTH/COLUMNS;
const int tile_height=HEIGHT/ROWS;


//structure for tiles and relevant entities
typedef struct{
    int x;
    int y;
    bool mine;
    bool revealed;
    bool flagged;
    int neighbor_mine;
} sTile;

sTile grid[COLUMNS][ROWS];

int revealed_count;
int mine_count=24;

//constant strings to be used in different game_states
const char* label_win="YOU WIN!!";
const char* label_lose="KABOOM!"; 
const char* label_enter="Press ENTER to play again.";

#define max_textures 2

//enumerating the textures used
typedef enum {
    texture_flag=0,
    texture_bomb=1
} assets; 

Texture2D textures[max_textures];

//enumerating the different game states
typedef enum{
    main_menu=0,
    playing,
    win,
    lose
} states;

states game_state;

//function prototypes
void start();
void update();
void render();
void reset();
void shut_down();
int nearby_mine(int, int);
bool valid(int,int);
void reveal(int,int);
void reveal_all();
void flag (int,int);
void cascade(int,int);

void render_tiles();
void draw_tiles(sTile tile);
void reset_tiles();

/*runs nested loop to check around a cell to see
if mines are nearby and save the total value*/
int nearby_mine(int col,int row) 
{
    int count=0;
    for (int dx=-1;dx<=1;dx++)
    {
        for (int dy=-1;dy<=1;dy++)
        {
            if (dx==0&&dy==0)
            {
                continue;
            }
            if(!valid(col+dx,row+dy))
            {
               continue;
            }
            if(grid[col+dx][row+dy].mine)
            {
               count++; 
            }
        }
    }
    return count;
}

//find out if input or processing happens within limits
bool valid (int col,int row) 
{
    return (col>=0&&col<COLUMNS&&row>=0&&row<ROWS);
}

//flag and unflag cells
void flag(int col,int row) 
{
    grid[col][row].flagged= !grid[col][row].flagged;
}

/*cascade effect using similar logic to near_by mines 
to reveal the offset cells that don'y contain mines*/ 
void cascade(int col,int row)
{
    
    for (int dx=-1;dx<=1;dx++)
    {
        for (int dy=-1;dy<=1;dy++)
        {
            if (dx==0&&dy==0)
            {
                continue;
            }
            if(!valid(col+dx,row+dy))
            {
               continue;
            }
            if (grid[col + dx][row + dy].revealed) continue;

            reveal(col+dx,row+dy);
        }
    }
}

/*reveal the cell if conditions are met
also decide if cascade effect is triggered or 
game win or lose*/
void reveal(int col,int row) 
{
    if (grid[col][row].flagged||grid[col][row].revealed)
    {
        return;
    }
    grid[col][row].revealed=true;
    if(grid[col][row].mine)
    {
        game_state=lose;
    }
    else if (grid[col][row].flagged) return;

    else if(grid[col][row].neighbor_mine==0)
    {
        cascade(col,row);
    }
    revealed_count++;
    if(revealed_count>=(ROWS*COLUMNS-mine_count))
    {
        game_state=win;
    }
}

//reveals the grid entirely. called if game is win or lost
void reveal_all()
{
    for(int i=0;i<COLUMNS;i++)
    {
        for (int j=0;j<ROWS;j++)
        {
            grid[i][j].revealed=true;
        }
    }
}


//what different tiles should look like if revealed
void draw_tiles(sTile tile) 
{
    if(tile.revealed)
    {
    if(tile.mine)
    {
        DrawRectangle(tile.x * tile_width,tile.y * tile_height, tile_width, tile_height, RED);
        Rectangle source={0,0,(float)textures[texture_bomb].width, (float)textures[texture_bomb].height};
        Rectangle dest= {(float)(tile.x*tile_width),(float)(tile.y*tile_height),(float)(tile_width),(float)(tile_height)};
        Vector2 origin ={0,0};
        DrawTexturePro(textures[texture_bomb],source,dest,origin, 0.0f, RED);
    }
    else 
    {
        DrawRectangleLines(tile.x * tile_width,tile.y * tile_height, tile_width, tile_height, WHITE);
        if(tile.neighbor_mine>=0)
        {
            DrawText(TextFormat("%d", tile.neighbor_mine),tile.x*tile_width+14, tile.y*tile_height+4, tile_height, DARKGRAY);
        }
    }
    }
    else if(tile.flagged)
    {
        Rectangle source={0,0,(float)textures[texture_flag].width, (float)textures[texture_flag].height};
        Rectangle dest= {(float)(tile.x*tile_width),(float)(tile.y*tile_height),(float)(tile_width),(float)(tile_height)};
        Vector2 origin ={0,0};
        DrawTexturePro(textures[texture_flag],source,dest,origin, 0.0f, WHITE);
    }
    DrawRectangleLines(tile.x * tile_width,tile.y * tile_height, tile_width, tile_height, WHITE);
    
}

//builds the grid
void render_tiles() 
{
    for (int i=0;i<COLUMNS;i++)
    {
        for (int j=0;j<ROWS;j++)
        {
            draw_tiles(grid[i][j]);
        }
    }
}

//reset the grid and replaces the mines and neighbor_mine if game is restarted
void reset_tiles()
{
    for (int i=0;i<COLUMNS;i++)
    {
        for (int j=0;j<ROWS;j++)
        {
            grid[i][j]=(sTile)
            {
                .x=i,
                .y=j,
                .mine=false,
                .revealed=false,
                .flagged=false,
                .neighbor_mine=-1
            };
        }
    }
    int mines_placed=mine_count;
    while(mines_placed>0)
    {
        int col=GetRandomValue(0,COLUMNS);
        int row=GetRandomValue(0,ROWS);
        if(!grid[col][row].mine)
        {
            grid[col][row].mine=true;
            mines_placed--;
        }
    }
    for(int i=0;i<COLUMNS;i++)
    {
        for(int j=0;j<ROWS;j++)
        {
            if (!grid[i][j].mine)
            {
                grid[i][j].neighbor_mine=nearby_mine(i,j);
            }
        }
    }
}

//loads in textures
void start()
{
    Image image1=LoadImage("flag.png");
    Image image2=LoadImage("bomb.png");
    textures[texture_flag]=LoadTextureFromImage(image1);
    textures[texture_bomb]=LoadTextureFromImage(image2);
    UnloadImage(image1);
    UnloadImage(image2);
    game_state=main_menu;
    
}

//takes input and switches between game states depending on it
void update()
{
     switch(game_state)
    {
        case main_menu:
        if(IsKeyPressed(KEY_N))
        {
            reset();
        }
        break;
        case playing:
         if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        Vector2 mousePos=GetMousePosition();
        int col=(int)(mousePos.x/tile_width);
        int row=(int)(mousePos.y/tile_height);
        if(valid(col,row))
        {
            reveal(col,row);
        }
    }
    else if(IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
    {
        Vector2 mousePos=GetMousePosition();
        int col=(int)(mousePos.x/tile_width);
        int row=(int)(mousePos.y/tile_height);
        if(valid(col,row))
        {
            flag(col,row);
        }
    }
        break;
        case lose:
        if(IsKeyPressed(KEY_ENTER))
        {
            game_state=main_menu;
        }
        break;
        case win:
        if(IsKeyPressed(KEY_ENTER))
        {
            game_state=main_menu;
        }
        break;
        if(IsKeyPressed(KEY_ENTER)){
            game_state= main_menu;
    }
    break;
   
}
}

//which graphics get displayed wrt game states
void render()
{
    switch(game_state)
    {
        case main_menu:
        DrawRectangle(0,0,WIDTH,HEIGHT,DARKBLUE);
        DrawText("MINESWEEPER",20,20,40,BLACK);
        DrawText("[N]ew Game",120,220,20,WHITE);
        DrawText("ESC to QUIT",120,250,20,WHITE);
        DrawText("Group Project by: Afeerah Shafqat CT-153, Farwa Raza CT-156, Khadija Mustafa CT-165",120,500,10,BLACK);
        break;
        case playing:
        render_tiles();
        break;
        case lose:
        render_tiles();
        reveal_all();
        DrawRectangle(0,0,WIDTH,HEIGHT,Fade(WHITE,0.6f));
        DrawText(label_lose,WIDTH/2-MeasureText(label_lose,60)/2,HEIGHT/2-10,60,RED);
        DrawText(label_enter,WIDTH/2-MeasureText(label_enter,34)/2,(int)(HEIGHT*0.75f)-10,34,DARKGREEN);
        break;
        case win:
        render_tiles();
        reveal_all();
        DrawRectangle(0,0,WIDTH,HEIGHT,Fade(WHITE,0.6f));
        DrawText(label_win,WIDTH/2-MeasureText(label_win,60)/2,HEIGHT/2-10,60,DARKGREEN);
        DrawText(label_enter,WIDTH/2-MeasureText(label_enter,34)/2,(int)(HEIGHT*0.75f)-10,34,DARKGREEN);
        break;
    }
}

//reset game state and relevant entities if game restarted
void reset()
{
    reset_tiles();
    game_state=playing;
    revealed_count=0;
}

//unloads textures
void shut_down()
{
    for (int i=0;i<max_textures;i++)
    {
        UnloadTexture(textures[i]);
    }
}


int main() 
{
    InitWindow(WIDTH, HEIGHT, "Minesweeper!");
    SetTargetFPS(60);
    start();
    // Main game loop
    while (!WindowShouldClose()) 
    {
        update();
        BeginDrawing();
        ClearBackground(GRAY);
        render();
        EndDrawing();
        
    }
    shut_down();
    CloseWindow();
   return 0;
  }