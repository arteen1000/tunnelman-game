
#include "Actor.h"
#include "StudentWorld.h"

// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp

#include <stack>
#include <queue>
#include <cmath>

using namespace std;

void Actor::move(Direction d)
{
    switch (d)
    {
        case GraphObject::up:
//            if (getDirection() != GraphObject::up)
//            {
//                setDirection(up);
//                break;
//            }
            if (! (getY() + 1 > 60) && ! (getWorld()->checkBoulderBlocking(up, getX(), getY())) && ! (getWorld()->checkEarthBlocking(up, getX(), getY())))
            {
            moveTo(getX(), getY() + 1 );
            }
            break;
        case GraphObject::down:
//            setDirection(down);
            if (! (getY() - 1 < 0) && ! (getWorld()->checkBoulderBlocking(down, getX(), getY())) && ! (getWorld()->checkEarthBlocking(down, getX(), getY())))
            {
            moveTo(getX(), getY() - 1 );
            }
            break;
        case GraphObject::left:
//            setDirection(left);
            if (! (getX() - 1 < 0) && ! (getWorld()->checkBoulderBlocking(left, getX(), getY())) && ! (getWorld()->checkEarthBlocking(left, getX(), getY())))
            {
            moveTo(getX() - 1, getY());
            }
            break;
        case GraphObject::right:
//            setDirection(right);
            if ( ! (getX() + 1 > 60) && ! (getWorld()->checkBoulderBlocking(right, getX(), getY())) && ! (getWorld()->checkEarthBlocking(right, getX(), getY())))
            {
            moveTo(getX() + 1, getY());
            }
            break;
        case GraphObject::none:
        default:
            return;
    }
}


void TunnelMan::doSomething()
{
//    It must have a limited version of a doSomething() method that lets the user pick a direction by hitting a directional key. If the player hits a directional key during the current tick, it updates the TunnelMan’s location to the target square, removing any Earth objects that overlap with the TunnelMan’s 4x4 graphic image. To move the TunnelMan, all this doSomething() method has to do is properly adjust the player’s X,Y coordinates using GraphObject’s moveTo() method and our graphics system will automatically animate its movement it around the oil field!
    
    if (! (getHealth() > 0))
    {
        setState(dead);
        return;
    }
    // if overlapping, destroy Earth #2
    
    int ch;
//    const int KEY_PRESS_SPACE = ' ';
//    const int KEY_PRESS_ESCAPE = '\x1b';
//    const int KEY_PRESS_TAB      = '\t';
    
    if (getWorld()->getKey(ch) == true)
    {
        switch (ch)
        {
            case KEY_PRESS_ESCAPE:
                // world plays the sound
                setState(dead);
                break;
            case KEY_PRESS_SPACE:
                if (m_water > 0)
                {
                    m_water--;
                    getWorld()->playSound(SOUND_PLAYER_SQUIRT);
                    if (getDirection()==down)
                    {
                        getWorld()->squirtGun(getDirection(), getX(),getY()-2);     // 4 in spec, prof wanted 2
                        break;
                    }
                    if (getDirection()==up)
                    {
                        getWorld()->squirtGun(getDirection(),getX(),getY()+2);
                        break;
                    }
                    if (getDirection()==right)
                    {
                        getWorld()->squirtGun(getDirection(), getX()+2,getY());
                        break;
                    }
                    if (getDirection()==left)
                    {
                        getWorld()->squirtGun(getDirection(), getX()-2,getY());
                        break;
                    }
                }
                break;
            case 'z':
            case 'Z':
                if (m_sonar > 0)
                {
                    m_sonar--;
                    getWorld()->playSound(SOUND_SONAR);
                    getWorld()->illuminate(getX(), getY());       // radius of 12
                }
                break;
            case KEY_PRESS_TAB:
                if (m_gold > 0)
                {
                    m_gold--;
                    getWorld()->dropGold(getX(),getY());    // pickupablebyprotesters & other criteria....
                }
                break;
            case KEY_PRESS_LEFT:
                if (getDirection() != left)
                {
                    setDirection(left); break;
                }
                if (! (getX() - 1 < 0) && ! (getWorld()->checkBoulderBlocking(left, getX(), getY())))
                {
                moveTo(getX() - 1, getY());
                }
                dig();
                break;
            case KEY_PRESS_RIGHT:
                if (getDirection() != right)
                {
                    setDirection(right); break;
                }
                if ( ! (getX() + 1 > 60) && ! (getWorld()->checkBoulderBlocking(right, getX(), getY())))
                {
                moveTo(getX() + 1, getY());
                }
                dig();
                break;
            case KEY_PRESS_UP:
                if (getDirection() != up)
                {
                    setDirection(up); break;
                }
                if (! (getY() + 1 > 60) && ! (getWorld()->checkBoulderBlocking(up, getX(), getY())))
                {
                moveTo(getX(), getY() + 1 );
                }
                dig();
                break;
            case KEY_PRESS_DOWN:
                if (getDirection() != down)
                {
                    setDirection(down); break;
                }
                if (! (getY() - 1 < 0) && ! (getWorld()->checkBoulderBlocking(down, getX(), getY())))
                {
                moveTo(getX(), getY() - 1 );
                }
                dig();
                break;

        }
        
    }
}

