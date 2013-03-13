#ifndef TANK_CONTROL_H_INCLUDED
#define TANK_CONTROL_H_INCLUDED
#include "global_header.h"
using namespace std;
int knockbackCount=0;

class Knockback: public Executable
{
    //klasa która odpowiada za animację odrzutu i bezwładności czołgu
    //jako parametry wejściowe podajemy pod jakim kątem działa siła,
    //moc (kąt maksymalny) wychylenia oraz czas trwania
    public:
    Position *tank;
    float xang,zang,max,cur,rad,scale;
    Knockback(Position *tank,float direction,float power,float duration): Executable()
    {
        this->tank=tank;
        direction*=(M_PI/180);
        zang=-power*sin(direction);
        xang=power*cos(direction);
        cur=0;
        max=duration;
        rad=M_PI/duration;
        knockbackCount++;
    }
    void nextFrame(float time)
    {
        cur+=time;
        if(cur>=max)
        {
            cur=max;
            active=false;
            knockbackCount--;
        }
        if(knockbackCount)
        {
            scale=sin(cur*rad)-sin((cur-time)*rad);
            tank->lean(xang*scale);
            tank->flip(zang*scale);
        }
        else
        {
            tank->setAngle(1,0);
            tank->setAngle(3,0);
        }
    }
};

class WeaponControl: public Animation
{
    //klasa która odpowiada za kontorlę wieżyczki oraz wystrzału
    //jej zadaniem jest również tworzenie i wystrzeliwanie pocisków
    //oraz wywoływanie odrzutu przy strzale
    Tank *tank;
    float rise;
    bool up,down;
    Lag fire;
    Lag fireMode;
    bool beamWeapon;
    public:
    WeaponControl(Tank *tank,float speed): Animation(),fire(tank->turret->reloadTime,' '),fireMode(0.5,'0')
    {
        this->tank=tank;
        add(new Movement(tank->turret,TURN,0,tank->turret->tSpeed,0,'a'));
        add(new Movement(tank->turret,TURN,0,-tank->turret->tSpeed,0,'d'));
        up=down=false;
        add(new Movement(tank->turret->gunGroup,TURN,tank->turret->rSpeed,0,0,&up));
        add(new Movement(tank->turret->gunGroup,TURN,-tank->turret->rSpeed,0,0,&down));

        if(tank->turret->sSpeed)
            {add(new Movement(tank->turret->gun2,TURN,0,0,tank->turret->sSpeed,' '));}
        if(tank->turret->ammo.find("beam")!=string::npos)
        {
            add(new Beam(tank->turret));
            beamWeapon=true;
        }
    }
    void nextFrame(float time)
    {
        Animation::nextFrame(time);
        rise=tank->turret->gunGroup->getAngle().x;
        up=keyPressed('w') and rise<=tank->turret->rAngle;
        down=keyPressed('s') and rise>=0;

        if(fireMode.trigger())
            {Bullet::slowMotion=not(Bullet::slowMotion);}
        if((!beamWeapon)&&(fire.trigger()))
        {
            if(tank->turret->mode!=SERIAL)
                {new Bullet(tank->turret);}
            if(tank->turret->mode!=SINGLE)
                {new Bullet(tank->turret,true);}

            if(tank->turret->recoil)
            {
                float power=tank->turret->recoil*cos(rise*M_PI/180);
                animation.add(new Knockback(tank->tank,tank->turret->getAngle().y,power,1));
            }
        }
    }

};

class Steering: public Executable
{
    //klasa odpowiada za kontrolę napędu czołgu oraz przechwytywanie sterowania
    float forw_speed,turn_speed,prev_spd;
    int left,right,spd,dir;
    float leftCat, rightCat; // aktualne prędkości każdej z gąsienic
    Tank *tank;
    bool advanced; //tryb sterowania
    Lag changeMode;
    public:
    Steering(Tank *tank,float speed,bool pro=false): Executable(), changeMode(0.5,'c')
    {
        //obliczanie prędkości prostoliniowych i obrotowych
        this->tank=tank;
        forw_speed=speed;
        turn_speed=speed/50;
        turn_speed*=(180/M_PI);
        advanced=pro;
        prev_spd=spd=0;
    }
    void recalc()
    {
        // przelicza kierunek ruchu czołgu
        if(changeMode.trigger())
            {advanced=not advanced;}
        prev_spd=spd;
        if(advanced)
        {
            left=keyPressed(KEY_SHIFT_L)-keyPressed(KEY_CTRL_L);
            right=keyPressed(KEY_SHIFT_R)-keyPressed(KEY_CTRL_R);
            dir=right-left;
            spd=right+left;
            if(spd) {spd/=abs(spd);}
        }
        else
        {
            spd=keyPressed(KEY_UP)-keyPressed(KEY_DOWN);
            dir=keyPressed(KEY_LEFT)-keyPressed(KEY_RIGHT);
            if(spd==0)
                {dir*=2;}
            else if(spd<0)
                {dir=-dir;}
        }
    }

    void getSpeeds(float &left,float &right)
    {
        if(spd<0) {dir=-dir;}
        if(!dir)
        {
            left=right=spd*forw_speed;
        }
        else if(dir>0)
        {
            right=forw_speed;
            left=(1-abs(dir))*forw_speed;
        }
        else
        {
            dir=abs(dir);
            left=forw_speed;
            right=(1-abs(dir))*forw_speed;
        }
    }

    void nextFrame(float time)
    {
        //wykonuje animacje
        recalc();
        tank->move(spd*forw_speed*time,0,0);
        tank->turn(dir*turn_speed*time);
        if(prev_spd!=spd)
        {
            //odrzut związany z rusznaiem lub hamowaniem
            if(spd>prev_spd)
                {animation.add(new Knockback(tank->tank,0,2,0.7));}
            else
                {animation.add(new Knockback(tank->tank,180,2,0.5));}
        }

        /* ******************* Caterpillar's moves *********************** */
        this->getSpeeds(leftCat, rightCat);
        leftCat /= -42.0f;
        rightCat /= -42.0f;
        cat2->flip(leftCat);
        cat4->flip(leftCat);
        cat1->flip(rightCat);
        cat3->flip(rightCat);
        /* *************************************************************** */
    }
};

class TankControl: public Animation
{
    //klasa która inicjuje klasy sterujące dany czołg
    Tank *tank;
    public:
    TankControl(Tank *tank)
    {
        this->tank=tank;
        add(new Steering(tank,50/tank->armor->weight,false));
        add(new WeaponControl(tank,2));
    }
};

class Garage: public Executable
{
    //klasa której głównym zadaniem jest umożliwienie zmian części czołgu
    //oraz inicjacja sterowania
    Lag turretSwitch,armorSwitch;
    TankControl *TC;
    Tank *tank;
    public:
    Garage(Tank *tank): turretSwitch(0.5,'1'), armorSwitch(0.5,'2'), tank(tank)
    {
        TC=new TankControl(tank);
    }
    void reloadControls()
    {
        delete TC;
        TC=new TankControl(tank);
        cam->refresh();
    }
    void nextFrame(float time)
    {
        if(turretSwitch.trigger())
        {
            *tank->turret=*(new Turret(*libTurret.next()));
            nextActualTurretMode();
            tank->armor->attach(tank->turret);
            reloadControls();
        }
        if(armorSwitch.trigger())
        {
            *tank->armor=*(new Armor(*libArmor.next()));
            tank->drive->attach(tank->armor);
            tank->armor->attach(tank->turret);
            reloadControls();
        }
        TC->nextFrame(time);
    }
};

#endif // TANK_CONTROL_H_INCLUDED
