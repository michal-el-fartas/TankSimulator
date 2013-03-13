#ifndef CAMERA_H_INCLUDED
#define CAMERA_H_INCLUDED
#include "global_header.h"
using namespace std;

class Rotation
{
    // klasa ta pozwala obliczyć położenie punktu wokół [0,0,0]
    // na orbicie o promieniu distance w oparciu o kąty
    // xang (obrót w poziomie) oraz yang (obrót w pionie)
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
        // wprowadza ograniczenia obrotu w pionie
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
        normAngle(xang);
        normAngle(yang);
        float x,y,z,r;
        y=distance*sin((yang/180)*M_PI);
        r=distance*cos((yang/180)*M_PI);
        x=r*sin((xang/180)*M_PI);
        z=sqrt(abs(pow(r,2)-pow(x,2)));
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
            if(yang<lbY) {yang=lbY;}
            if(yang>ubY) {yang=ubY;}
            clean=false;
        }
    }
    void setAngle(int x,int y)
    {
        xang=x;
        yang=y;
        if(yang<lbY) {yang=lbY;}
        if(yang>ubY) {yang=ubY;}
        clean=false;
    }
};

class Camera
{
    //Podstawowa mechanika kamery która ma pozwalać na zmianę odległości
    //oraz odbieranie sygnału obrotu z myszki
    //Udostępnia również podstawowy interfej pozwalający na pobranie
    //lokalizacji kamery oraz obserwawanego celu co ma pozwolić na wylicznie macierzy widoku
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
    //kamera która podąża za celem i również obraca się razez z danym obiektem
    float angle;
    public:
    LockedChaseCamera(Position *pos,float dist,float ang=20): Camera(pos,dist,true)
    {
        angle=ang;
        rotation.setAngle(180,angle);
    }
    glm::vec3 getTarget()
    {
        return pos->getWorldLocation();
    }
    glm::vec3 getViewer()
    {
        return pos->getWorldLocation(vec3to4(rotation.orbit()));
    }
};

class FreeChaseCamera: public Camera
{
    //kamera która porusza się za celem
    //aczkolwiek możemy dowolnie regulować kąt pod którym patrzymy na cel
    float angle;
    glm::vec3 target;
    bool vert;
    public:
    FreeChaseCamera(Position *pos,float dist,bool limited=true): Camera(pos,dist,false)
    {
        vert=true;
        if(limited)
            {rotation.setBounds(1,85);}
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
        target=pos->getWorldLocation();
        return target;
    }
    glm::vec3 getViewer()
    {
        return target+rotation.orbit();
    }
    void move(int x,int y)
    {
        if(!vert) {y=0;}
        rotation.turn(x,y);
    }
};

class CameraOperator
{
    // klasa zarządzająca kamerami oraz odpowiedzialna za macierz widoku
    public:
    int mode;
    glm::mat4 V;
    glm::vec3 eye,center,nose;
    vector <Camera*> vcam;
    int curr;
    CameraOperator(Position *pos,float dist)
    {
        //wszystkie kamery są 'przywiązane' do obiektu
        //aczkolwiek różnią się obrotem wokół niego
        nose=glm::vec3(0.0f,1.0f,0.0f);
        vcam.push_back(new LockedChaseCamera(pos,dist));            //możliwość obrotu zablokowana
        vcam.push_back(new FreeChaseCamera(pos,dist,float(20)));    //obrót jedynie w poziomie
        vcam.push_back(new FreeChaseCamera(pos,dist));              //obrót w pionie i pozimie
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
    glm::vec3 getTarget()
    {
        return center;
    }
    glm::vec3 getViewer()
    {
        return eye;
    }
    glm::mat4 getV()
    {
        return glm::lookAt(eye,center,nose);
    }

} *cam; // wskaźnik na główną kamerę

class FPS: public AbstractObject, public Executable
{
    // klasa która oblicza i wypisuje na konsolę ilość klatek oraz animacji na sekundę
    Lag lag,shift;
    Timer timer;
    float draws,anims,dur;
    bool visible;
    public:
    FPS(): lag(1), shift(0.5,'p')
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