void TunnelMan::dig()
{
    for (int i = 0 ; i < 4 ; i++)
        for (int j = 0 ; j < 4 ; j++)
        {
            int x = getX()+i; int y = getY()+j;
            if (getWorld()->getEarths(x,y)->isVisible())
            {
                getWorld()->earthsKiller(x,y);
            }
        }
}


void Boulder::doSomething()
{
    if (getState() == dead)
        return;
    
    if (getState() == stable)
    {
        bool stability = false;
        
        for (int i = 0; i < 4 ; i++)
        {
            if (getWorld()->getEarths(getX() + i, getY() - 1)->isVisible())
            {
                stability = true;
                break;
            }
        }
        
        if (!stability)
        {
            setState(waiting);
            beginCountdown(30);          // change to 30
        }
        return;
    }
    
    if (getState() == waiting)
    {
        decrementCount();
        
        if (itsTime())
        {
            setState(falling);
            getWorld()->playSound(SOUND_FALLING_ROCK);
        }
        return;
    }
    
    if (getState() == falling)
    {
        if (mustStop())         // stopping condition
        {
            setState(dead);
            return;
        }
        
        
        getWorld()->runawayBoulder(getX(), getY());             // hits anything that's annoyable
        moveTo(getX(), getY() - 1);         // keep moving until hits stopping condition
        return;
    }
    
}



bool Boulder::mustStop()
{
    
    if (getY() == 0)
    {
        return true;
    }
    
    if (getWorld()->checkBoulderBlocking(down, getX(), getY())) // sets state to dead for me
        return true;

    for (int i = 0 ; i < 4 ; i++)
    {
        if (getWorld()->getEarths(getX() + i, getY() - 1)->isVisible())
        {
            return true;
        }
    }
    
    return false;
}


void Squirt::doSomething()

{
    if (getWorld()->killerSquirt(getX(), getY()))
    {
        setState(dead);
        return;             // squirt is dead and done if collides with a protester
    }                       // /setting its countdown when constructed as its implicit
    
    if (itsTime())
    {
        setState(dead);
        return;             // didn't hit anything
    }
    
    if (checkAnyBlock(getDirection(), getX(), getY()))      // blocked
    {
        setState(dead);
        return;
    }
    
    move(getDirection());
    decrementCount();       // 4 total
    
    return;
    
}

bool Actor::checkAnyBlock(GraphObject::Direction d, int x, int y)

