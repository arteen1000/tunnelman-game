
#include "StudentWorld.h"
#include <string>

#include "Actor.h"
#include "GraphObject.h"

#include <sstream>
#include <iomanip>
#include <algorithm>
#include <vector>
#include <cmath>

using namespace std;

GameWorld* createStudentWorld(string assetDir)
{
    return new StudentWorld(assetDir);
}

// Students:  Add code to this file (if you wish), StudentWorld.h, Actor.h and Actor.cpp
    // if i wish?

void StudentWorld::updateDisplayText()
{
    
    int level = getLevel();
    int lives = getLives();
    int health = tunnelman->getHealth();
    int squirts = tunnelman->getWater();
    int gold = tunnelman->getGold();
    int sonar = tunnelman->getSonar();
    int score = getScore();
    int barrels = m_barrels;
    
    ostringstream oss;
    
    
    oss << "Lvl: " << level << setw(2) << " Lives: " << setw(1) << lives
    << " Hlth: " << setw(3) << health * 10 << "% Wtr: "
    << squirts << " Gld: " << gold << " Oil left: "
    << barrels << " Sonar: " << sonar <<                // i didnt do setw(2) here because it looks better and i even ran it by the professor
    " Scr: ";
    oss.fill('0');
    oss << setw(6) << score << endl;
    string s = oss.str();
    setGameStatText(s);
}

void StudentWorld::earthsKiller(int x, int y)
{
    if (tunnelman->itsTime())
    {
        playSound(SOUND_DIG);
        tunnelman->beginCountdown(4);
    }
    if (earths[x][y]->isVisible())
    {
        earths[x][y]->setVisible(false);
        
        tunnelman->decrementCount();
//        if (rand()%4)
//            playSound(SOUND_DIG);
    }
        
}

int StudentWorld::init()
{
    
    G = getLevel() * 25 + 300;
    T = fmax(25, 200 - getLevel());
    P = fmin(15, 2 + getLevel() * 1.5);
    H = fmin(90, getLevel() * 10 + 30);      // seems to be / 100
    
    
    // construction (instead 0f actual constructor)
    
    tunnelman = new TunnelMan(this);
    
    for (int i = 0; i < 64 ; i++)   // every column, up to 63
        for (int j = 0 ; j < 64 ; j ++) // all the rows up to 59        // all the rows now cuz of visibility
            earths[i][j] = new Earth(i, j);
    
    for (int i = 0; i < 64; i ++)   // cols
        for (int j = 60 ; j < 64 ; j++) // rows
            earths[i][j]->setVisible(false);         // top 4 spaces
    
    for (int i = 30; i < 34 ; i++)  // cols 30 - 33
        for (int j = 4 ; j < 60 ; j ++ )    // from row 4 - 59
        {

            earths[i][j]->setVisible(false);
            
        }

    int B = fmin(getLevel()/2 + 2, 9);
    
    while (B > 0)
    {
        int x; int y;
        generateBoulderValues(x, y);
        
//        deleteBoulderEarth(20, 20);
//        Boulder *b = new Boulder(this, 20, 20);
//        actors.push_back(b);
//        B--;
//        deleteBoulderEarth(20, 56);
//        Boulder *c = new Boulder(this, 20, 56);
//        actors.push_back(c);
//        B--;
            vector<Actor*>::iterator it = actors.begin();

            for ( ; it != actors.end(); it++)
            {
                // calculations, x and GetX() (omitting y for simplicity) represent the lower lefts, a radius (or distance) would come out the same whether centers or lower lefts are subtracted from one another, therefore, it makes no difference, and the following calculations are as if from center, or any arbitrary point on the boulder and the object it is compared to (a boulder in this case)

               if (sqrt(pow(((*it)->getX() - x), 2) + pow(((*it)->getY() - y), 2)) <= 6.0)
                   break;
            }
            if (it == actors.end())         // got to the end (all are good size)]
            {
                deleteBoulderEarth(x, y);
                Boulder *b = new Boulder(this, x, y);
                actors.push_back(b);
                B--;
            }

    }
    
    
    int G = fmax(5 - getLevel() / 2, 2);
    
    while (G > 0)
    {
        int x; int y;
        generateBoldValues(x, y);
        
        vector<Actor*>::iterator it = actors.begin();

        for ( ; it != actors.end(); it++)
        {

           if (sqrt(pow(((*it)->getX() - x), 2) + pow(((*it)->getY() - y), 2)) <= 6.0)
               break;
        }
        if (it == actors.end())         // got to the end (all are good size)]
        {
            Gold* g = new Gold(pickupableT, this, x, y);
            actors.push_back(g);
            G--;
        }
    }
    
    
    int L = fmin(2 + getLevel(), 21);
    m_barrels = L;
    while (L > 0)
    {
        int x; int y;
        generateBoldValues(x, y);
        
        vector<Actor*>::iterator it = actors.begin();

        for ( ; it != actors.end(); it++)
        {

           if (sqrt(pow(((*it)->getX() - x), 2) + pow(((*it)->getY() - y), 2)) <= 6.0)
               break;
        }
        if (it == actors.end())         // got to the end (all are good size)]
        {
            Barrel *b = new Barrel(this, x, y);
            actors.push_back(b);
            L--;
        }
    }
    
    
    
//    cerr << "Size of actors array: " << actors.size() << endl;
    ticksPassed = 0;
    numberOfProtesters = 0; // add protester responsible for dealing with these
    addProtester();
    return GWSTATUS_CONTINUE_GAME;
}


