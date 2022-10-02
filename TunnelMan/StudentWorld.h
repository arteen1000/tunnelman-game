#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include <string>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class TunnelMan;
class Earth;
class Actor;
class Squirt;

#include "GraphObject.h"
#include <vector>

class StudentWorld : public GameWorld
{
public:
	StudentWorld(std::string assetDir)
		: GameWorld(assetDir)
	{
	}
    
    // can't place another constructor b/c it wouldn't initialize gameworld(assetdir)
    
    virtual ~StudentWorld() {};
    
    Earth* getEarths(int x, int y) const {return earths[x][y];}
    void earthsKiller(int x, int y);
    
    void generateBoulderValues(int& x, int& y);
    void deleteBoulderEarth(int x, int y);
    bool checkBoulderBlocking(GraphObject::Direction d, int x, int y);
    void runawayBoulder(int x, int y);

    bool checkEarthBlocking(GraphObject::Direction d, int x, int y);
    
    bool killerSquirt(int x, int y);
    void squirtGun(GraphObject::Direction d, int x, int y);
    
    double distanceToTunnelMan(int x, int y);   // how far r u for tunnelman? useful for gold and barrels, and protesters, not protesters
    bool closeToProtester(int x, int y);        // allows pickup for gold
    
    void dropGold(int x, int y);    // called by tunnelman-TAB
    void increaseGold(); // tells tunnelman
    
    void increaseSonar();
    void illuminate(int x, int y);
    
    void increaseWater();
    
    bool isClear(int x, int y);      // for adding sonar/water
    
    bool checkFacing(GraphObject::Direction, int, int);      // POSSIBLY THE SMARTEST THING I'VE EVER DONE IN MY LIFE
    void annoyTunnelMan();
    
    int tunnelmanGetX();                       // returns tunnelman's X (for protester)
    int tunnelmanGetY();        // implemented in .cpp because class forwarding
    
            // checks to see if the path is clear to tunnelman in that direction (for vertical/horizontal movement to tunnelman)
    
    virtual int init();
    
    virtual int move();

    virtual void cleanUp();
    
    void decreaseBarrels() {m_barrels--;}   // would it be ok to just put m_barrels in public and do that instead? ask professor.
    void generateBoldValues(int& x, int& y);        // barrels and gold

    
private:
    TunnelMan * tunnelman;
    Earth* earths[64][64];              // no top 4, there for clarity
    std::vector<Actor*> actors;
    int m_barrels;
    
    void updateDisplayText();
    
    void addProtester();        // self explanatory
    
    int G;      // goodie chance, 1 in G
    
    int ticksPassed;    // helper to this
    int T;  // t sicks since last protester added, decrement, restate
    
    int numberOfProtesters; // helper to this
    int P;  // how many protesters any given time
    
    int H;  // probably of hardcore protester
};


#endif // STUDENTWORLD_H_