{
    switch (d)
    {
        case GraphObject::up:
            if ((y >= 60))
            {
                return true;
            }
            else if ((getWorld()->checkBoulderBlocking(up, x, y)) || (getWorld()->checkEarthBlocking(up, x, y)))
            {
                return true;
            }
            break;
        case GraphObject::down:
            if ((y <= 0) ) // was checking blocking only from current
            {
                return true;
            }
            else if ((getWorld()->checkBoulderBlocking(down, x, y)) || (getWorld()->checkEarthBlocking(down, x, y)))
            {
                return true;
            }
            break;
        case GraphObject::left:
//            setDirection(left);
            if ((x <= 0))      // boundary block first, don't check others if bad boundary
            {
                return true;
            }
            else if ((getWorld()->checkBoulderBlocking(left, x, y)) || (getWorld()->checkEarthBlocking(left, x, y)))
            {
                return true;
            }
            break;
        case GraphObject::right:
//            setDirection(right);
            if (  (x >= 60) )
            {
                return true;
            }
            else if ((getWorld()->checkBoulderBlocking(right, x, y)) || (getWorld()->checkEarthBlocking(right, x, y)))
            {
                return true;
            }
            break;
        case GraphObject::none:
        default:
            return false;
    }
    return false;
}

void Barrel::doSomething()

{
    if (getState() == dead)
        return;
    
    if ( ! isVisible() && getWorld()->distanceToTunnelMan(getX(), getY()) <= 4.0)
    {
        setVisible(true);
        return;
    }
    
    // at this point its visible to tunnelman/activated
    
    if (getWorld()->distanceToTunnelMan(getX(), getY()) <= 3.0)
    {
        setState(dead);
        getWorld()->playSound(SOUND_FOUND_OIL); // us government's happy
        getWorld()->increaseScore(1000);
        getWorld()->decreaseBarrels();
        return;
    }
}

void Gold::doSomething()

{
    if (getState() == dead)
        return;
    
    if ( ! isVisible() && getWorld()->distanceToTunnelMan(getX(), getY()) <= 4.0)
    {
        setVisible(true);
        return;
    }
    
    if (getState() == pickupableT && getWorld()->distanceToTunnelMan(getX(), getY()) <= 3.0)
    {
        setState(dead);
        getWorld()->playSound(SOUND_GOT_GOODIE);
        getWorld()->increaseScore(10);
        getWorld()->increaseGold();         // for tunnelman
        return;
    }
    
    if (getState() == pickupableP) // second function is checker, radius
    {
        
        if (itsTime())
        {
            setState(dead);
            return;
        }
        if( getWorld()->closeToProtester(getX(), getY()))   // have this call protesters whatever
        {
        setState(dead);
        getWorld()->playSound(SOUND_PROTESTER_FOUND_GOLD);  // played sound don't play again. all protesters
        return;
        }
        
        decrementCount();
    }
    return;
}

Sonar::Sonar(StudentWorld* world, int x, int y)
 : Actor(world, TID_SONAR, x, y, right, 1.0, 2)

{
    setState(pickupableT);
    
    int THelper = 300 - 10 * (getWorld()->getLevel());
    beginCountdown(fmax(100, THelper));             // max doesn't work
    // already visible via actor
}

Water::Water(StudentWorld* world, int x, int y)
: Actor(world, TID_WATER_POOL, x, y, right, 1.0, 2)
{
    setState(pickupableT);
    
    int THelper = 300 - 10 * (getWorld()->getLevel());
    beginCountdown(fmax(100, THelper));
}                   // swater and bold - activatingobject

void Water::doSomething()
{
    if (getState() == dead)
        return;
    
    if (itsTime())
    {
        setState(dead);
        return;
    }
    
    if (getWorld()->distanceToTunnelMan(getX(), getY()) <= 3.0)
    {
        setState(dead);
        getWorld()->playSound(SOUND_GOT_GOODIE);
        getWorld()->increaseWater();
        getWorld()->increaseScore(100);
        return;
    }
    decrementCount();
}

void Sonar::doSomething()
{
    if (getState() == dead)
        return;
    
    if (itsTime())
    {
        setState(dead);
        return;
    }
    
    if (getWorld()->distanceToTunnelMan(getX(), getY()) <= 3.0)
    {
        setState(dead);
        getWorld()->playSound(SOUND_GOT_GOODIE);
        getWorld()->increaseSonar();
        getWorld()->increaseScore(75);
        return;
    }
    
    decrementCount();
}

