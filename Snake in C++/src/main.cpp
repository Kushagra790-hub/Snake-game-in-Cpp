#include <raylib.h>
#include <iostream> 
#include <deque>
#include <raymath.h>
using namespace std;

Color green = {173, 204, 96, 255}; // We made a green color construct by using default byte colors 
Color darkGreen = {43, 51, 24, 255}; // We made a dark green color construct by using default byte colors
Color lightBlue = {59, 85, 162, 255};  // We made a light blue color construct by using default byte colors
Color red = {232, 18, 18, 255};
Color orange = {226, 116, 17, 255};

int cellSize = 30; // To find the size of each cell
int cellCount = 25; // To count each cell in row and column  
int offSet = 75; // To create a frame in the game

double lastUpdatedTime = 0; // The variable is used to keep the track of time at which the last update of the snake occured

bool ElemeniinDeque(Vector2 element, deque <Vector2> deque){ // To check if the food does not spawn on snake's body

for(unsigned int i = 0; i < deque.size(); i++){

    if(Vector2Equals(deque[i], element)){

        return true;
    }
}
return false;
}

bool eventtriggered(double interval){ // To check if the time is passed or not and control the speed of the snake

double currentTime = GetTime();
if(currentTime - lastUpdatedTime >= interval){

    lastUpdatedTime = currentTime;
    return true;
}
return false;
}
class Snake{ // A class is created to draw the snake

    public:
    deque <Vector2> body = {Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9}}; // A deque is created to create the snake positions
    Vector2 direction = {1, 0};
    bool addSegment = false; // To check if the snake body is growing or not 
    Texture2D headTexture;

    Snake() {
        Image headImage = LoadImage("Graphics/snake_head.png");
        headTexture = LoadTextureFromImage(headImage);
        UnloadImage(headImage);
    }

    ~Snake() {
        UnloadTexture(headTexture);
    }

    void Draw(){ // The function is used to draw the snake

        for (unsigned int i = 0; i < body.size(); i++)
        {
            float x = body[i].x;
            float y = body[i].y;
            float drawX = offSet + x * cellSize;
            float drawY = offSet + y * cellSize;

            if (i == 0) { // To move the image as head and rotate it
            
                Vector2 position = { drawX + cellSize / 2.0f, drawY + cellSize / 2.0f }; // center of cell
                float rotation = 0.0f;

                if (direction.x == 1) rotation = 0;
                else if (direction.x == -1) rotation = 180;
                else if (direction.y == 1) rotation = 90;
                else if (direction.y == -1) rotation = -90;

                    
            DrawTexturePro(
                headTexture,
                Rectangle{0, 0, (float)headTexture.width, (float)headTexture.height},
                Rectangle{position.x, position.y, (float)cellSize, (float)cellSize},
                Vector2{cellSize / 2.0f, cellSize / 2.0f},
                rotation,
                WHITE);

            } 
            
            else if (i == body.size() - 1) {
                Vector2 tail = body[i];
                Vector2 beforeTail = body[i - 1];
            
                Vector2 center = {
                    offSet + tail.x * cellSize + cellSize / 2.0f,
                    offSet + tail.y * cellSize + cellSize / 2.0f
                };
            
                Vector2 dir = Vector2Normalize(Vector2Subtract(beforeTail, tail)); // tail -> beforeTail
                float tailLength = cellSize * 0.5f;
            
                Vector2 point1 = Vector2Add(center, Vector2Scale(dir, tailLength));
            
                Vector2 rotatedLeft = Vector2Rotate(dir, DEG2RAD * 120);
                Vector2 point2 = Vector2Add(center, Vector2Scale(rotatedLeft, tailLength * 0.5f));
            
                Vector2 rotatedRight = Vector2Rotate(dir, DEG2RAD * -120);
                Vector2 point3 = Vector2Add(center, Vector2Scale(rotatedRight, tailLength * 0.5f));
            
                DrawTriangle(point1, point2, point3, darkGreen);
            }
            
            
            
            else {
                Vector2 prev = { offSet + body[i - 1].x * cellSize + cellSize / 2.0f, offSet + body[i - 1].y * cellSize + cellSize / 2.0f };
                Vector2 curr = { drawX + cellSize / 2.0f, drawY + cellSize / 2.0f };
                DrawLineEx(prev, curr, cellSize - 10, green);
            }
            
        }
        
    }

    void Update(){ // This function is used to move the snake

        body.push_front(Vector2Add(body[0], direction)); // To add the front cell of the snake in different direction
        
        if(addSegment == true){

            addSegment = false;
        }

        else{

            body.pop_back(); // To remove the last block/ cell
        }
    }

    void Reset(){

        body = {Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9}};
        direction = {1,0};
    }
};


