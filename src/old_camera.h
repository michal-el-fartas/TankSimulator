#ifndef CAMERA_H_INCLUDED
#define CAMERA_H_INCLUDED
#include "global_header.h"
using namespace std;

class Rotation
{
    float distance,speed;
    float xang,yang;
    float ubY,lbY;
    glm::vec3 pos;
    bool clean;

    public:

    Rotation(float dist,float speed=1)
    {
        this->speed=speed;
        distance=dist;
        clean=false;
        lbY=0;
        ubY=360;
    }
    void setBounds(float l,float u)
    {
        lbY=l;
        ubY=u;
    }
    void setDistance(float d)
    {
        if(d!=distance)
        {
            distance=d;
            clean=false;
        }
    }
    glm::vec3 orbit()
    {
        if(clean) {return pos;}
        float x,y,z,r;
        y=distance*sin((yang/180)*M_PI);
        r=distance*cos((yang/180)*M_PI);
        x=r*sin((xang/180)*M_PI);
        z=sqrt(pow(r,2)-pow(x,2));
        if((xang<90)||(xang>270)) {z=-z;}
        pos=glm::vec3(x,y,z);
        return pos;
    }
    void turn(int x,int y)
    {
        if(x || y)
        {
            xang+=x;
            yang+=y;
            normAngle(xang);
            normAngle(yang);
            if(yang<lbY) {yang=lbY;}
            if(yang>ubY) {yang=ubY;}
            clean=false;
        }
    }
    void setAngle(int x,int y)
    {
        xang=x;
        yang=y;
        normAngle(xang);
        normAngle(yang);
        if(yang<lbY) {yang=lbY;}
        if(yang>ubY) {yang=ubY;}
        clean=false;
    }
};

class Camera
{
    protected:
    Rotation rotation;
    Position *pos;
    float distance;
    bool scale;
    public:
    Camera(Position *pos,float dist,bool scaled): rotation(dist)
    {
        this->pos=pos;
        distance=dist;
        scale=scaled;
        //rotation=Rotation(distance);
    }
    void setDistance(float dist)
    {
        if(dist>=0)
        {
            distance=dist;
            if(scale)
                {rotation.setDistance(distance/(pos->getScale()));}
            else
                {rotation.setDistance(distance);}
        }
    }
    void zoom(float z)
    {
        if(-z<distance)
        {
            distance+=z;
            if(scale)
                {rotation.setDistance(distance/(pos->getScale()));}
            else
                {rotation.setDistance(distance);}
        }
    }
    virtual glm::vec3 getTarget() {}
    virtual glm::vec3 getViewer() {}
    virtual void move(int x,int y) {}
};

class LockedChaseCamera: public Camera
{
    float angle;
    public:
    LockedChaseCamera(Position *pos,float dist,float ang=20): Camera(pos,dist,true)
    {
        angle=ang;
        rotation.setAngle(180,angle);
    }
    glm::vec3 getTarget()
    {
        //showVec(pos->getWorldM()*glm::vec4(0.0f,0.0f,0.0f,1.0f));
        return vec4to3(pos->getWorldM()*glm::vec4(0.0f,0.0f,0.0f,1.0f));
    }
    glm::vec3 getViewer()
    {
//        glm::vec3 ang=pos->getAngle();
//
//        return getTarget()+rotation.orbit();
        return vec4to3(pos->getWorldM()*vec3to4(rotation.orbit()));
        //return getTarget()+glm::vec3(0.0f,5.0f,distance);
    }
};

class FreeChaseCamera: public Camera
{
    float angle;
    glm::vec3 target;
    bool vert;
    public:
    FreeChaseCamera(Position *pos,float dist,bool limited=true): Camera(pos,dist,false)
    {
        vert=true;
        if(limited)
            {rotation.setBounds(0,85);}
        rotation.setAngle(180,20);
    }
    FreeChaseCamera(Position *pos,float dist,float ang): Camera(pos,dist,false)
    {
        angle=ang;
        vert=false;
        rotation.setAngle(180,angle);
    }
    glm::vec3 getTarget()
    {
        //showVec(pos->getWorldM()*glm::vec4(0.0f,0.0f,0.0f,1.0f));
        target=vec4to3(pos->getWorldM()*glm::vec4(0.0f,0.0f,0.0f,1.0f));
        return target;
    }
    glm::vec3 getViewer()
    {
//        glm::vec3 ang=pos->getAngle();
//
//        return getTarget()+rotation.orbit();
        return target+rotation.orbit();
        //return getTarget()+glm::vec3(0.0f,5.0f,distance);
    }
    void move(int x,int y)
    {
        if(!vert) {y=0;}
        rotation.turn(x,y);
    }
};

class CameraOperator
{
    #define FirstPerson 0
    #define ThirdPerson 1
    #define FreeView 2
    #define FreeChase 3
    #define LockedChase 4
    public:
    int mode;
    glm::mat4 V;
    glm::vec3 eye,center,nose;
    vector <Camera*> vcam;
    int curr;
    CameraOperator(Position *pos,float dist)
    {
        nose=glm::vec3(0.0f,1.0f,0.0f);
        vcam.push_back(new LockedChaseCamera(pos,dist));
        vcam.push_back(new FreeChaseCamera(pos,dist,float(20)));
        vcam.push_back(new FreeChaseCamera(pos,dist));
        Position *ps=new Position();
        ps->setLocation(0,30,0);
        vcam.push_back(new FreeChaseCamera(ps,dist,false));
        curr=0;
    }
    void refresh()
    {
        vcam[curr]->zoom(0);
    }
    void nextCam()
    {
        curr++;
        if(curr>=vcam.size())
            {curr=0;}
        vcam[curr]->zoom(0);
    }
    void zoom(float z)
        {vcam[curr]->zoom(z);}
    void turn(int x,int y)
        {vcam[curr]->move(x,y);}
    void recalc()
    {
        center=vcam[curr]->getTarget();
        eye=vcam[curr]->getViewer();
    }
    glm::mat4 getV()
    {
        return glm::lookAt(eye,center,nose);
    }

} *cam;

class FPS: public AbstractObject, public Executable
{
    Lag lag,shift;
    Timer timer;
    float draws,anims,dur;
    bool visible;
    public:
    FPS(): lag(0.2), shift(0.5,'p')
    {
        draws=0;
        anims=0;
        timer.reset();
        visible=false;
    }
    void display()
    {
        if(shift.trigger())
        {
            visible=not(visible);
            if(visible)
            {
                printf("FPS display on\n");
                timer.reset();
                lag.reset();
                draws=0;
                anims=0;
            }
            else
                {printf("FPS display off\n");}
        }
        if(visible and lag.trigger())
        {
            dur=timer.getInterval();
            printf("   %4.1f FPS \t %4.1f anims/s\n",draws/dur,anims/dur);
            draws=0;
            anims=0;
        }
    }
    void drawObject()
    {
        draws++;
        display();
    }
    void nextFrame(float time)
    {
        anims++;
        display();
    }
};

#endif // CAMERA_H_INCLUDED