bool Protester::staticBehavior()           // shouting, dead, resting, or falling behaviors ; staticBehaviors
{
    if (getState() == dead)
        return true;             // 1 done, DO NOTHING ELSE FOR OTHER HALF IF TRUE
    
    if (getState() == waiting || getState() == stunned)  // resting  // non resting ticks for perpendicular turn
    {
        if (restingTicks > 0)
        {
        restingTicks--;
        return true;
        }
        else
        {
            setState(waiting);  // not stunned
            restingTicks = ticksToWaitBetweenMoves; // reset
//            setState(alive);        // doesn't need it, keep it in waiting state so it comes right back
        }
    }
    
    if (getState() == falling)  // non resting tick but falling so doesn't matter
    {
        if (getX() == 60 && getY() == 60)
        {
            setState(dead);
            return true;
        }
        
        if (S.empty() == true)
        {
            findShortestRoute(60, 60, S);       // issue: bad values into the stack, only partial route that doesn't end in completion
            return true;
        }
        else if (!S.empty())
        {
            moveOne(S.top()->getX(), S.top()->getY());   // follow the path, required for direction else it would be fucked up
            S.pop();
            return true;
        }
        
        // stack to store the path, genius, backward queue, path comes from 60, 60, store backward or something of the sort
        // moves one step and returns eeverytime in the direction/to the x, y of the path
        // some sort of storage for it
        
        // PSEUDOCODE: since it moves one step at a time, have stack that holds points that lead to the exit, and this has been found by the queue and stored so in reverse order, pop one every time u move and then just keep moving until eventually it gets to 60, 60
    }
    
    if (ticksSinceLastShout >= 15)
    {
        if (getWorld()->checkFacing(getDirection(), getX(), getY()))
        {               // this should be interesting
            shout();
            ticksSinceLastShout = 0;        // increment only when not resting or falling, so from this point on
            
            decrementCount();   //  non resting tick tick without turning
            return true;
        }
    }
        // go to the rest of doSomething (don't decrement count) - movementBehavior
    
    return false;       // didn't perform behavior, except perhaps resetiing restingTicks, should be alive if it got here
    
    // may split into two parts to make it easier to manage, because of similar behavior of hardcore, but kind of splitting when it comes to finding the tunnelman, so i could do part of it before, and part of it after
}

bool Protester::checkStraightPath(int x, int y, GraphObject::Direction d)   // true if path is good
{   // know that the tunnelman's the same x if up or down and the same y if left or right, so algorithm to check if any block between my x if up/down and tunnelman's if up down, y if left/right
    // passed by value so I can make use of the x and y
    // y comparison for up/down
    
    // right means left, everything means it's opposite, as in, for case up, that means that the protester (the int y) is less than tunnelman's y
    
    switch (d)
    {
        case GraphObject::up:
            while ( ! (y ==getWorld()->tunnelmanGetY()))
            {
                if (checkAnyBlock(up, x, y))        // if blockage has occured
                {
                    return false;
                }
                else
                {
                    y++;
                }
            }
            break;
        case GraphObject::down:
            while ( ! (y ==getWorld()->tunnelmanGetY()))
            {
                if (checkAnyBlock(down, x, y))        // if blockage has occured
                {
                    return false;
                }
                else
                {
                    y--;
                }
            }
            break;
        case GraphObject::left:
            while ( ! (x ==getWorld()->tunnelmanGetX()))
            {
                if (checkAnyBlock(left, x, y))        // if blockage has occured
                {
                    return false;
                }
                else
                {
                    x--;
                }
            }
            break;
        case GraphObject::right:
            while ( ! (x ==getWorld()->tunnelmanGetX()))
            {
                if (checkAnyBlock(right, x, y))        // if blockage has occured
                {
                    return false;
                }
                else
                {
                    x++;
                }                       // PERFECT ! Could've done this an hr before but I spent an hr brainstorming over the distance and radius of 4 because it would cause different behavior for different directions
            }
            break;
        case GraphObject::none:
        default:
            return true;
    }
    return true;
}
void Protester::movementBehavior()

