#ifndef GLFRAMEWORK_H_INCLUDED
#define GLFRAMEWORK_H_INCLUDED
#include "global_header.h"
using namespace std;

//Ustawienia okna i rzutowania
int windowPositionX=100;
int windowPositionY=100;
int windowWidth=400;
int windowHeight=400;
float cameraAngle=45.0f;
float sightReach=1000;
int lastTime=0;

//wsaźniki dla procedury rysowania oraz animacji obiektów
void emptyP() {}
void emptyF(float i) {}
void (*drawObjects)(void)=emptyP;
void (*timeCapture)(float)=emptyF;

//globalnie zadeklarowana macierz widoku i perspektywy
glm::mat4 P,V;


void displayFrame() //Procedura rysująca
{
	glClearColor(0,0,0,1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	drawObjects(); // rysowanie obiektów sceny

	glutSwapBuffers();
}

void setupProjection()
{
    P=glm::perspective(cameraAngle, (float)windowWidth/(float)windowHeight, 1.0f, sightReach);
}


void nextFrame(void) //procedura animacji
{
	int actTime=glutGet(GLUT_ELAPSED_TIME);
	float interval=actTime-lastTime;
	lastTime=actTime;
	interval/=1000;

	timeCapture(interval);

	glutPostRedisplay();
}


void changeSize(int w, int h)
{
	glViewport(0,0,w,h);
	windowHeight=h;
	windowWidth=w;
	setupProjection();
}


void initGLUT(int *argc, char** argv)
{
	glutInit(argc,argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowPosition(windowPositionX,windowPositionY);
	glutInitWindowSize(windowWidth,windowHeight);
	glutCreateWindow("Tank Simulator ProGL - by Elf & blacksmithy");
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

	glutReshapeFunc(changeSize); //Procedura obsługująca zmianę rozmiaru okna
	glutDisplayFunc(displayFrame); //Procedura obsługująca odświerzanie okna
	glutIdleFunc(nextFrame); //Procedura wywołująca najczęściaj jak się da (animacja)
}


void initGLEW() {
	GLenum err=glewInit();
	if (GLEW_OK!=err) {
		fprintf(stderr,"%s\n",glewGetErrorString(err));
		exit(1);
	}
}


void initOpenGL(int argc, char** argv) {
    initGLUT(&argc,argv);
    initGLEW();
	setupProjection();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // inicja;izacja mapowania klawiszy
    nKeyMap.resize(256,false);
    sKeyMap.resize(256,false);
    glutKeyboardFunc(nKeyDown);
    glutKeyboardUpFunc(nKeyUp);
    glutSpecialFunc(sKeyDown);
    glutSpecialUpFunc(sKeyUp);
}



#endif // GLFRAMEWORK_H_INCLUDED
