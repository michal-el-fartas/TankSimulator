#ifndef OBJECTS_H_INCLUDED
#define OBJECTS_H_INCLUDED
#include "global_header.h"
using namespace std;
Library <Model*> libModel;
Library <Texture*> libTexture;

class Position
{
    //Podstawowa klasa to orientacji się po świecie
    //pozwala na określanie położenia na świecie oraz odpowiednich kątów
    //jej głównym zadaniem jest obliczania macierzy M
    glm::mat4 M,M_0;
    glm::vec3 location; //lokalizacja w przestrzeni
    glm::vec3 movement; //przesunięcie po osiach własnych modelu
    glm::vec4 zero;
    float horizontal,vertical,side; // kąty obrotu
    glm::vec3 scale;   // skala względem każdej z osi
    bool clean;     // przedstawia czy pozycja została jakkolwiek zmodyfikowana
    glm::vec3 socket;   //'zaczep' pozwala na podpięcie innego obiektu w określone miejsce względm tego
    protected:
    glm::mat4 worldM;  //macierz M w przestrzeni świata
    public:
    glm::vec3 getLocation() //pobiera lokację względem obiektu nadrzędnego
    {
        return location;
    }
    glm::vec3 getWorldLocation(glm::vec4 point=glm::vec4(0,0,0,1)) //pobiera lokację w świecie
    {
        return vec4to3(worldM*point);
    }
    void setLocation(float x,float y,float z) //ustawia lokację
    {
        location=glm::vec3(x,y,z);
        movement=glm::vec3(0);
        clean=false;
    }
    void moveLocation(float x,float y,float z)//wykonuje przesunięcie względem współżędnych świata
    {
        location+=glm::vec3(x,y,z);
        clean=false;
    }
    void move(float x,float y,float z)//wykonuje przesunięcie względem współżędnych modelu
    {
        movement=glm::vec3(x,y,z);
        clean=false;
    }
    void rescale(float s)
    {
        scale*=s;
        clean=false;
    }
    void setScale(float x,float y,float z)
    {
        scale=glm::vec3(x,y,z);
        clean=false;
    }
    void setScale(float s)
    {
        scale=glm::vec3(s);
        clean=false;
    }
    float getScale(int type=1)
    {
        switch(type)
        {
            case 1: return scale.x;
            case 2: return scale.y;
            case 3: return scale.z;
        }
    }
    void turn(float angle)
    {
        horizontal+=angle;
        clean=false;
    }
    void flip(float angle)
    {
        vertical+=angle;
        clean=false;
    }
    void lean(float angle)
    {
        side+=angle;
        clean=false;
    }
    void setAngle(int type,float angle)
    {
        if(type==1)
            {side=angle;}
        else if(type==2)
            {horizontal=angle;}
        else if(type==3)
            {vertical=angle;}
        clean=false;
    }
    glm::vec3 getAngle()
    {
        return glm::vec3(side,horizontal,vertical);
    }
    glm::mat4 getM()
    {
        if(clean) {return M;}
        M=M_0;
        M=glm::translate(M,location);

        M=glm::rotate(M,horizontal,glm::vec3(0.0f,1.0f,0.0f));
        M=glm::rotate(M,vertical,glm::vec3(0.0f,0.0f,1.0f));
        M=glm::rotate(M,side,glm::vec3(1.0f,0.0f,0.0f));

        M=glm::translate(M,movement);
        M=glm::scale(M,scale);

        glm::vec4 point=M*zero;
        location.x=point.x;
        location.y=point.y;
        location.z=point.z;
        movement=glm::vec3(0);

        clean=true;
        return M;
    }
    Position()
    {
        zero=glm::vec4(0,0,0,1);
        worldM=M_0=glm::mat4(1.0f);
        location=glm::vec3(0.0f);
        movement=glm::vec3(0.0f);
        horizontal=0;
        vertical=0;
        side=0;
        scale=glm::vec3(1);
        clean=false;
    }
    void setSocket(float x,float y,float z)
    {
        socket=glm::vec3(x,y,z);
    }
    void attach(Position &pos) // przypina dany pos do naszego zaczepu
    {
        glm::vec3 newLoc=getLocation()+socket;
        pos.setLocation(newLoc.x,newLoc.y,newLoc.z);
    }
    void attach(Position *pos)
    {
        glm::vec3 newLoc=getLocation()+socket;
        pos->setLocation(newLoc.x,newLoc.y,newLoc.z);
    }
    glm::mat4 getWorldM()
    {
        return worldM;
    }
};

class AbstractObject
{
    //interfejs obiektu który udostępnia funkcje przeliczenia macierzy M oraz narysowaniu obiektu
    public:
    virtual void drawObject() {}
    virtual void recalcM(glm::mat4 M) {}
};

class Object: public AbstractObject, public Position
{
    //każdy element wyświetlany na scenie jest klasy Object
    public:
    // wskaźniki na wspólne modele, textury i shadery
    Model *model;
    Texture *tex;
    ShaderProgram *shader;
    virtual void recalcM(glm::mat4 M)
    {
        //oblicza macierz M w operciu o macierz włąsną modelu
        //oraz macierz obiektu nadrzędnego podaną przez parametr
        worldM=M*getM();
    }
    virtual void drawObject()
    {
        //procedura rysująca obiekt - wpierw ustawia prognam cienuijący
        //oraz udostępnia mu odpowiednie zmienne oraz tekstury
        //a nasztępnie wykożystuje model do rysowania
        shader->use();
        glUniformMatrix4fv(shader->getUniformLocation("P"),1, false, glm::value_ptr(P));
        glUniformMatrix4fv(shader->getUniformLocation("V"),1, false, glm::value_ptr(V));
        glUniformMatrix4fv(shader->getUniformLocation("M"),1, false, glm::value_ptr(worldM));
        glUniform1i(shader->getUniformLocation("textureMap0"),0);
        glUniform1i(shader->getUniformLocation("textureMap1"),1);
        tex->use();
        model->drawModel();
    }
    Object(): Position() {}
    Object(string obj);
    Object(string md,string tx): Position()
    {
        //ładuje wskażniki do modelu i tekstury
        //z wspólnych bibliotek elementów zdefiniowanych w plikach
        model=libModel.get(md);
        tex=libTexture.get(tx);
        shader=model->shaderProgram;
    }
};

class ObjectGroup: public AbstractObject, public Position
{
    //klasa służąca do przechowywania innych obietków i ich rysowania
    //pozwala na manipulacje położeniem całej grupy obiektów
    vector < AbstractObject* > group;
    public:
    virtual void recalcM(glm::mat4 M)
    {
        worldM=M*getM();
        for(int i=0;i<group.size();i++)
        {
            group[i]->recalcM(worldM);
        }
    }
    virtual void drawObject()
    {
        for(int i=0;i<group.size();i++)
        {
            group[i]->drawObject();
        }
    }
    void add(AbstractObject *o)
    {
        group.push_back(o);
    }
    ObjectGroup(): Position() {}
    ObjectGroup(string obj);
};

#endif // OBJECTS_H_INCLUDED