{
    
    // 5, straight line of sight, even if not facing tunnelman - straight line means same Xs or same Ys
            // more than 4 units away
                // could move all the way to tunnelman walking straight, anyBlock
        // then, will face tunnelman and take oneStep (moveOne)
        // will set numSquarestoMoveInCurr to 0, picks new dir next non-resting, unless tunnelman still in line of sight, continues
        // then returns
    
    
    // the radius from the protester and the tunnelman is greater than 4 units away (radius as in anchor point)
    
    bool pathClear = false;
    
    if (getX() == getWorld()->tunnelmanGetX())          // vertically simple, now find the integral
    {
        if (getY() > getWorld()->tunnelmanGetY() + 3)   // more than 4 units away (above tunnelman)     // so + 7, 4 away from tunnelman
        {
            if (checkStraightPath(getX(), getY(), down))    // check downward path because above tunnelman
            {
                pathClear = true;

                    setDirection(down);

            }
        }
        
        if (getY() < getWorld()->tunnelmanGetY())   // below    // - 4, 4 away from tunnelman
        {
            if (checkStraightPath(getX(), getY(), up))
            {
                pathClear = true;

                setDirection(up);

            }
        }
    }
                                                    // 6 due to anchor point being in lower left, same reasoning for 2 for being hmm
    if (getY() == getWorld()->tunnelmanGetY())
    {
        if (getX() > getWorld()->tunnelmanGetX() + 3)   // right of tunnelman, reasoning is + 4 if right or above because that's far from the bottom left anchor point, whereas u'd already be 4 away if left/bottom
            // i could simplify this, but I'm a tryhard         // 7 makes sense I SPENT AN HR ON THIS
        {
            if (checkStraightPath(getX(), getY(), left))    // turn left
            {
                pathClear = true;

                setDirection(left);
                          
                
            }
        }
            
        if (getX() < getWorld()->tunnelmanGetX())   // left of tunnelman, check path to the right   // - 4, radius of 4 away , no - 4 needed
        {
            if (checkStraightPath(getX(), getY(), right))
            {
                pathClear = true;

                    setDirection(right);
               

            }
        }
    }
    

    if (pathClear)
    {
        // move in current direction
        if (getWorld()->distanceToTunnelMan(getX(), getY()) > 4.0)
            move(getDirection());
        
        numSquaresToMoveInCurrentDirection = 0;
        
        decrementCount();       // 1/200 error chance but i think it's fine, covers other bases
        
        ticksSinceLastShout++;      // think about perpendicular turn in next section
        
        return;     // perfect #5, took maybe 2-3 hrs
    }
    
    
    // can't directly see #6
    
    numSquaresToMoveInCurrentDirection--;       // decrement as per #6
    
    if (numSquaresToMoveInCurrentDirection <= 0)
    {
        int randomdirection = rand()%4; // 0 - 3 is generated       /
        bool directionblocked = true;
        
        while (directionblocked)
        {
            switch (randomdirection)
            {
                case 0:
                    if (!checkAnyBlock(up, getX(), getY()))
                    {
                                     // break the loop
                        if (getDirection() != up)                   // if I don't put this hear, gets stuck and crashes game
                        {
                            setDirection(up);
                            
                            directionblocked = false;
                        }
                        else
                            randomdirection = rand()%4;             // TODO: if the direction is the same, then roll again, ambiguous decision #3, but could it be a new random direction that's the same direction?
                    }
                    else
                        randomdirection = rand()%4; // roll again
                    break;
                case 1:
                    if (!checkAnyBlock(down, getX(), getY()))
                    {
                        if (getDirection() != down)
                        {
                            setDirection(down);
                            
                            directionblocked = false;
                        }
                        else
                            randomdirection = rand()%4;
                    }
                    else
                        randomdirection = rand()%4; // roll again
                    break;
                case 2:
                    if (!checkAnyBlock(left, getX(), getY()))
                    {
                        if (getDirection() != left)
                        {
                            setDirection(left);
                            
                            directionblocked = false;
                        }
                        else
                            randomdirection = rand()%4;
                    }
                    else
                        randomdirection = rand()%4; // roll again
                    break;
                case 3:
                    if (!checkAnyBlock(right, getX(), getY()))
                    {
                        if (getDirection() != right)
                        {
                            setDirection(right);
                            
                            directionblocked = false;
                        }
                        else
                            randomdirection = rand()%4;
                    }
                    else
                        randomdirection = rand()%4; // roll again
                    break;
                default:
                    break;
            }
        }
        // 6 cont.
        
        numSquaresToMoveInCurrentDirection = rand()%53 + 8; // reset
        
        // continue with step 8? from step 6?, else statement
    }
    else            // otherwise
    {
        if (itsTime())      // hasn't made a perpendicular or 180 turn in last however many, this is perpendicular, the others were 180s of its own accord that resetted the count! I can always completely re do this. I'd just have to delete the beginCountDowns() before. I'm effectively nerfing protesters slightly
        {
            if (getDirection() == left || getDirection() == right) // perpendicular is up/down
            {
                if (!checkAnyBlock(up, getX(), getY()) || !checkAnyBlock(down, getX(), getY()))
                {   // if can move in either up or down
                    // 3 options then
                    
                    if (checkAnyBlock(up, getX(), getX()))
                    {   // can move down, not up
                        setDirection(down);
                    }
                    else if (checkAnyBlock(down, getX(), getX()))
                    { // other way
                        setDirection(up);
                    }
                    else    // can move either way, no blockage
                    {
                        int d = rand()%2;   // 0 or 1
                        
                        if (d == 0)
                            setDirection(up);
                        else
                            setDirection(down);
                    }
                       
                    numSquaresToMoveInCurrentDirection = rand()%53 + 8;
                    beginCountdown(200);    // made perp turn
                }
            }
            else      //  up/down so perp is left/right
            {
                // only make perp turn if THIS if statement is entered (that is when u perform countdown and the ticks or whatever operations, not outside it
                
                if (!checkAnyBlock(left, getX(), getY()) || !checkAnyBlock(right, getX(), getY()))  // if no blockage in one of these directions
                {   // if can move in either up or down
                    // 3 options then
                    
                    if (checkAnyBlock(left, getX(), getX()))
                    {   // can move right, not left
                        setDirection(right);
                    }
                    else if (checkAnyBlock(right, getX(), getX()))
                    { // other way
                        setDirection(left);
                    }
                    else    // can move either way, no blockage
                    {
                        int d = rand()%2;   // 0 or 1
                        
                        if (d == 0)
                            setDirection(right);
                        else
                            setDirection(left);
                    }
                    numSquaresToMoveInCurrentDirection = rand()%53 + 8;
                    beginCountdown(200);    // made perp turn
                }
            }
        }   // for itsTime
    }   // for entire else
    
    // finally step 8
    
    if (checkAnyBlock(getDirection(), getX(), getY()))  // blockage in current direction
    {
        numSquaresToMoveInCurrentDirection = 0;
        ticksSinceLastShout ++;
        decrementCount();
    }
    else    // move
    {
        move(getDirection());
        ticksSinceLastShout ++;
        decrementCount();
    }
    
}