class Food{ // A class is created to draw the food
public:
Vector2 position; //To find the x, y co-ordinates in the grid
Texture2D texture; // To load the texture from the image

Food(deque <Vector2> snakebody){ // A constructor is used to load the image, change it into the texture and get randomize loacation 

    Image image = LoadImage("Graphics/food.png");
    texture = LoadTextureFromImage(image);
    UnloadImage(image);
    position = GetRandomPos(snakebody);
}

~Food(){ // A destructor is used to unload the texture when the object is destroyed 

    UnloadTexture(texture);
}

void Draw(){ // The draw function is used to draw the food
    DrawTexture(texture, offSet + position.x * cellSize, offSet + position.y * cellSize, WHITE);
}

Vector2 GenerateRandomCell(){

    float x = GetRandomValue(0, cellCount - 1); // To get the randomize x c- ordinate
    float y = GetRandomValue(0, cellCount - 1); // To get the randomize x c- ordinate
    return Vector2{x, y};
}

Vector2 GetRandomPos(deque <Vector2> snakeBody){ // To generate the randomize location of the food

    Vector2 position = GenerateRandomCell();

    while (ElemeniinDeque(position, snakeBody))
    {
        position = GenerateRandomCell();
    }
        return position;
}
};

class Game{

    public:
    Snake snake = Snake(); // To create a snake class object
    Food food = Food(snake.body); // To create a food class object 
    bool running = true; // To stop the game if it is over
    int score = 0; // TO calculate the score of the snake
    Sound eatSound; // To make a object for swallowing food
    Sound gameOverSound; // To make a object for game over
    Music music; // To make a object for background music

    Game(){

        InitAudioDevice();
        eatSound = LoadSound("Sounds/Swallow.mp3"); // To load the swallowing sound
        gameOverSound = LoadSound("Sounds/game over.mp3"); // To load the game over sound
        music = LoadMusicStream("Sounds/music.mp3"); // To load the background music
    }

    ~Game(){ // To destroy the sound if the game window is closed

        UnloadSound(eatSound); 
        UnloadSound(gameOverSound);
        UnloadMusicStream(music);
    }

    void Draw(){

        
    food.Draw(); // To draw the food
    snake.Draw(); // To draw the snake
    }

    void Update(){
        
        if(running == true){
        
        snake.Update(); // To update the snake movement
        CheckCollisionWithFood();
        CheckCollisionWithEdges(); // To check if the snake has collided with edges of the wall
        CheckCollisionWithTail(); // To check if snake's head is collided with its own tail or not
    }
    }

    void CheckCollisionWithFood(){ // To check if snake is eating food or not 

        if(Vector2Equals(snake.body[0], food.position)){

            food.position = food.GetRandomPos(snake.body); // TO get new random position of the food
            snake.addSegment = true; // To notify that the snake has to grow
            score ++; // To increase the score if the snake has eaten the food 
            PlaySound(eatSound);
        }
    }

