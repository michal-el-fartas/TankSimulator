#include "global_header.h"
using namespace std;

int main(int argc, char** argv)
{
    drawObjects=show;
    timeCapture=doTime;
    initOpenGL(argc,argv);
    glutPassiveMotionFunc(motion);

    FileLoader("resources/global_resources.def");
    world=new World(1000);

    FPS counter;
    world->add(&counter);
    animation.add(&counter);

    Tank panzer;
    Garage control(&panzer);
    animation.add(&control);
    world->add(&panzer);

    cam=new CameraOperator(panzer.turret,40);

    explosionSingle = new Explosion(2.0f, 800, 28);
    explosionSingle->turn(-90.0f);
    explosionSingle->lean(-90.0f);
    world->add(explosionSingle);

    explosionDouble1 = new Explosion(1.5f, 400, 28);;
    explosionDouble1->turn(-90.0f);
    explosionDouble1->lean(-90.0f);
    explosionDouble1->setScale(0.8f);
    world->add(explosionDouble1);

    explosionDouble2 = new Explosion(1.5f, 400, 28);;
    explosionDouble2->turn(-90.0f);
    explosionDouble2->lean(-90.0f);
    explosionDouble2->setScale(0.8f);
    world->add(explosionDouble2);

    explosionGun = new Explosion(1.0f, 100, 4);
    explosionGun->turn(-90.0f);
    explosionGun->lean(-90.0f);
    explosionGun->setScale(0.4f);
    world->add(explosionGun);

    /* ****************** Caterpillar ************************** */

    Object chassisSource("drive", "metal2");
    caterpillarGrp->add(&chassisSource);
    Caterpillar ctp1(7, 7, 0.0f, "metal", "metal", "seg", cat1);
    Caterpillar ctp2(7, 7, 0.0f, "metal", "metal", "seg", cat2);
    Caterpillar ctp3(7, 7, 0.0f, "metal", "metal", "seg", cat3);
    Caterpillar ctp4(7, 7, 0.0f, "metal", "metal", "seg", cat4);

    catGrp1->add(cat1);
    catGrp1->add(cat2);

    catGrp2->add(cat3);
    catGrp2->add(cat4);

    cat2->move(0,0,-76);
    cat4->move(0,0,-76);

    catGrp1->setScale(0.1, 0.15, 0.5);
    catGrp2->setScale(0.1, 0.15, 0.5);

    catGrp1->turn(90.0);
    catGrp2->turn(90.0);

    catGrp1->move(-21.5f, 4.5f, 19.0f);
    catGrp2->move(21.5f, 4.5f, 19.0f);

    caterpillarGrp->add(catGrp1);
    caterpillarGrp->add(catGrp2);
    caterpillarGrp->add(&chassisSource);

    /* **************************************************** */

	glutMainLoop();
    delete cam;
    delete world;
    libGroup.clear();
    libObject.clear();
    libTexture.clear();
    libModel.clear();
    libShader.clear();

	return 0;
}
