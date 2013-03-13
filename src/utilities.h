#ifndef UTILITIES_H_INCLUDED
#define UTILITIES_H_INCLUDED
#include "global_header.h"
float sqrt2=sqrt(2);

template <class X> class Library
{
    //klasa służącza do przechowywania oraz odwoływania się po nazwach
    vector <X> content;
    vector <string> index;
    int curr;
    public:
    Library() {curr=0;}
    void add(string name,X item)
    {
        index.push_back(name);
        content.push_back(item);
    }
    X get(string name)
    {
        for(int i=0;i<index.size();i++)
        {
            if(index[i]==name)
            {
                curr=i;
                return content[i];
            }
        }
        cout << " Item " << name << " not found!!!" << endl;
    }
    X next()
    {
        curr=(curr+1)%content.size();
        return content[curr];
    }
    void clear()
    {
        for(int i=0;i<index.size();i++)
        {
            delete content[i];
        }
    }
};

char *strclone(string str)
{
    //zamiana stringa na dynamiczną tablicę charów
    char *tab;
    tab=new char[str.size()];
    strcpy(tab,str.c_str());
    return tab;
}

float normAngle(float &angle)
{
    //normalizacja kąta
    while(angle>=360) {angle-=360;}
    while(angle<0) {angle+=360;}
    return angle;
}

//wyświetlanie wektora na konsoli
void showVec(glm::vec3 v)
{
    cout << v.x << " ";
    cout << v.y << " ";
    cout << v.z << endl;
}

void showVec(glm::vec4 v)
{
    cout << v.x << " ";
    cout << v.y << " ";
    cout << v.z << " ";
    cout << v.w << endl;
}

//zamiana wektora 3-elementowego na 4-elementowy i na odwrót
glm::vec3 vec4to3(glm::vec4 v)
{
    return glm::vec3(v.x,v.y,v.z);
}

glm::vec4 vec3to4(glm::vec3 v)
{
    return glm::vec4(v.x,v.y,v.z,1);
}


class Timer
{
    //klasa służąca do zlicznia czasu ora przedziałów czasowych
    float firstTime,currTime,lastTime;
    void update()
    {
        lastTime=currTime;
        currTime=float(glutGet(GLUT_ELAPSED_TIME))/1000;
    }
    public:
    void reset()
    {
        firstTime=lastTime=currTime=float(glutGet(GLUT_ELAPSED_TIME))/1000;
    }
    Timer()
    {
        reset();
    }
    float getInterval()
    {
        update();
        return currTime-lastTime;
    }
    float getTotal()
    {
        update();
        return currTime-firstTime;
    }
};


class Lag
{
    //klasa która pozwala ustawienie minimalnego przedziału
    //czasowego między wywołaniami lub naciśnięciem klawisza
    #define NO_KEY 1
    #define S_KEY 2
    #define N_KEY 3
    float last,timeout,sKey;
    int mode;
    char nKey;
    Timer timer;
    bool ready()
    {
        if(last)
        {
            last-=timer.getInterval();
            if(last<=0)
            {
                last=0;
                return true;
            }
            else {return false;}
        }
        else {return true;}
    }
    bool requested()
    {
        switch(mode)
        {
            case NO_KEY: return true;
            case S_KEY: return keyPressed(sKey);
            case N_KEY: return keyPressed(nKey);
        }
        return false;
    }
    public:
    Lag(float duration)
    {
        last=0;
        timeout=duration;
        mode=NO_KEY;
    }
    Lag(float duration,char key)
    {
        last=0;
        timeout=duration;
        mode=N_KEY;
        nKey=key;
    }
    Lag(float duration,float key)
    {
        last=0;
        timeout=duration;
        mode=S_KEY;
        sKey=key;
    }
    bool trigger()
    {
        if(ready()&&requested())
        {
            last=timeout;
            timer.reset();
            return true;
        }
        return false;
    }
    void reset()
    {
        last=timeout;
        timer.reset();
    }
};

string getPath(string str)
{
    //pobiera ścieżkę z adresu podanego w str
    for(int i=str.size()-1;i>=0;i--)
    {
        if(str[i]=='/')
        {
            return str.substr(0,i+1);
        }
    }
    return "";
}


string getFileName(string str)
{
    //pobiera nazwę pliku z adresu podanego w str
    for(int i=str.size()-1;i>=0;i--)
    {
        if(str[i]=='/')
        {
            return str.substr(i+1,str.size()-i-1);
        }
    }
    return str;
}


//konstrukcja pozwalająca na odpowiednie formatowanie
//logów w konsoli przez manipulację wcięciami
string tab="";
void inTab()
{
    tab+="   ";
}
void outTab()
{
    tab=tab.substr(0,tab.size()-3);
}


template <class X>
void memGet(X *target,X *source)
{
    //funkcja templateowa pozwalająca kopiować zawartość wskazywaną przez wskaźniki
    (*target)=(*source);
}

//deklaracja funcji do sprawdzania położenia w świecie
bool inBounds(glm::vec3 point);
bool inBounds(glm::vec4 point);
bool inBounds(glm::mat4 object);
bool isVisible(glm::vec3 point);
bool isVisible(glm::vec4 point);
bool isVisible(glm::mat4 object);

#endif // UTILITIES_H_INCLUDED
