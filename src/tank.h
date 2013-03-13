#ifndef TANK_H_INCLUDED
#define TANK_H_INCLUDED
#include "global_header.h"
using namespace std;

class Turret: public ObjectGroup
{
    //klasa która zawiera i odpowiednio grupuje elementy wierzyczki
    public:
    Object *base,*gun1,*gun2;
    ObjectGroup *gunGroup;
    float tSpeed,rSpeed,sSpeed,rAngle; //prędkości wieżyczki
    float reloadTime,recoil; //czas przeładowania oraz odrzut
    float range,fastFire,slowFire; //zasięg i czas lotu pocisku
    string ammo; //nazwa pocisku
    // typy budowy wierzyczki i rodzaje lufy
    int mode;
    #define SINGLE 1
    #define SERIAL 2
    #define PARALLEL 3
    void init()
    {
        //odpowiednia aranżacja obiektów w wierzyczce
        //w zależności od typu budowy
        gunGroup=new ObjectGroup();
        gunGroup->add(gun1);
        if(mode!=SINGLE)
        {
            if(mode==SERIAL)
            {
                gun1->attach(gun2);
            }
            gunGroup->add(gun2);
        }
        base->attach(gunGroup);
        add(gunGroup);
        add(base);
    }
    Turret(): ObjectGroup()
    {
        //wartości domyślne
        reloadTime=2;
        tSpeed=50;
        rSpeed=10;
        sSpeed=0;
        rAngle=95;
        recoil=10;
        mode=SINGLE;
        ammo="cube";
        //range=1000;
        range=300;
        fastFire=0.5;
        slowFire=3;
    }
    Turret(string turret_name);
    Turret(string base_name,string gun1_name,string gun2_name="",int mode=SINGLE): ObjectGroup()
    {
        base=new Object(base_name);
        gun1=new Object(gun1_name);
        if(mode!=SINGLE)
            {gun2=new Object(gun2_name);}
        reloadTime=2;
        tSpeed=50;
        rSpeed=10;
        sSpeed=0;
        rAngle=95;
        recoil=10;
        init();
    }
};
Library <Turret*> libTurret;

class Bullet: public Object, public Executable
{
    //klasa będąca jednocześnie obiektem oraz animacją pocisku
    Timer timer;
    float g,gx,gy,v0;
    bool action,first;
    glm::mat4 rootM;
    public:
    static bool slowMotion;
    void calcGrav(float range,float duration)
    {
        //obliczenie fizyki pocisku czyli grawitacji
        //oraz prędkości wylotowej pocisku
        g=30/pow(duration,2);
        v0=range/duration;
    }
    void calcStrike(float angle)
    {
        //obliczenie składowych prędkości potrzebnych
        //do wyliczenia toru lotu w zależności od kąta wystezału
        angle*=M_PI/180;
        gx=g*sin(angle);
        gy=g*cos(angle);
    }
    Bullet(string obj,float range,float duration): Object(obj), Executable()
    {
        calcGrav(range,duration);
        action=false;
        first=true;
    }
    Bullet(Turret *t,bool secondary=false): Object(t->ammo), Executable()
    {
        //utworzenie pocisku dopasowanego dla danej wieżyczki
        if(slowMotion)
            {calcGrav(t->range,t->slowFire);}
        else
            {calcGrav(t->range,t->fastFire);}
        calcStrike(t->gunGroup->getAngle().x);
        action=false;
        first=true;
        if(!secondary)
            {t->gun1->attach(this);}
        else
            {t->gun2->attach(this);}
        t->gunGroup->add(this);
        animation.add(this);
    }
    void nextFrame(float time)
    {
        if(action)
        {
            //obliczanie aktualnego czasu
            //oraz położenia w świecie
            float t=timer.getTotal();
            float sx=v0*t-gx*t*t/2;
            float sy=-(gy*t*t/2);
            setLocation(0,sy,-sx);

            //obliczenie kąta ułożenia pocisku
            float angle=atan((-gy*t)/(v0-gx*t));
            angle*=180/M_PI;
            setAngle(1,angle);

            //pocisk jest rysowany oraz animowany tylko dopuki znajduje się w granicach świata
            //jeżeli uderzy w ziemie lub wyleci poza świat to przestaje istnieć
            action=isVisible(worldM);

            if(!action)
            {
                // można wykonać swoisty "destruktor"
                // nie będziemy usuwać zupełnie pocisku z obiegu - zbyt dużo komplikacji
                // po wykonaniu zadania draw, recalc i frame stają się prawie puste

                if(actualTurretMode == 0) { // single
                    explosionSingle->kaboom(this->getWorldLocation().x, 0.0f, this->getWorldLocation().z, 0);
                }
                else if(actualTurretMode == 1) { // double
                    if(explosionDouble1->isActivated()) {
                        explosionDouble2->kaboom(this->getWorldLocation().x, 0.0f, this->getWorldLocation().z, 0);
                    }
                    else {
                        explosionDouble1->kaboom(this->getWorldLocation().x, 0.0f, this->getWorldLocation().z, 0);
                    }
                }
                else if(actualTurretMode == 2) { // gatling
                    explosionGun->kaboom(this->getWorldLocation().x, 0.0f, this->getWorldLocation().z, 0);
                }
            }
        }
    }
    void drawObject()
    {
        if(action)
        {
            Object::drawObject();
        }
    }
    void recalcM(glm::mat4 M)
    {
        //pocisk przy wystrzeleniu pobiera macierz obiektu nadrzędnego
        //i zachowuje jego kopię dzięki czemu lot pocisku jest niezależny
        //od ruchu czołgu
        if(action)
            {Object::recalcM(rootM);}
        else if(first)
        {
            Object::recalcM(M);
            rootM=worldM;
            action=true;
            setLocation(0,0,0);
            first=false;
        }
    }
};
bool Bullet::slowMotion=false;

