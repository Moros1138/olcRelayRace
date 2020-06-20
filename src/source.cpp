// THE AUTHORS
//
// Moros1138
// Alexio 
// AlterEgo
// Gecero
// SirFelixDelazar
// Ben (benjaminkyd@gmail.com)
//
//

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#define MAPWIDTH 211
#define MAPHEIGHT 16
#define TILE_SIZE 16


struct Camera
{
    olc::vf2d position;
};

class Rect
{
public:
    olc::vd2d position;
    double width, height;
    olc::Pixel colour;

    Rect( olc::vd2d position, double width, double height, olc::Pixel colour )
        : position{ position }, width{ width }, height{ height }, colour{ colour }
    {

    }

    bool Overlaps( Rect rect, olc::vd2d& displacement )
    {
        // Detect overlap
        if ( position.x + (width / 2) > rect.position.x - (rect.width / 2) &&
            rect.position.x + (rect.width / 2) > position.x - (width / 2) &&
            position.y + (height / 2) > rect.position.y - (rect.height / 2) &&
            rect.position.y + (rect.height / 2) > position.y - (height / 2) )
        {
            double restingDistanceX = (width / 2) + (rect.width / 2);
            double restingDistanceY = (height / 2) + (rect.height / 2);

            double currentDistanceX = position.x - rect.position.x;
            double currentDistanceY = position.y - rect.position.y;

            double overlapX = restingDistanceX - abs( currentDistanceX );
            double overlapY = restingDistanceY - abs( currentDistanceY );

            if ( overlapX < overlapY ) overlapY = 0;
            else if ( overlapY < overlapX ) overlapX = 0;

            if ( position.x < rect.position.x ) overlapX = -overlapX;
            if ( position.y < rect.position.y ) overlapY = -overlapY;

            displacement = { overlapX, overlapY };

            return true;
        }

        return false;
    }
};

class GameObject
{
public:
    GameObject( olc::vd2d position, double width, double height, olc::Pixel colour )
        : collider( position, width, height, colour ),
          position( position )
    {
    }
    Rect collider;
    olc::vf2d position;
    olc::vf2d velocity;
    olc::vi2d size;
};

class olc_RelayRace : public olc::PixelGameEngine
{
public:
    olc_RelayRace() :
        rect1( olc::vd2d( 50, 100 ), 100, 50, olc::WHITE ),
        rect2( olc::vd2d( 200, 100 ), 100, 50, olc::WHITE ),
        player( ) // construct the player here 
        // then make the collider do shit
        // collide it with the world idk
    {
        sAppName = "SuperMarioBros. Reverse";
    }
    GameObject player;
    Camera camera;

    olc::Sprite* tileSet;
    Rect rect1, rect2;

    std::vector<olc::Sprite*> levelTileSprites;

public:

    bool OnUserCreate() override
    {
        LoadMapFromCSV( "assets/map.csv" );

        tileSet = new olc::Sprite( "assets/Tileset.png" );

        camera.position = { 0.0f, -100.0f };

        player.position = { 0 , (float)ScreenHeight() / 2 - 16 };
        player.velocity = { 0, 0 };
        player.size = { 16, 16 };

        ground.position = { 0, (float)ScreenHeight() / 2 };
        ground.velocity = { 0, 0 };
        ground.size = { 16, 16 };

        player.position = { 0, (float)ScreenHeight() / 2 };
        player.velocity = { 0, 0 };

        return true;
    }

    bool OnUserUpdate( float fElapsedTime ) override
    {

        //rect1.position.x = GetMouseX();
        //rect1.position.y = GetMouseY();

        if ( GetKey( olc::W ).bHeld )
        {
            player.position.y -= fElapsedTime * 100;
        }

        if ( GetKey( olc::S ).bHeld )
        {
            player.position.y += fElapsedTime * 100;
        }

        if ( GetKey( olc::A ).bHeld )
        {
            player.position.x -= fElapsedTime * 100;
        }

        if ( GetKey( olc::D ).bHeld )
        {
            player.position.x += fElapsedTime * 100;
        }

        olc::vd2d displacement;

        if ( rect1.Overlaps( rect2, displacement ) )
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


        Clear( olc::CYAN );

        // render loop

        for ( int x = 0; x < MAPWIDTH; x++ )
        for ( int y = 0; y < MAPHEIGHT; y++ )
        {
            // hacky but it works
            if ( (y * MAPWIDTH + x) > 3375 )
                continue;

            int ActiveTile = MapVector[y * MAPWIDTH + x];

            // sky
            if ( ActiveTile == -1 ) continue;

            // find tile in map
            auto DrawFrom = [&] (int tile) {
                // mod w for x int div y - JavidX9
                int w = tile % (tileSet->width / 16);
                int h = tile / (tileSet->width / 16);
                return olc::vi2d( w * 16, h * 16 );
            };

            SetPixelMode( olc::Pixel::ALPHA );
            DrawPartialSprite( { static_cast<int>(camera.position.x + (x * 16)), static_cast<int>(camera.position.y + (y * 16)) }, tileSet, DrawFrom( ActiveTile ), { 16, 16 });
            SetPixelMode( olc::Pixel::NORMAL );

        }

        FillRect( player.position, player.size, olc::RED );


        DrawRect( rect2.position.x, rect2.position.y, rect2.width, rect2.height, rect2.colour );
        DrawRect( rect1.position.x, rect1.position.y, rect1.width, rect1.height, rect1.colour );


        return !GetKey( olc::Key::ESCAPE ).bPressed;
    }

private:

    void LoadMapFromCSV( std::string file )
    {

        std::ifstream t( file );
        std::string text( (std::istreambuf_iterator<char>( t )),
            std::istreambuf_iterator<char>() );

        std::cout << text.length() << std::endl;

        std::string delimiter = ",";

        MapVector.reserve( MAPWIDTH * MAPHEIGHT );

        size_t pos = 0;
        std::string token;
        while ( (pos = text.find( delimiter )) != std::string::npos )
        {
            token = text.substr( 0, pos );
            MapVector.emplace_back( std::stoi( token ) );
            text.erase( 0, pos + delimiter.length() );
        }

        std::cout << "Map Size: " << MapVector.size() << std::endl;
    }

    std::vector<int> MapVector;
};

int main()
{
    olc_RelayRace app;
    if ( app.Construct( 320, 180, 4, 4, false, false ) )
    {
        app.Start();
    }
    else
    {
        std::cout << "Failed to start PGE" << std::endl;
    }

    return 0;
}