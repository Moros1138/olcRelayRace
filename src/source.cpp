// THE AUTHORS
//
// Moros1138
// Alexio 
//
//
//
//
//

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

struct GameObject
{
    olc::vf2d position;
    olc::vf2d velocity;
    olc::vi2d size;    
};

class olc_RelayRace : public olc::PixelGameEngine
{
public:
    olc_RelayRace()
    {
        sAppName = "SuperMarioBros. Reverse";
    }
    GameObject player;
    GameObject ground;
    olc::Sprite* tileSet;   


public:
    bool OnUserCreate() override
    {
        player.position = { 0 , (float)ScreenHeight() / 2 - 16 };
        player.velocity = { 0, 0 };
        player.size = { 16, 16 };

        ground.position = {0, (float)ScreenHeight() / 2};
        ground.velocity = {0, 0};
        ground.size = {16, 16};
        player.position = {0, (float)ScreenHeight() / 2 };
        player.velocity = {0, 0 };
        tileSet = new olc::Sprite("assets/Tileset.png");
        return true;
    }

    bool OnUserUpdate(float fElapsedTime) override
    {
        //player.velocity = { 0, 0 };
        if (GetKey(olc::LEFT).bHeld)
        {
            if (player.velocity.x < -30.0f)
                player.velocity.x -= 0.8f;
            else
                player.velocity.x = -30.0f;
        }
        if (GetKey(olc::RIGHT).bHeld)
        {
            if (player.velocity.x > 30)
                player.velocity.x += 0.8f;
            else
                player.velocity.x = 30.0f;
        }
            //player.velocity.x = 30;

        player.position += player.velocity * fElapsedTime;

        Clear(olc::CYAN);

        DrawPartialSprite(ground.position, tileSet, olc::vi2d(0, 0), ground.size);

        FillRect(player.position, player.size, olc::RED);
        
        return !GetKey(olc::Key::ESCAPE).bPressed;
    }

private:

};

int main()
{
    olc_RelayRace app;
    if(app.Construct(320,180,4, 4, false, false))
    {
        app.Start();
    }
    else
    {
        std::cout << "Failed to start PGE" << std::endl;
    }
    
    return 0;
}