void Actor::moveOne(int x, int y)
{           // implementing block checking here
    if ( x > getX() )
    {
        setDirection(right);
        move(right);
    }
    if ( x <getX() )
    {
        setDirection(left);
        move(left);
    }
    if ( y > getY() )
    {
        setDirection(up);
        move(up);
    }
    if ( y < getY() )
    {
        setDirection(down);
        move(down);
    }
    
}

void Protester::shout()

{
    getWorld()->playSound(SOUND_PROTESTER_YELL);
    getWorld()->annoyTunnelMan();
}


// idea: find depth-first most optimal path that takes u to the end, once ur there, come back using backtracking somehow and store it in a stack as u go

bool Protester::findShortestRoute(int endx, int endy, std::stack<Point *> &S)
{
    // move back to top after testing complete
    
    for (int i = 0; i < 64 ; i ++)              // weird bug, can't use one i and one j and reinitialize for all the loops
    {
        for (int j = 0; j < 64 ; j++ )
        {
            points[i][j].setX(i);
            points[i][j].setY(j);
        }
    }
//    for (int i = 0; i < 64 ; i ++)
//    {
//        for (int j = 0 ; j < 64 ; j++ )
//        {
//            cerr << points[i][j].getX() << endl;
//            cerr<< points[i][j].getY() << endl;
//        }       // no idea why this is happening
//    }
    
    for (int i = 0 ; i < 64 ; i ++)
    {
        for (int j = 0 ; j < 64 ; j++ )
        {
            visited[i][j] = false;
        }
    }
    for (int i = 0 ; i < 64 ; i ++)
    {
        for (int j = 0 ; j < 64 ; j++ )
        {
            previous[i][j] = nullptr;
        }
    }
    Point* current = & points[getX()][getY()];
      // insert starting point
    visited[getX()][getY()] = true;  // set discovered starting point
    
    queue<Point*> Q;
    Q.push(current);
    
//    Point* current = start;
        // the edge of my maze is my issue, not finding full path because of blocks?
    while (!Q.empty())  // exit when Q empty, means didn't discover, return false
    {
        if (Q.front()->getX() <= 60 && Q.front()->getX() >= 0 && Q.front()->getY() <= 60 && Q.front()->getY() >= 0 )               // for debugging
            current = Q.front();                // Q for optimal path (according to slides)
        else
            continue;   // skip one iteration
        
//        cerr << current->getX() << " " << current->getY() << endl;
        Q.pop();            // #4
        
        if (current->getX() == endx && current->getY() == endy) // reached end
            break;
        
//        while (current->getX() > 63 || current->getY() > 63)       // debugging
//        {
//            cerr << "here's the issue" << endl;
//
//            cerr << "searching outside the bounds somehow" << endl;
//
//            if ((current->getX() > 10000 || current->getY() > 10000))
//                Q.size();   // what's going on?     // Q is empty??
//        }
        
        if ( !checkAnyBlock(up, current->getX(), current->getY()) && visited[current->getX()][current->getY() + 1] == false)
        {
            visited[current->getX()][current->getY() + 1] = true;
            Q.push( & points[current->getX()][current->getY() + 1]);
            
            previous[current->getX()][current->getY() + 1] = current;       // now holds one before, points ot it
            
            if (current->getX() == endx && current->getY() + 1 == endy)
            {
                current = & points[endx][endy];
                break;
            }
        }
        
        if ( !checkAnyBlock(down, current->getX(), current->getY()) && visited[current->getX()][current->getY() - 1] == false)
        {
            visited[current->getX()][current->getY() - 1] = true;
            Q.push( & points[current->getX()][current->getY() - 1]);
            
            previous[current->getX()][current->getY() - 1] = current;
            
            if (current->getX() == endx && current->getY() - 1 == endy)
            {
                current = & points[endx][endy];
                break;
            }
        }
        
        if ( !checkAnyBlock(left, current->getX(), current->getY()) && visited[current->getX() - 1][current->getY()] == false)
        {   // eventually we have this massive integer TODO: fix this, it's always the same issue
            visited[current->getX() - 1][current->getY()] = true;
            Q.push( & points[current->getX() - 1][current->getY()]);
            
            previous[current->getX() - 1][current->getY()] = current;
            
            if (current->getX() - 1 == endx && current->getY() == endy)
            {
                current = & points[endx][endy];
                break;
            }
        }

        
        if ( !checkAnyBlock(right, current->getX(), current->getY()) && visited[current->getX() + 1][current->getY()] == false)
        {
            visited[current->getX() + 1][current->getY()] = true;
            Q.push( & points[current->getX() + 1][current->getY()]);
            
            previous[current->getX() + 1][current->getY()] = current;
            
            if (current->getX() + 1 == endx && current->getY() == endy)
            {
                current = & points[endx][endy];
                break;
            }
        }
    }           // missing one fucking bracket caused by DFS not to work
        

    
        // at this point, current is either at the end (in which case the queue holds the path, or not, in which case...
        // queue is empty, so need something that marks where the queue has been if it's a successful path
        
        if (current != & points[endx][endy])
            return false;   // end not found
        else
        {
            while (current != & points[getX()][getY()]) // while have not expunged all the points yet
            {
                S.push(current);
                current = previous[current->getX()][current->getY()];   // gets the one before it, reversing the order from up there, this makes complete since, makes sense, this should work, should backtrack the current and get back to where we came
            }
            
            return true;
        
        }
        
        
    }
    
    
    
    
    // needs a way to backtrack (previous)
    // needs a way to drop a breadcrumb (visited/discovered)
    // needs a way to tell if there is a block - checkAnyBlock
    // needs a way to store the path - stack
    // needs a way to search (the queue) - queue searches oldest x, y position placed in it first
    // if stack empty, call this, otherwise, use the stack to move one step at a time
    /*
     1.insert starting point
     2. mark discovered starting point
     3. if empty queue, no solution, we are done, return false?
     4. remove top point from queue
     5. if at endpoint, done (end condition) - return true
     6. if west is open(not blocked) & not visited before (undiscovered
        mark curx-1, cury discovered
        insert into queue
     7. repeat 6 for east
    8. 6 for north
    9. 6 for south
     10. go back to step 3? how do this, ah, a loop? recursion? recursion or iteration? loop.
     */



