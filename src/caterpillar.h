#ifndef CATERPILLAR_H
#define CATERPILLAR_H
#include "global_header.h"

/*
    Łączy ze sobą zadaną ilość segmentów - tutaj używane do stworzenia okręgu
    symulującego wygląd gąsienicy - okrąg pozwala na łatwe imitowanie ruchu
    przez obrót.
*/
class Caterpillar {
    private:
        vector<Object*> vSegments;
        glm::vec3 nextPosition;
        string texture1;
        string texture2;
        string objectName;
        const float MOVE_X;
        const float MOVE_Y;
        const float MOVE_Z;
        ObjectGroup* group;

        float radius;
        float segAngle;

        float toRadians(float angle) { // Radiany = ( PI * Stopnie / 180 )
            return 3.1415f*angle/180.0f;
        }

    public:
        Caterpillar(int n, int disp, float angle2, string tex1, string tex2, string objName, ObjectGroup* objGrp)
            : MOVE_X(-28.0f), MOVE_Y(0.0f), MOVE_Z(0.0f) {
            texture1 = tex1;
            texture2 = tex2;
            objectName = objName;
            group = objGrp;

            segAngle = 360.0f/n;
            radius = (abs(MOVE_X)/2)/tan(toRadians(segAngle/2));

            float nextX, nextY;

            vSegments.push_back(new Object(objectName, texture1));

            if(angle2!=0.0f) {
                vSegments[0]->flip(-angle2);
                nextX = MOVE_X*cos(toRadians(angle2)) + 0.5*abs(MOVE_X);
                nextY = -MOVE_X*sin(toRadians(angle2)) - radius;
                nextPosition = glm::vec3(nextX, nextY, 0);
            }
            else nextPosition = glm::vec3(MOVE_X + 0.5*abs(MOVE_X), -radius, 0.0f);

            group->add(vSegments[0]);

            vSegments[0]->move(0.5*abs(MOVE_X), -radius, 0.0f);

            float angleCounter = angle2+segAngle;
            for(int i=0; i<disp-1 && i<n-1; ++i) {
                addSegment(angleCounter);
                angleCounter+=segAngle;
            }
        }

        void addSegment(float angle) { // dodaje nowy segment i łączy z aktualnymi
            float nextX, nextY;

            int n = vSegments.size();
            if(n%2 == 0) vSegments.push_back(new Object(objectName, texture1));
            else vSegments.push_back(new Object(objectName, texture2));

            vSegments[n]->flip(-angle);
            group->add(vSegments[n]);
            vSegments[n]->setLocation(nextPosition.x, nextPosition.y, 0);

            nextX = vSegments[n]->getLocation().x + MOVE_X*cos(toRadians(angle));
            nextY = vSegments[n]->getLocation().y - MOVE_X*sin(toRadians(angle));
            nextPosition = glm::vec3(nextX, nextY, 0);
        }
};

    ObjectGroup* cat1 = new ObjectGroup();
    ObjectGroup* cat2 = new ObjectGroup();
    ObjectGroup* cat3 = new ObjectGroup();
    ObjectGroup* cat4 = new ObjectGroup();
    ObjectGroup* catGrp1 = new ObjectGroup();
    ObjectGroup* catGrp2 = new ObjectGroup();
    ObjectGroup* chassis = new ObjectGroup();

    ObjectGroup* caterpillarGrp = new ObjectGroup();

#endif