int StudentWorld::move()
{
    updateDisplayText();

    
    if (tunnelman->getState() == alive)
    {
        
        tunnelman->doSomething();
        
        if (m_barrels == 0) // decreased with decreaseBarrels()
        {
            playSound(SOUND_FINISHED_LEVEL);
            return GWSTATUS_FINISHED_LEVEL;
        }
        
        vector<Actor*>::iterator it = actors.begin();
        
        for ( ; it != actors.end(); it++)
        {
            (*it)->doSomething();

            
            if (tunnelman->getState() == dead)
            {
                playSound(SOUND_PLAYER_GIVE_UP);
                decLives();
                return GWSTATUS_PLAYER_DIED;
            }

        }
        
        
        it = actors.begin();
        
        for ( ; it != actors.end() ; it ++)
        {
            if ( (*it)->getState() == dead)
            {
                delete *it;
                it = actors.erase(it);  // one after, but it just reshuffled my shit
                it--;
            }
        }
        
        // P is the target, T is the time since last, so may be updated immediately or if killed off too quick, may not
        // add first protester at end of init
        
        if (ticksPassed >= T && numberOfProtesters < P)
        {
            addProtester();
        }                      
     
        
        if (rand()%G == 0)  // whatever G is, it goes between 0- (G- 1) so that means that it's a 1 in G chance
        {
            if (rand()%5 == 0)      // 1 in 5 chance
            {
                Sonar* s = new Sonar(this, 0, 60);
                actors.push_back(s);
            }
            else
            {
                int x = rand()%61; int y = rand()%61;       // ANYWHERE EARTHLESS
                
                while (!isClear(x, y))
                {
                    x = rand()%61; y = rand()%61;
                }
                Water* w = new Water(this, x, y);
                actors.push_back(w);
                
            }
        }
        
    }
    else if (tunnelman->getState() == dead)
    {
        playSound(SOUND_PLAYER_GIVE_UP);
        decLives();
        return GWSTATUS_PLAYER_DIED;
    }
    
    ticksPassed++;
    return GWSTATUS_CONTINUE_GAME;
    
}

void StudentWorld::addProtester()
{
    ticksPassed = 0;
    numberOfProtesters++;
                                // H being 100 means 100%
    // create integer with a H percent chance of being <= H
    int probability = rand()%100 + 1;   // generates values between 1 to 100 with equal probability, probability of it being under H is H
    
    if (probability <= H)   // probability of being under H is probability of H, like example: if H was 5 and it generates 1 to 10, 1 2 3 4 5, 6 7 8 9 10, would be half probability
    {
        HardcoreP *h = new HardcoreP(this); // this is his world now
        actors.push_back(h);
    }
    else
    {
        RegularP *r = new RegularP(this);
        actors.push_back(r);                // and we're done, hallelujah. fml, probably 50 - 60 hrs on this
    }
    
}

void StudentWorld::generateBoldValues(int &x, int &y)
{
    x = rand()%61;
    y = rand()%57;
    
    while (! (y > 0))
        y = rand()%57;
    
    while (x>= 27 && x <= 33)
        x=rand()%61;
}

void StudentWorld::deleteBoulderEarth(int x, int y)
{
    for (int i = 0 ; i < 4 ; i ++)
    {
        for (int j = 0 ; j < 4 ; j ++)
        {
            if (earths[x + i][y + j]->isVisible())
            {
                earths[x+i][y+j]->setVisible(false);
            }
        }
    }
}