Protester::Protester(int health, StudentWorld* world, int imageID)
: Human (health, world, imageID, 60, 60, left, 1.0, 0)
{ setState(waiting);    // rest
    restingTicks = fmax(0, world->getLevel() / 4);  // current
    ticksToWaitBetweenMoves = fmax(0, 3 - world->getLevel() / 4);   // reset for restingTicks
    numSquaresToMoveInCurrentDirection = rand()%53 + 8;
    ticksSinceLastShout = 15;       // allow immediate shouting
    beginCountdown(0);      // hasn't made a perpendicular turn to start off, decrement before every return, reset it when he does make a perpendicular turn
} // could use countdown, keep in mind

void Protester::decreaseHealth(int howMuch)
{
    if (getState() == falling)
        return;                 // leaving already leave me alone
    
    if (getState() == stunned)
        return;         // can't

    Human::decreaseHealth(howMuch); // just decreases hp
        
    if (getHealth() > 0)
    {
        getWorld()->playSound(SOUND_PROTESTER_ANNOYED);
        setState(stunned);  // stunned
        restingTicks = fmax(50, 100 - getWorld()->getLevel() * 10);
    }
    else
    {
        setState(falling);          // now dead, 0 or less than 0
        getWorld()->playSound(SOUND_PROTESTER_GIVE_UP);
        restingTicks = 0;
        
        if (howMuch == 100) // boulder
        {
            getWorld()->increaseScore(500);
        }
    }
    // deal with the squirt kill independently for hardcore/regular 100 pts for regular
}

