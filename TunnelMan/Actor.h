#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include "StudentWorld.h"   // necessary for getKey()

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp
    // Will do.
#include <stack>

enum State {falling, waiting, stable, alive, dead, pickupableP, pickupableT, stunned};      // stunned is specifically so can't hit protesters who are stunned

// maybe stunned? when hit by squirt
// pickupableP is a temporary state
// gold starts in tunnelman state (permanent state start out)
// invisible barrels of oil and gold - sonar or nearby tunnelman

struct Point
{
public:
    Point(int x, int y) : m_x(x), m_y(y) {}
    Point(){}
    
    int getX() const {return m_x;}
    int getY() const {return m_y;}
    
    void setX(int x) { m_x = x;}
    void setY(int y) { m_y = y;}
private:
    int m_x;
    int m_y;
};                  // for shortest path, like in lecture #5


class Actor : public GraphObject
{
    
public:
    // constructor
    Actor(int imageID, int startX, int startY, Direction startDirection, double size, unsigned int depth)
    : GraphObject(imageID, startX, startY, startDirection, size, depth) { setVisible(true); } // for Earth only
    
    Actor(StudentWorld* world, int imageID, int startX, int startY, Direction startDirection, double size, unsigned int depth)
        : GraphObject(imageID, startX, startY, startDirection, size, depth)
    , m_world(world) {setVisible(true);}
    
    virtual ~Actor(){}
    
    virtual bool blocksOthers() {return false;} // true for boulder
    
    
    virtual void decreaseHealth(int howMuch) {} // upcast
    
    virtual bool annoyable(){return false;}
    virtual void pickedUpGold() {}          // upcast for protesters, only accessable if annoyable
    
    virtual void doSomething() = 0;
    
    void move(Direction);       // retarded in hindsight but saves me some typing
    void moveOne(int x, int y);      // specifically for protesters, cleans stuff up, takes care of direction setting, moves them toward their proper path
            // originally named moveTo until I became aware of my retardation
    
    bool checkAnyBlock(Direction, int, int);        // given any direction and a coordinate
    
    void beginCountdown(int c) { count = c; }
    void decrementCount() {count--;}
    bool itsTime() {if (count <= 0) return true; else return false;}
    
    State getState() const {return s;}
    void setState(State input) {s = input;}
    
    StudentWorld* getWorld() { return m_world; }
                                    
private:
    StudentWorld* m_world;
    State s;
    int count;
};


class Earth : public Actor
{
public:
    Earth(int x, int y) : Actor(TID_EARTH, x, y, right, 0.25, 3) {}
    virtual ~Earth(){}
    
    virtual void doSomething() override {}
};

class Boulder : public Actor
{
public:
    Boulder (StudentWorld *world, int x, int y) : Actor(world, TID_BOULDER, x, y, down, 1.0, 1) {setState(stable);}
    
    virtual ~Boulder() {}
    
    virtual void doSomething() override;
    
    virtual bool blocksOthers() override {return true;}
    
    bool mustStop();
};

class Squirt : public Actor
{
public:
    Squirt(Direction d, int x, int y, StudentWorld *world) : Actor(world, TID_WATER_SPURT, x, y, d, 1.0, 1)
    {setState(falling); beginCountdown(4);}            // instantly falling, use countDown method
    
    virtual ~Squirt(){}
    
    virtual void doSomething() override;
    
};

class Barrel : public Actor
{
public:
    Barrel(StudentWorld* world, int x, int y) : Actor(world, TID_BARREL, x , y , right, 1.0, 2)
        {setVisible(false); setState(pickupableT);}
    virtual ~Barrel(){}
    
    virtual void doSomething() override;
    
    
};


class Gold : public Actor

{
public:
    
    Gold(State s, StudentWorld* world, int x, int y) : Actor(world, TID_GOLD, x, y, right, 1.0, 2)
    { setState(s);
        if (s == pickupableT) setVisible(false);
        else
            beginCountdown(100);            // lasts 100 ticks once TAB pressed
    }             // either starts out pickupableT or pickupableP and that determines visibility, so we could pass its state to it // becomes visible and pickupableP when dropped by tunnelman
    ~Gold(){}
    
    virtual void doSomething() override;
};


class Sonar : public Actor          // i see lots of similarites

