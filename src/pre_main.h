#ifndef PRE_MAIN_H_INCLUDED
#define PRE_MAIN_H_INCLUDED
#include "global_header.h"
using namespace std;

void camControl()
{
    //procedura zczytuje z klawiszy polecenia
    //manipulacji kamerą i wyświetlaniem
    static Lag changeView(0.5,'v');
    if (changeView.trigger()) {cam->nextCam();}
    if (keyPressed(KEY_PAGE_UP)) {cam->zoom(-1);}
    if (keyPressed(KEY_PAGE_DOWN)) {cam->zoom(1);}
    if (keyPressed(KEY_F12))
    {
        glutFullScreen();
        glutSetCursor(GLUT_CURSOR_NONE);
    }
    if (keyPressed(KEY_F11))
    {
        glutReshapeWindow(windowWidth,windowHeight);
        glutSetCursor(GLUT_CURSOR_INHERIT);
    }
    if (keyPressed(char(27)))
    {
        exit(0);
    }
	if (keyPressed('e'))
    {
        explosionSingle->kaboom(0.0f, 0.0f, 0.0f, 0);
    }
}

void show()
{
    //odpowiada za odpowiednie rysowanie świata
    world->recalcM(glm::mat4(1.0f));    //przeliczenie macierzy M dla wszystkich elementów świata
    cam->recalc();  //przeliczenie położenia kamery
    V=cam->getV();  //pobranie macierzy V dla bierzącej kamery
    world->drawObject();    //rysowanie świata
}

void doTime(float time)
{
    animation.nextFrame(time); //wywołanie animacji
    camControl();   //kontrola kamery
    glutPostRedisplay();
}

void motion(int x,int y)
{
    //funkcja obsługi myszki
    static int msx=x,msy=y;
    cam->turn(msx-x,msy-y);
    msx=x;
    msy=y;
}

#endif // PRE_MAIN_H_INCLUDED
