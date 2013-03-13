#ifndef KEY_MAP_H_INCLUDED
#define KEY_MAP_H_INCLUDED
#include "global_header.h"
using namespace std;
//Poniżej mamy dwa zestawy narzędzi służącze do mapowania klawiszy
//prefix 'n' oznacza klawisze alfanumeryczne natomiast 's' klawisze specjalne
// xKeyMap - mapa klawiszy
// xKeyDown - wciśnięcie klawisza
// xKeyUp - puszczenie klawisza
// keyPresed() - zwraca czy klawisz jest wciśnięty - jedna przeciążona funkcja dla wszystkich klawiszy

vector <bool> nKeyMap;
vector <bool> sKeyMap;

void sKeyDown(int z,int x,int y)
{
    sKeyMap[z]=true;
}
void sKeyUp(int z,int x,int y)
{
    sKeyMap[z]=false;
}

void nKeyDown(unsigned char z,int x,int y)
{
    nKeyMap[int(z)]=true;
}
void nKeyUp(unsigned char z,int x,int y)
{
    nKeyMap[int(z)]=false;
}

bool keyPressed(char z)
{
    return nKeyMap[int(z)];
}

bool keyPressed(float z)
{
    return sKeyMap[int(z)];
}


//poniższe zmienne były potrzebne ze względu na konflikt typów
float KEY_UP=GLUT_KEY_UP;
float KEY_DOWN=GLUT_KEY_DOWN;
float KEY_LEFT=GLUT_KEY_LEFT;
float KEY_RIGHT=GLUT_KEY_RIGHT;
float KEY_PAGE_UP=GLUT_KEY_PAGE_UP;
float KEY_PAGE_DOWN=GLUT_KEY_PAGE_DOWN;
float KEY_SHIFT_L=GLUT_KEY_SHIFT_L;
float KEY_SHIFT_R=GLUT_KEY_SHIFT_R;
float KEY_CTRL_L=GLUT_KEY_CTRL_L;
float KEY_CTRL_R=GLUT_KEY_CTRL_R;
float KEY_ALT_L=GLUT_KEY_ALT_L;
float KEY_ALT_R=GLUT_KEY_ALT_R;
float KEY_F1=GLUT_KEY_F1;
float KEY_F2=GLUT_KEY_F2;
float KEY_F3=GLUT_KEY_F3;
float KEY_F4=GLUT_KEY_F4;
float KEY_F5=GLUT_KEY_F5;
float KEY_F6=GLUT_KEY_F6;
float KEY_F7=GLUT_KEY_F7;
float KEY_F8=GLUT_KEY_F8;
float KEY_F9=GLUT_KEY_F9;
float KEY_F10=GLUT_KEY_F10;
float KEY_F11=GLUT_KEY_F11;
float KEY_F12=GLUT_KEY_F12;



#endif // KEY_MAP_H_INCLUDED