class Beam: public Object, public Executable
{
    //inny typ wystrzału - promień laserowy
    float height,width;
    float charge; //poziom naładowania promienia
    bool charged; //promień naładowany
    Turret *turret;
    public:
    float brightness;
    Beam(Turret *t,float width=5,float height=19): Object(t->ammo), Executable()
    {
        brightness=0;
        this->height=height;
        this->width=width;
        charge=0;
        charged=false;
        turret=t;
        t->gun1->attach(this);
        t->gunGroup->add(this);
        setScale(width,height,0.1);
    }
    void nextFrame(float time)
    {
        if(keyPressed(' '))
        {
            if(!charged)
            {
                charge+=time;
                if(charge>=turret->reloadTime)
                {
                    charge=turret->reloadTime;
                    charged=true;
                    setScale(width,height,sightReach);
                }
            }
        }
        else
        {
            if(charged)
            {
                setScale(width,height,0.1);
                charged=false;
            }
            if(charge)
            {
                charge-=time;
                if(charge<0)
                    {charge=0;}
            }
        }
        brightness=charge/turret->reloadTime;
        brightness=sin(brightness*M_PI_2);
    }
};


class Armor: public Object
{
    //obiekt pancerza czołgu
    public:
    Armor(string name);
    Armor(Object obj): Object(obj) {}
    float hp,weight;
};
Library <Armor*> libArmor;

class Drive: public ObjectGroup
{
    Object *base;
    ObjectGroup *drive;
};

class Tank: public ObjectGroup
{
    //klasa grupująca w całość wszystkie elementy czołgu
    glm::vec3 prevPos;
    public:
    ObjectGroup *tank;
    Turret *turret;
    Armor *armor;
    ObjectGroup *drive;
    Tank(Turret *t,Armor *a,Object *d): ObjectGroup()
    {
        tank=new ObjectGroup();
        turret=t;
        armor=a;
        //drive=d;
        drive = caterpillarGrp;
        drive->setSocket(0.0f, 16.0f, 0.5f);

        drive->attach(armor);
        armor->attach(turret);

        tank->add(turret);
        tank->add(armor);
        tank->add(drive);
        tank->turn(-90);
        add(tank);
    }
    Tank(string t="single",string a="light",string d="Drive"): ObjectGroup()
    {
        *this=*(new Tank(new Turret(t),new Armor(a),new Object(d)));
    }
    // WAŻNE!!!!  sprawdza czy znajduje się w granicach świata!
    // Aktualnie ustawione na blokowanie ruchu!
    void recalcM(glm::mat4 M)
    {
        //tu można ograniczać ruch czołgu i np. blokować jego wyjazd poza granice świata
        /*if(!inBounds(M*getM()))
            {setLocation(prevPos.x,prevPos.y,prevPos.z);}
        */
        if(!inBounds(M*getM())) {
            //setLocation(prevPos.x,prevPos.y,prevPos.z);
            explosionSingle->kaboom(prevPos.x, prevPos.y, prevPos.z, 0.0f);
            setLocation(0.0f, prevPos.y, 0.0f);
        }

        ObjectGroup::recalcM(M);
        prevPos=getLocation();
    }
};
#endif // TANK_H_INCLUDED