void StudentWorld::generateBoulderValues(int &x, int &y)
{
    x = rand()%59 + 1;
    y = rand()%36 + 20;
    
    while (! (y > 0))
        y = rand()%36 + 20;
    
    while (x >= 26 && x <= 34)              // 30 to 33 want to AVOID, so, 27 would cause bad value     // don't want it to be right next to tunnel
        x = rand()%59 + 1;
}

bool StudentWorld::checkFacing(GraphObject::Direction d, int x, int y)      // for protester
{
    if (sqrt(pow((tunnelman->getX() - x), 2) + pow((tunnelman->getY() - y), 2) ) <= 4.0)
        /*  ||
         
         (d == GraphObject::up && sqrt(pow((tunnelman->getX() + 1.5) - x, 2) + pow(tunnelman->getY() - (y + 3), 2)) <= 2.0)

         ||

         (d == GraphObject::up && sqrt(pow((tunnelman->getX() + 1.5) - (3 + x), 2) + pow(tunnelman->getY() - (y + 3), 2)) <= 2.0)

         ||

         (d == GraphObject::down && sqrt(pow((tunnelman->getX() + 1.5) - x, 2) + pow((tunnelman->getY() + 3) - y, 2)) <= 2.0)

         ||

         (d == GraphObject::down && sqrt(pow((tunnelman->getX() + 1.5) - (3 + x), 2) + pow((tunnelman->getY() + 3) - y, 2)) <= 2.0)

         ||

         (d == GraphObject::left && sqrt(pow((tunnelman->getX() + 3) - x, 2) + pow((tunnelman->getY() + 1.5) - y, 2)) <= 2.0 )


         ||

         (d == GraphObject::left && sqrt(pow((tunnelman->getX() + 3) - x, 2) + pow((tunnelman->getY() + 1.5) - (y + 3), 2)) <= 2.0 )

         ||

         ( d == GraphObject::right && sqrt(pow((tunnelman->getX()) - (x + 3), 2) + pow((tunnelman->getY() + 1.5) - y, 2)) <= 2.0)

         ||

         ( d == GraphObject::right && sqrt(pow((tunnelman->getX()) - (x + 3), 2) + pow((tunnelman->getY() + 1.5) - (y + 3), 2)) <= 2.0)*/
    {
        switch (d)
        {
            case GraphObject::up:
                if (tunnelman->getY() > y)
                    return true;
                break;
            case GraphObject::down:
                if (tunnelman->getY() < y)
                    return true;
                break;
            case GraphObject::left:
                if (tunnelman->getX() < x)
                    return true;
                break;
            case GraphObject::right:
                if ( tunnelman->getX() > x)
                    return true;
                break;
            case GraphObject::none:
            default:
                return false;
    }
    }
        
        return false;
        
        
}
bool StudentWorld::checkBoulderBlocking(GraphObject::Direction d, int x, int y)
// given a direction, and a location, tell me if I've got a blockage by a boulder

{
    vector<Actor*>::iterator p = actors.begin();
    for ( ; p != actors.end() ; p ++)
    {
        if ( (*p)->blocksOthers() )         // boulder, must check if same boulder
        {
            if ( (*p)->getX() == x && (*p)->getY() == y)    // same boulder so fuck it (also this messes shit up)
                continue;                   // skips this iteration of for loop
            
            if (sqrt(pow(((*p)->getX() - x), 2) + pow(((*p)->getY() - y), 2) ) <= 4.0 ||
                
                (d == GraphObject::up && sqrt(pow(((*p)->getX() + 1.5) - x, 2) + pow((*p)->getY() - (y + 3), 2)) <= 2.0)

                ||

                (d == GraphObject::up && sqrt(pow(((*p)->getX() + 1.5) - (3 + x), 2) + pow((*p)->getY() - (y + 3), 2)) <= 2.0)

                ||

                (d == GraphObject::down && sqrt(pow(((*p)->getX() + 1.5) - x, 2) + pow(((*p)->getY() + 3) - y, 2)) <= 2.0)

                ||

                (d == GraphObject::down && sqrt(pow(((*p)->getX() + 1.5) - (3 + x), 2) + pow(((*p)->getY() + 3) - y, 2)) <= 2.0)

                ||

                (d == GraphObject::left && sqrt(pow(((*p)->getX() + 3) - x, 2) + pow(((*p)->getY() + 1.5) - y, 2)) <= 2.0 )


                ||

                (d == GraphObject::left && sqrt(pow(((*p)->getX() + 3) - x, 2) + pow(((*p)->getY() + 1.5) - (y + 3), 2)) <= 2.0 )

                ||

                ( d == GraphObject::right && sqrt(pow(((*p)->getX()) - (x + 3), 2) + pow(((*p)->getY() + 1.5) - y, 2)) <= 2.0)

                ||

                ( d == GraphObject::right && sqrt(pow(((*p)->getX()) - (x + 3), 2) + pow(((*p)->getY() + 1.5) - (y + 3), 2)) <= 2.0))       // and that's how it's done (time spent thinking: roughly 3 hrs time spent typing: roughly 10 minutes)
            {
                switch (d)
                {
                    case GraphObject::up:
                        if ((*p)->getY() > y)
                            return true;
                        break;
                    case GraphObject::down:
                        if ((*p)->getY() < y)
                            return true;
                        break;
                    case GraphObject::left:
                        if ((*p)->getX() < x)
                            return true;
                        break;
                    case GraphObject::right:
                        if ( (*p)->getX() > x)
                            return true;
                        break;
                    case GraphObject::none:
                    default:
                        return false;
                }
            }
            
        }
    }
    
    return false;
}