    void CheckCollisionWithEdges(){

        if(snake.body[0].x == cellCount or snake.body[0].x == -1){

            GameOver();
        }

        if(snake.body[0].y == cellCount or snake.body[0].y == -1){

            GameOver();
        }
    }

    void GameOver(){
        snake.Reset(); // To reset the snake if it collided in the walls
        food.position = food.GetRandomPos(snake.body); // To rest the position of food if game is over
        running = false;
        score = 0;
        PlaySound(gameOverSound);
        
    }

    void CheckCollisionWithTail(){

        deque <Vector2> headlessBody = snake.body;
        headlessBody.pop_front();

        if(ElemeniinDeque(snake.body[0], headlessBody)){

            GameOver();
        }
    }
};

int main() 
{
float barY = offSet + cellSize * cellCount + 10; // To create a rectangle

cout << "Starting the Game......" << endl;

InitWindow(2 * offSet + cellSize * cellCount, 2 * offSet + cellSize * cellCount, "Retro Snake"); // It initialises the window, in it contains height , width and the title of the window
SetTargetFPS(60); // To lock the speed of game in every device 
 
Game game = Game();
PlayMusicStream(game.music);

while(WindowShouldClose() == false){ // To check if the "Esc" key or close button is pressed to close the window  
    UpdateMusicStream(game.music);
    BeginDrawing(); // To begin the drawing in the canvas
    
    if(eventtriggered(0.2)){
    
        game.Update();
    }

    if((IsKeyPressed(KEY_UP) or IsKeyPressed(KEY_W)) && game.snake.direction.y != 1){ // If the user wants to move the snake in up direction

        game.snake.direction = {0, -1};
        game.running = true;
        PlayMusicStream(game.music);
    }

    if((IsKeyPressed(KEY_DOWN) or IsKeyPressed(KEY_S)) && game.snake.direction.y != -1){ // If the user wants to move the snake in down direction

        game.snake.direction = {0, 1};
        game.running = true;
        PlayMusicStream(game.music);
    }

    if((IsKeyPressed(KEY_LEFT) or IsKeyPressed(KEY_A)) && game.snake.direction.x != 1){ // If the user wants to move the snake in left direction

        game.snake.direction = {-1, 0};
        game.running = true;
        PlayMusicStream(game.music);
    }

    if((IsKeyPressed(KEY_RIGHT) or IsKeyPressed(KEY_D)) && game.snake.direction.x != -1){ // If the user wants to move the snake in right direction
       
        game.snake.direction = {1, 0};
        game.running = true;
        PlayMusicStream(game.music);
    }

    if(IsKeyPressed(KEY_R)){

        game.running = true;
        PlayMusicStream(game.music);
    }

    ClearBackground(lightBlue); // To clear the background of the canvas with green color 
    
    DrawRectangleLinesEx(Rectangle{(float) offSet - 5, (float) offSet - 5, (float) cellSize * cellCount, (float) cellSize * cellCount}, 5, darkGreen); // To draw the rectangle frame
    
    // To create a title and its box
    DrawRectangleRec(Rectangle{0, 0, (float)GetScreenWidth(), 70}, Color{255, 248, 220, 255});
    DrawText("Retro Snake", offSet - 5, 20, 40, red);
    
    // To create score and its box
    DrawRectangleRec(Rectangle{0, barY - 10, (float)GetScreenWidth(), 70}, Color{255, 248, 220, 255});
    DrawText(TextFormat("Score: %i", game.score), offSet, barY + 10, 30, orange);
    DrawText("Press R to restart", GetScreenWidth() - offSet - MeasureText("Press R to restart", 20), barY + 15, 20, darkGreen);
    
    game.Draw();
    if(!game.running){ // To add a game over text if the game has ended

        DrawText("Game Over", (offSet - 5) + 540, 20, 40, red);
        StopMusicStream(game.music);
    }
    EndDrawing(); // To end the drawing in the canvas
}

CloseWindow(); // To close the window
return 0;
}