void RegularP::decreaseHealth(int howMuch)
{
    Protester::decreaseHealth(howMuch);
    
    if (getHealth() <= 0 && howMuch == 2)
        getWorld()->increaseScore(100);
}

void RegularP::pickedUpGold()
{
    getWorld()->increaseScore(25);
    setState(falling);
}

void RegularP::doSomething()
{
    if (Protester::staticBehavior())
        return;                             // return true means don't do any more
    else
        Protester::movementBehavior();
}

void HardcoreP::doSomething()
{
    if (Protester::staticBehavior())
    {
        return;
    }
    else if (getWorld()->distanceToTunnelMan(getX(), getY()) > 4.0 && getWorld()->distanceToTunnelMan(getX(), getY()) < M)     // check facing took care of the other possibility, but it could be that they're reloading their shout, so if they're close, it never calls this and just goes to movement behavior outside the else statement, aka follow mode, until within range
        // 16 is absolutely necessary for performance
    {
        if (findShortestRoute(getWorld()->tunnelmanGetX(), getWorld()->tunnelmanGetY(), T) && T.size() <= M)    // must be the number of legal moves (this is a memory sink
        {
            
            moveOne(T.top()->getX(), T.top()->getY());
            
            decrementCount();   // time since last perpendicular move, liberty here cuz it could be making perpendicular
            ticksSinceLastShout++;
        }
    }
    
    movementBehavior();
}

void HardcoreP::pickedUpGold()
{
    getWorld()->increaseScore(50);
    setState(waiting);
    restingTicks = fmax (50 , 100 - getWorld()->getLevel() * 10);
}

void HardcoreP::decreaseHealth(int howMuch)
{
    Protester::decreaseHealth(howMuch);
    
    if (getHealth() <= 0 && howMuch == 2)
        getWorld()->increaseScore(250);
}