bool StudentWorld::checkEarthBlocking(GraphObject::Direction d, int x, int y)
{
    switch (d)
    {
        case GraphObject::up:
            for (int i = 0 ; i < 4 ; i ++)
            {
                if (y == 63)
                    return false;            // no bad access allowed! serves as boundary check !! NOT THE CASE THAT EARTH IS LOCKING IN THAT DIRECTION WITH THAT VALUE!
                if (earths[x + i][y + 4]->isVisible())
                    return true;
            }

                // for testing
            break;
        case GraphObject::down:
            for (int i = 0 ; i < 4 ; i ++)
            {
                if (y == 0)
                    return false;
                if (earths[x + i][y - 1]->isVisible())
                    return true;
            }
 
            break;
        case GraphObject::left:
            if (x == 0)
                return false;
            for (int i = 0 ; i < 4 ; i ++)
            {
                if (earths[x - 1][y + i]->isVisible())
                    return true;
            }
           
            break;
        case GraphObject::right:
            for (int i = 0 ; i < 4 ; i ++)
            {
                if (x == 63)
                    return false;
                if (earths[x + 4][y + i]->isVisible())
                    return true;
            }

            break;
        case GraphObject::none:
        default:
            return false;
    }
    
    return false;
}


void StudentWorld::runawayBoulder(int x, int y)
{
    
    if (sqrt(pow((tunnelman->getX() - x), 2) + pow((tunnelman->getY() - y), 2)) <= 3.0)
    {
        tunnelman->decreaseHealth(100);
//        tunnelman->setState(dead);
    }
    vector<Actor*>::iterator p = actors.begin();                // for protesters when they occur
    for ( ; p != actors.end() ; p ++)
    {
        if ((*p)->annoyable() == true && (*p)->getState() != falling)           // human and not falling boulder
        {
            if (sqrt(pow(((*p)->getX() - x), 2) + pow(((*p)->getY() - y), 2)) <= 3.0)
            {
                (*p)->decreaseHealth(100);  // honestly what's the point?
//                (*p)->setState(dead);
            }
        }
    }
}

bool StudentWorld::killerSquirt(int x, int y)
{
    vector<Actor*>::iterator p = actors.begin();                // for protesters when they occur
    for ( ; p != actors.end() ; p ++)
    {
        if ((*p)->annoyable() == true && (*p)->getState() != falling)   // not the squirt
        {
            if (sqrt(pow(((*p)->getX() - x), 2) + pow(((*p)->getY() - y), 2)) <= 3.0)
            {
                (*p)->decreaseHealth(2);
                return true;
            }
        }
    }
    
    return false;
}

void StudentWorld::squirtGun(GraphObject::Direction d, int x, int y)
    // responsible for squirt creation and vector addition
{
    if (x < 0 || x > 63 || y < 0 || y > 63)
        return;
    else if (checkEarthBlocking(d, x, y) || checkBoulderBlocking(d, x, y))
        return;
        
    Squirt* s = new Squirt(d, x, y, this);
    actors.push_back(s);
}

double StudentWorld::distanceToTunnelMan(int x, int y)
{
    return (sqrt (pow((tunnelman->getX()) - x, 2) + pow((tunnelman->getY()) - y, 2)));
}

