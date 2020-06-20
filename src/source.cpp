// THE AUTHORS
//
// Moros1138
// Alexio 
// AlterEgo
//
//
//
//

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#define MAPWIDTH 212
#define MAPHEIGHT 16

struct GameObject
{
    olc::vf2d position;
    olc::vf2d velocity;
    olc::vi2d size;    
};



class Rect
{
public:
    olc::vd2d position;
    double width, height;
    olc::Pixel colour;

    Rect(olc::vd2d position, double width, double height, olc::Pixel colour) 
        : position{position}, width{width}, height{height}, colour{colour}
    {

    }

    bool Overlaps(Rect rect, olc::vd2d& displacement)
    {
        // Detect overlap
        if(position.x + (width / 2) > rect.position.x - (rect.width / 2) &&
           rect.position.x + (rect.width / 2) > position.x - (width / 2) &&
           position.y + (height / 2) > rect.position.y - (rect.height / 2) &&
           rect.position.y + (rect.height / 2) > position.y - (height / 2))
        {
            double restingDistanceX = (width / 2) + (rect.width / 2);
            double restingDistanceY = (height / 2) + (rect.height / 2);

            double currentDistanceX = position.x - rect.position.x;
            double currentDistanceY = position.y - rect.position.y;

            double overlapX = restingDistanceX - abs(currentDistanceX);
            double overlapY = restingDistanceY - abs(currentDistanceY);

            if(overlapX < overlapY) overlapY = 0;
            else if(overlapY < overlapX) overlapX = 0;

            if(position.x < rect.position.x) overlapX = -overlapX;
            if(position.y < rect.position.y) overlapY = -overlapY;

            displacement = {overlapX, overlapY};

            return true;
        }

        return false;
    }
};

class olc_RelayRace : public olc::PixelGameEngine
{
public:
    olc_RelayRace() : 
        rect1(olc::vd2d(50, 100), 100, 50, olc::WHITE),
        rect2(olc::vd2d(200, 100), 100, 50, olc::WHITE)
    {
        sAppName = "SuperMarioBros. Reverse";
    }
    GameObject player;
    GameObject ground;

    olc::Sprite* tileSet;
    Rect rect1, rect2;

	std::vector<olc::Sprite*> levelTileSprites;

	olc::vi2d camPos = { 0, 0 };

public:
	void loadTiles() {
		tileSet = new olc::Sprite("assets/Tileset.png");
		int tileSetWidth = tileSet->width / 16;
		int tileSetHeight = tileSet->height / 16;

		for (int i = 0; i < (tileSet->width / 16) * (tileSet->height / 16); i++) {
			olc::Sprite* tileSprite = new olc::Sprite(16, 16);
			int x = i % tileSetWidth;
			int y = i / tileSetHeight;
			SetDrawTarget(tileSprite);
			DrawPartialSprite({ x * 16, y * 16 }, tileSet, { 0, 0 }, { 16, 16 });
			levelTileSprites.push_back(tileSprite);
		}
		SetDrawTarget((uint8_t)0);
	}

    bool OnUserCreate() override
    {
        LoadMapFromCSV("assets/map.csv");

        player.position = { 0 , (float)ScreenHeight() / 2 - 16 };
        player.velocity = { 0, 0 };
        player.size = { 16, 16 };

        ground.position = {0, (float)ScreenHeight() / 2};
        ground.velocity = {0, 0};
        ground.size = {16, 16};
        player.position = {0, (float)ScreenHeight() / 2 };
        player.velocity = {0, 0 };
        
		loadTiles();


        return true;
    }

    bool OnUserUpdate(float fElapsedTime) override
    {
		for (int i = 0; i < MAPWIDTH * MAPHEIGHT-1; i++) {
			int camBounds[4];
			int x = i % MAPWIDTH;
			int y = i / MAPHEIGHT;

			

			int sprIndex = vecMap[i];
			
			if (sprIndex >= 0 && sprIndex < levelTileSprites.size()-1)
				DrawSprite(x * 16, y * 16, levelTileSprites[sprIndex]);
		}

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

        //rect1.position.x = GetMouseX();
        //rect1.position.y = GetMouseY();

        if(GetKey(olc::W).bHeld)
        {
            rect1.position.y -= fElapsedTime * 100;
        }

        if(GetKey(olc::S).bHeld)
        {
            rect1.position.y += fElapsedTime * 100;
        }

        if(GetKey(olc::A).bHeld)
        {
            rect1.position.x -= fElapsedTime * 100;
        }

        if(GetKey(olc::D).bHeld)
        {
            rect1.position.x += fElapsedTime * 100;
        }

        olc::vd2d displacement;

        if(rect1.Overlaps(rect2, displacement))
        {
            std::cout << "Rects colliding\n";
            rect1.colour = olc::RED;
            rect2.colour = olc::RED;

            rect1.position += displacement;
        }
        else
        {
            std::cout << "Rects not colliding\n";
            rect1.colour = olc::WHITE;
            rect2.colour = olc::WHITE;
        }


        Clear(olc::CYAN);

        DrawPartialSprite(ground.position, tileSet, olc::vi2d(0, 0), ground.size);

        FillRect(player.position, player.size, olc::RED);


        DrawRect(rect2.position.x, rect2.position.y, rect2.width, rect2.height, rect2.colour);
        DrawRect(rect1.position.x, rect1.position.y, rect1.width, rect1.height, rect1.colour);

        
        return !GetKey(olc::Key::ESCAPE).bPressed;
    }

private:

	std::vector<std::string> split(std::string str, char delimiter) {
		std::string currentPart = "";
		std::vector<std::string> output;
		for (int i = 0; i < str.length(); i++) {
			if (str[i] != delimiter)
				currentPart += std::string(1, str[i]);
			else {
				output.push_back(currentPart);
				currentPart = "";
			}
		}
		output.push_back(currentPart);

		return output;
	}

    void LoadMapFromCSV(std::string file)
    {

        std::ifstream t(file);
        std::string text((std::istreambuf_iterator<char>( t )),
            std::istreambuf_iterator<char>() );

		std::cout << text.length() << std::endl;

        std::string delimiter = ",";

		vecMap.reserve(MAPWIDTH * MAPHEIGHT);

        size_t pos = 0;
        std::string token;
        while ((pos = text.find(delimiter)) != std::string::npos) {
            token = text.substr(0, pos);
            //std::cout << std::stoi(token) << std::endl;
            vecMap.emplace_back(std::stoi(token));

            text.erase(0, pos + delimiter.length());
        }
		/*
		std::vector<std::string> csvSegments = split(text, ',');
		
		std::cout << csvSegments.size() << std::endl;

		for (std::string str : csvSegments) {
			int val = std::stoi(str);
			std::cout << val << std::endl;
			vecMap.push_back(val);
		}*/

		std::cout << "vecmap size " << vecMap.size() << std::endl;
    }

    std::vector<int> vecMap;
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