{
public:
    Sonar(StudentWorld* world, int x, int y);
    virtual ~Sonar(){}
    
    virtual void doSomething() override;        // I see ways to combine ... could be a pain in the ass
    
};

class Water : public Actor
{
public:
    Water(StudentWorld* world, int x, int y);
    virtual ~Water(){}
    
    virtual void doSomething() override;
};


class Human : public Actor
{
public:
    Human(int health, StudentWorld* world, int imageID, int startX, int startY, Direction startDirection, double size, unsigned int depth)
    : Actor(world, imageID, startX, startY, startDirection, size, depth) , m_health(health)
    {setState(alive);}
    
    virtual ~Human(){}
    
    
    virtual bool annoyable() override {return true;}
    virtual void decreaseHealth(int howMuch) override { m_health -= howMuch;}
    
    int getHealth() const {return m_health;}    // for tunnelman to declare deadness
    
private:
    int m_health;
};

class Protester : public Human          // a base class
{
public:
    Protester(int health, StudentWorld* world, int imageID);    // moved to cpp for cleanliness
    virtual ~Protester(){}  // no dynamic allocation, only objects of the type and then pointers to those objects that are objects of Protester
    
    bool staticBehavior();      // same for both hardcore and regular protester - resting, falling, dead, or shouting covered in this, half of doSomething, if this happens, then none of the other behaviors happen, hence true/false
    void movementBehavior();
    
        // ABC because no doSomething()
    
    bool checkStraightPath(int x, int y, GraphObject::Direction d); // could I move the whole way in the direction given to me and get to where tunnelman is without any sort of blockage?
    
    void shout();   // force interaction with tunnelman
    
    bool findShortestRoute(int endx, int endy, std::stack<Point*> & S);   // uses BFS to get to x, y (60 for the exit route, may be different when involves tunnelman - > variable), true if solvable, false if not
    
    virtual void decreaseHealth(int howMuch) override;      // for annoyance
    
    virtual void pickedUpGold() override {}     // make sure u do this
    
protected:
    int restingTicks;       // for adding gold
    int ticksSinceLastShout;    // for hardcore to increment
private:
    int numSquaresToMoveInCurrentDirection;
    int ticksToWaitBetweenMoves;
    std::stack<Point*> S;       // holds points for exit route
    bool visited[64][64];    // u been here before?
    Point points[64][64];       // can only go from 0 - 60, the rest are for Earth, checkAnyBlock takes care of that, could always make it 64 64
    Point* previous[64][64];        // not
};

class RegularP : public Protester
{
public:
    RegularP(StudentWorld* world) : Protester(5, world, TID_PROTESTER) {}
    virtual ~RegularP(){}
    
    virtual void doSomething() override;
    
    virtual void pickedUpGold() override;
    
    virtual void decreaseHealth(int howMuch) override;

};
// move 8 - 60 inclusive before switching direction, kinda retarded
// int ticksToWaitBetweenMoves = max(0, 3 – current_level_number/4)

class HardcoreP : public Protester
{
public:
    HardcoreP(StudentWorld* world) : Protester(20, world, TID_HARD_CORE_PROTESTER) { M = 16 + getWorld()->getLevel() * 2;}
    virtual ~HardcoreP(){}
    
    virtual void doSomething() override;        // used static and movement behavior and added the one extra in between #5 in spec
    
    virtual void pickedUpGold() override;
    
    virtual void decreaseHealth(int howMuch) override;
    
private:
    std::stack<Point*> T;   // for tunnelman
    double M;           // double allows comparison to distanceToTunnelMan
};
class TunnelMan : public Human
{
public:
    TunnelMan(StudentWorld *world) : Human (10, world, TID_PLAYER, 30, 60, right, 1.0, 0) {m_water = 5; m_sonar = 1; m_gold = 0; beginCountdown(4);}
    virtual ~TunnelMan(){}
                            
    virtual void doSomething() override;
    
    void dig();         
    
    void increaseGold() {m_gold++;}
    void increaseSonar() {m_sonar++;}
    void increaseWater() {m_water++;}
    
    int getGold() const {return m_gold;}
    int getWater() const {return m_water;}
    int getSonar() const {return m_sonar;}
    
private:
    int m_water;
    int m_sonar;
    int m_gold;
    
};

#endif // ACTOR_H_