//
//int StudentWorld::move()
//{
//    updateDisplayText();
//
//
//    if (tunnelman->getState() == alive)
//    {
//
//        tunnelman->doSomething();
//
//        if (m_barrels == 0) // decreased with decreaseBarrels()
//        {
//            playSound(SOUND_FINISHED_LEVEL);
//            return GWSTATUS_FINISHED_LEVEL;
//        }
//
//        vector<Actor*>::iterator it = actors.begin();
//
//        for ( ; it != actors.end(); it++)
//        {
//            (*it)->doSomething();
//
//
//            if (tunnelman->getState() == dead)
//            {
//                playSound(SOUND_PLAYER_GIVE_UP);
//                decLives();
//                return GWSTATUS_PLAYER_DIED;
//            }
//
//        }
//
//
//        it = actors.begin();
//
//        for ( ; it != actors.end() ; it ++)
//        {
//            if ( (*it)->getState() == dead)
//            {
//                delete *it;
//                it = actors.erase(it);  // one after, but it just reshuffled my shit
//                it--;
//            }
//        }
//
//
//
//        if (rand()%G == 0)  // whatever G is, it goes between 0- (G- 1) so that means that it's a 1 in G chance
//        {
//            if (rand()%5 == 0)      // 1 in 5 chance
//            {
//                Sonar* s = new Sonar(this, 0, 60);
//                actors.push_back(s);
//            }
//            else
//            {
//                int x = rand()%61; int y = rand()%61;       // ANYWHERE EARTHLESS
//
//                while (!isClear(x, y))
//                {
//                    x = rand()%61; y = rand()%61;
//                }
//                Water* w = new Water(this, x, y);
//                actors.push_back(w);
//
//            }
//        }
//
//    }
//    else if (tunnelman->getState() == dead)
//    {
//        playSound(SOUND_PLAYER_GIVE_UP);
//        decLives();
//        return GWSTATUS_PLAYER_DIED;
//    }
//
//    return GWSTATUS_CONTINUE_GAME;
//
//}

bool StudentWorld::isClear(int x, int y)

{
    for (int i = 0 ; i < 4 ; i ++)
    {
        for (int j = 0 ; j < 4 ; j ++)
        {
            if (earths[x + i][y + j]->isVisible())
            {
                return false;
            }
        }
    }
    
    return true;
}

void StudentWorld::cleanUp()
{
    for (int i = 0; i < 64 ; i++)   // every column, up to 63
        for (int j = 0 ; j < 64 ; j ++) // all the rows up to 59
        {
            if (earths[i][j] != nullptr)    // check for shaft
                delete earths[i][j];
        }
    
    delete tunnelman;
    
    vector<Actor*>::iterator p = actors.begin();
    
    for ( ; p != actors.end() ; )
    {
        delete *p;
        p = actors.erase(p);  // one after, but it just reshuffled my shit
    }
}

bool StudentWorld::closeToProtester(int x, int y)
{
    vector<Actor*>::iterator p = actors.begin();                // for protesters when they occur
    for ( ; p != actors.end() ; p ++)
    {
        if ((*p)->annoyable() == true && (*p)->getState() != pickupableP)           // human and not the gold itself
        {
            if (sqrt(pow(((*p)->getX() - x), 2) + pow(((*p)->getY() - y), 2)) <= 3.0)
            {
                (*p)->pickedUpGold();
                return true;
            }
        }
    }
    
    return false;
}

void StudentWorld::dropGold(int x, int y)

{
    Gold* g = new Gold(pickupableP, this, x, y);
    actors.push_back(g);
    
}

void StudentWorld::increaseGold()
{
    tunnelman->increaseGold();
}

void StudentWorld::increaseSonar()
{
    tunnelman->increaseSonar();
}

void StudentWorld::increaseWater()
{
    tunnelman->increaseWater();
}

void StudentWorld::illuminate(int x, int y)
{
    vector<Actor*>::iterator p= actors.begin();
    for (;p!=actors.end(); p++)
    {
        if ( ! (*p)->isVisible() )
        {
            if (sqrt(((*p)->getX()-x)*((*p)->getX()-x) + ((*p)->getY()-y)*((*p)->getY()-y)) <= 12.0)
                (*p)->setVisible(true);
        }
    }
}

void StudentWorld::annoyTunnelMan()
{
    tunnelman->decreaseHealth(2);
    playSound(SOUND_PLAYER_ANNOYED);
}

int StudentWorld::tunnelmanGetX()
{
    return tunnelman->getX();
}

int StudentWorld::tunnelmanGetY()
{
    return tunnelman->getY();
    
}


