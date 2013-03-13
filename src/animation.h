#ifndef ANIMATION_H_INCLUDED
#define ANIMATION_H_INCLUDED
#include "global_header.h"

class Executable
{
    // Klasa abstrakcyjna(intefejs) utworzony dla elementów zmieniających się w czasie
    // w założeniu wykonywany dopuki active==true
    //okresowo wywoływany jest nextFrame gdzie 'time' to czas od poprzedniego wywołania
    public:
    bool active;
    virtual void nextFrame(float time) {}
    Executable() {active=true;}
    ~Executable() {active=false;}
};

class Movement: public Executable //klasa wykonuje zaprogramowany ruch na podanym obiekcie
{
    // typy wykonywanego ruchu
    #define TURN 1
    #define MOVE 2
    //warunek wykonywania ruchu
    #define FACT 0
    #define DEST 1
    #define INF 2
    #define NKEY 3
    #define SKEY 4
    Position *pos;
    int mode;
    float x,y,z;
    int keyLock,key;
    bool *perm;
    float remainingTime;
    glm::vec3 destination,distance;
    bool check(float time) // sprawdza czy ruch może sie wykonać
    {
        if(keyLock==FACT)
        {
            return *perm;
        }
        else if(keyLock==DEST)
        {
            if(remainingTime<=time)
            {
                remainingTime=0;
                pos->setLocation(destination.x,destination.y,destination.z);
                return false;
            }
            else
            {
                remainingTime-=time;
                return true;
            }
        }
        else if(keyLock==NKEY)
        {
            return keyPressed(char(key));
        }
        else if(keyLock==SKEY)
        {
            return keyPressed(float(key));
        }
        return true;
    }
    void exec(float time) // procedura wykonania ruchu
    {
        if(mode==TURN)
        {
            pos->lean(x*time);
            pos->turn(y*time);
            pos->flip(z*time);
        }
        else if(mode==MOVE)
        {
            if(keyLock==-2)
                {pos->moveLocation(x*time,y*time,z*time);}
            else
                {pos->move(x*time,y*time,z*time);}
        }
    }

    public:
    void nextFrame(float time) // przeciążona procedura klasy 'Executable'
    {
        if(check(time))
        {
            exec(time);
            return;
        }
        active=(keyLock!=-2);
    }
    Movement(): Executable(){}
    Movement(Position *p,int mode,float x,float y,float z): Executable()
    {
        //ten konstruktor programuje stały(nieprzerwany) ruch obiektem p
        //x,y,z to prędkości ruchu wzdłuż osi (dla mode=MOVE)
        //lub (dla mode=TURN) obrotu wokół osi
        pos=p;
        this->mode=mode;
        this->x=x;
        this->y=y;
        this->z=z;
        remainingTime=0;
        keyLock=INF;
    }
    Movement(Position *p,int mode,float x,float y,float z,float key): Executable()
    {
        // podobne działanie do poprzedniego konstruktora z tą różnicą
        // że działa jedynie kiedy wciśnięty jest klawisz specjalny określony w 'key'
        pos=p;
        this->mode=mode;
        this->x=x;
        this->y=y;
        this->z=z;
        remainingTime=0;
        this->key=key;
        keyLock=SKEY;
    }
    Movement(Position *p,int mode,float x,float y,float z,char key): Executable()
    {
        // różni się od poprzedniego tym że zamiast klawisz specjalnego mamy klawisz alfanumeryczny
        pos=p;
        this->mode=mode;
        this->x=x;
        this->y=y;
        this->z=z;
        remainingTime=0;
        this->key=key;
        keyLock=NKEY;
    }
    Movement(Position *p,int mode,float x,float y,float z,bool *condition): Executable()
    {
        // podobny do dwóch poprzednich konstruktorów jednak działa nie dla klawisza
        // a dla zmiennej boolowskiej - jeżeli *condition==true ruch się wykonuje
        pos=p;
        this->mode=mode;
        this->x=x;
        this->y=y;
        this->z=z;
        keyLock=FACT;
        perm=condition;
        remainingTime=0;
    }
    Movement(Position *p,int mode,glm::vec3 dest,float speed): Executable()
    {
        // Ten konstruktor programuje inny typ ruchu - ruch do celu
        // wykonuje się tak długo aż nie osiągnie określonego położenia
        // lub kąta podanego w 'dest' - ruch wykonywany z prędkością speed
        pos=p;
        this->mode=mode;
        destination=dest;
        if(mode==MOVE)
            {distance=dest-(pos->getLocation());}
        if(mode==TURN)
            {distance=dest-(pos->getAngle());}
        keyLock=DEST;

        remainingTime=0;
        remainingTime+=distance.x*distance.x;
        remainingTime+=distance.y*distance.y;
        remainingTime+=distance.z*distance.z;
        remainingTime=sqrt(remainingTime)/speed;

        x=distance.x/remainingTime;
        y=distance.y/remainingTime;
        z=distance.z/remainingTime;
    }
};

class Animation: public Executable // klasa zarządza innymi klasami typu 'Executable'
{
    int old,last;
    protected:
    vector <Executable*> movs;
    public:
    Animation(): Executable()
    {
        last=-1;
        old=0;
    }
    void add(Executable* mv)
    {
        movs.push_back(mv);
    }
    void nextFrame(float time)
    {
        for(int i=0;i<movs.size();i++)
        {
            if (movs[i]->active)
            {
                movs[i]->nextFrame(time);
            }
            else
            {
                // jeżeli obiekt skończył pracę zostaje to odnotowane
                if(i>last)
                {
                    old++;
                    last=i;
                }
            }
        }
        if(old>5) // garbage colector - usuwa stare obiekty jeżeli jest ich za dużo
        {
            for(int i=0;i<movs.size();i++)
            {
                if(!movs[i]->active)
                {
                    delete movs[i];
                    swap(movs[i],movs.back());
                    movs.pop_back();
                    i--;
                }
            }
            last=-1;
            old=0;
        }
    }
};
// główna nadrzędna animacja wykonywana bezpośrednio przez framework
// jeżeli chcemy dodać kolejne animacje dodajemy je właśnie do tej klasy
Animation animation;

#endif // ANIMATION_H_INCLUDED
