#ifndef EXPLOSION_H
#define EXPLOSION_H
#include "global_header.h"
#include "randomGenerator.h"

/*
    Wartości R, G, B - przechowuje kolory
*/
class Color {
    public:
        float r, g, b;

        Color() {}

        Color(float v) {
            this->r = v/255.0f;
            this->g = v/255.0f;
            this->b = v/255.0f;
        }

        Color(float r, float g, float b) {
            this->r = r/255.0f;
            this->g = g/255.0f;
            this->b = b/255.0f;
        }
};

/*
    Tablica kolorów wykorzystywanych w pierwszej (1/3) fazie wybuchu
*/
const Color PARTICLES_COLORS_1[] = {
    Color(255.0f, 250.0f, 0.0f),
    Color(255.0f, 238.0f, 0.0f),
    Color(255.0f, 227.0f, 0.0f),
    Color(255.0f, 215.0f, 0.0f),
    Color(255.0f, 200.0f, 0.0f),
    Color(255.0f, 193.0f, 30.0f),
    Color(255.0f, 175.0f, 15.0f),
    Color(255.0f, 163.0f, 34.0f),
    Color(255.0f, 137.0f, 46.0f),
    Color(255.0f, 127.0f, 26.0f),
    Color(255.0f, 107.0f, 13.0f),
    Color(255.0f, 97.0f, 12.0f),
    Color(255.0f, 82.0f, 3.0f),
    Color(255.0f, 69.0f, 0.0f),
    Color(238.0f, 54.0f, 0.0f),
};

const int PARTICLES_COLORS_1_SIZE = 15; // rozmiar powyższej tablicy

/*
    Tablica kolorów wykorzystywanych w drugiej (2/3) fazie wybuchu
*/
const Color PARTICLES_COLORS_2[] = {
    Color(88.0f, 68.0f, 37.0f),
    Color(93.0f, 72.0f, 45.0f),
    Color(93.0f, 76.0f, 49.0f),
    Color(93.0f, 81.0f, 56.0f),
    Color(92.0f, 86.0f, 67.0f),
    Color(91.0f, 91.0f, 76.0f),
    Color(90.0f, 89.0f, 74.0f),
    Color(88.0f, 85.0f, 70.0f),
    Color(83.0f, 80.0f, 67.0f),
    Color(81.0f, 79.0f, 66.0f),
    Color(80.0f, 79.0f, 66.0f),
    Color(79.0f, 78.0f, 66.0f),
    Color(77.0f, 77.0f, 65.0f),
    Color(75.0f, 75.0f, 60.0f),
    Color(68.0f, 68.0f, 57.0f)
};

const int PARTICLES_COLORS_2_SIZE = 15; // rozmiar powyższej tablic

/*
    Tablica kolorów wykorzystywanych w trzeciej (3/3) fazie wybuchu
*/
const Color PARTICLES_COLORS_3[] {
    Color(230.0f),
    Color(227.0f),
    Color(222.0f),
    Color(218.0f),
    Color(214.0f),
    Color(204.0f),
    Color(191.0f),
    Color(186.0f),
    Color(180.0f),
    Color(172.0f),
    Color(170.0f),
    Color(168.0f),
    Color(160.0f),
    Color(156.0f),
    Color(150.0f)
};

const int PARTICLES_COLORS_3_SIZE = 15; // rozmiar powyższej tablic

/*
    Klasa pomocnicza przechowująca najbardziej podstawowe wartości
    dla danej cząsteczki. Wymagana do utworzenia tzw. wzorca wybuchu
    (na początku wykonywania programu tworzony jest wzorzec zachowań
    cząstek, który potem za kazdym razem jest jedynie odtwarzany a nie
    tworzony (nie wymaga dodatkowych obliczeń czy losowań).
 */
class ParticleValues {
    public:
        ParticleValues(float x, float y, float z, float spX, float spY, float spZ,
                       float r, float g, float b, float a, int ttl) {
            this->startX = x;
            this->startZ = y;
            this->startY = z;

            this->speedX = spX;
            this->speedY = spY;
            this->speedZ = spZ;

            this->colorR = r;
            this->colorG = g;
            this->colorB = b;
            this->colorAlpha = a;

            this->ttl = ttl;
        }

        ParticleValues() {}

        float startX, startY, startZ;
        float speedX, speedY, speedZ;
        float colorR, colorG, colorB, colorAlpha;
        int ttl;
};

/*
    Reprezentuję każdą pojedynczą "cząsteczkę"
*/
class Particle : public ParticleValues, public Object {
    private:
        float r_x, r_y, r_z;
        int colorStage2, colorStage3;

        void setColor(int stage) { // losuje odpowiedni dla danej fazy wybuchu kolor
            srand(time(0));
            Color tempColor;

            if(stage==2) {
                tempColor = PARTICLES_COLORS_2[(rand()%(PARTICLES_COLORS_2_SIZE))];
                colorR = tempColor.r;
                colorG = tempColor.g;
                colorB = tempColor.b;
                return;
            }
            else {
                tempColor = PARTICLES_COLORS_3[(rand()%(PARTICLES_COLORS_3_SIZE))];
                colorR = tempColor.r;
                colorG = tempColor.g;
                colorB = tempColor.b;
                return;
            }
        }

    public:
        Particle(string md, string tx, ParticleValues p) : Object(md, tx), ParticleValues() {
            this->startX = p.startX;
            this->startY = p.startY;
            this->startZ = p.startZ;

            this->speedX = p.speedX;
            this->speedY = p.speedY;
            this->speedZ = p.speedZ;

            this->colorR = p.colorR;
            this->colorG = p.colorG;
            this->colorB = p.colorB;
            this->colorAlpha = 0;

            this->ttl = p.ttl;

            this->colorStage3 = this->ttl/3;
            this->colorStage2 = this->colorStage3*2;

            this->setLocation(startX, startY, startZ);

            this->lean(90.0f);
            this->flip(90.0f);
            this->setScale(25.0f);
        }

        void setValues(ParticleValues p) { // inicjalizuje cząsteczki wg utworzonego wzorca
            this->startX = p.startX;
            this->startY = p.startY;
            this->startZ = p.startZ;

            this->speedX = p.speedX;
            this->speedY = p.speedY;
            this->speedZ = p.speedZ;

            this->colorR = p.colorR;
            this->colorG = p.colorG;
            this->colorB = p.colorB;
            this->colorAlpha = 0;

            this->ttl = p.ttl;

            this->setLocation(startX, startY, startZ);
        }

        void initMove(float speedScale) { // wykonuje pierwszy ruch cząsteczek podczas eksplozji
            this->moveLocation(speedX*speedScale, speedY*speedScale, speedZ*speedScale);
        }

        void drawObject() // rysuje wierzchołek nie wykonując żadnych przekształceń czy obliczeń
        {
            if(ttl == 0) return;
            if(this->getLocation().y < 0) {
                ttl = 0;
                return;
            }

            glDepthMask(false); // wymagane do uzyskania przezroczystości sterowanej kanałem alfa
            shader->use();
            glUniformMatrix4fv(shader->getUniformLocation("P"),1, false, glm::value_ptr(P));
            glUniformMatrix4fv(shader->getUniformLocation("V"),1, false, glm::value_ptr(V));
            glUniformMatrix4fv(shader->getUniformLocation("M"),1, false, glm::value_ptr(worldM));

            glUniform1f(shader->getUniformLocation("colorR"), colorR);
            glUniform1f(shader->getUniformLocation("colorG"), colorG);
            glUniform1f(shader->getUniformLocation("colorB"), colorB);
            glUniform1f(shader->getUniformLocation("colorAlpha"), colorAlpha);

            glUniform1i(shader->getUniformLocation("textureMap0"),0);
            glUniform1i(shader->getUniformLocation("textureMap1"),1);
            tex->use();
            model->drawModel();

            glDepthMask(true);
        }

        void drawObject2() // rysuje obiekt wykonując kolejny ruch - uaktualnia kolory i wykonuje przekształcenia
        {
            if(ttl == 0) return;
            if(this->ttl == colorStage2) {
                setColor(2);
                this->colorAlpha = 0.25f;
            }
            else if(this->ttl == colorStage3) {
                setColor(3);
                this->colorAlpha = 0.65f;
            }

            glDepthMask(false);

            this->moveLocation(speedX, speedY, speedZ);
            this->speedX *= 0.8f;
            this->speedZ *= 0.8f;

            if(this->speedY > 0) this->speedY -= 3.0f;
            else this->speedY = -0.7f;

            shader->use();
            glUniformMatrix4fv(shader->getUniformLocation("P"),1, false, glm::value_ptr(P));
            glUniformMatrix4fv(shader->getUniformLocation("V"),1, false, glm::value_ptr(V));
            glUniformMatrix4fv(shader->getUniformLocation("M"),1, false, glm::value_ptr(worldM));

            glUniform1f(shader->getUniformLocation("colorR"), colorR);
            glUniform1f(shader->getUniformLocation("colorG"), colorG);
            glUniform1f(shader->getUniformLocation("colorB"), colorB);
            glUniform1f(shader->getUniformLocation("colorAlpha"), colorAlpha);

            glUniform1i(shader->getUniformLocation("textureMap0"),0);
            glUniform1i(shader->getUniformLocation("textureMap1"),1);
            tex->use();
            model->drawModel();
            glDepthMask(true);

            --ttl;
        }
};

/*
    Przechowuje wszystkie cząsteczki zawarte w danej eksplozji, inicjalizuje wybuchy,
    tworzy początkowy wzorzec eksplozji.
*/
class Explosion : public ObjectGroup {
    private:
        float x, y, z, range, delay; // range - radius of sphere; delay - delay for explosion
        int precision, duration, precisionHalf, durationMax;
        vector <Particle*> group;
        vector<ParticleValues*> explosionPattern; // uses to increase performance
        bool activated; // true - explosion in progress; false - no explosion
        Timer timer;
        bool stage; // 2 stages to increase performance

        glm::vec3 direction;
        glm::vec3 expLocation;
        float a, h;

        static const float epsilon; // precision level for comparisions (float values)
        float rValue;

        inline float pow2(float val) {
            return val*val;
        }

        inline float radiansToDegrees(float radians) {
            return (radians*180.0f)/3.1416f;
        }

        inline float degreesToRadians(float degrees) {
            return (degrees*3.1416f)/180.0f;
        }

        void setInSphere(float &x, float &y, float &z, float r, float rx, float ry, float rz) {
            // sets particle into the required sphere
            while(pow2(x-rx)+pow2(z-rz)>pow2(r)) {
                x = rg.getFloat(rx-r, rx+r);
                z = rg.getFloat(rz-z, rz+z);
            }

            while(pow2(r)<pow2(x-rx)+pow2(y-ry)+pow2(z-rz)) {
                y = rg.getFloat(ry, ry+r);
            }
        }

    public:
        Explosion(float range, int precision, int durat) : ObjectGroup() {
            this->x = 0;
            this->y = 0;
            this->z = 0;
            this->range = range;
            this->precision = precision;
            this->precisionHalf = precision/2;
            this->durationMax = durat;

            this->activated = false;

            this->stage = true;

            this->rValue = 0;

            float r, leftX, rightX, leftZ, rightZ, speedX, speedY, speedZ, newX, newY, newZ, scaleFactor;
            float colorR, colorB, colorG;
            int tempTtl;
            Color tempColor;

            cout << "Creating explosion pattern: ";

            explosionPattern.clear();
            explosionPattern.resize(precision, 0);

            r = range;
            leftX = x-r;
            rightX = x+r;
            leftZ = z;
            rightZ = z+r;

            rg.start();

            for(int i=0; i<precision; ++i) {
                newX = rg.getFloat(leftX, rightX);
                newY = rg.getFloat(y, y+r);
                newZ = rg.getFloat(leftZ, rightZ);

                setInSphere(newX, newY, newZ, r, x, y, z);

                speedX = newX-x;
                speedY = abs(newY+y);
                speedZ = newZ-z;

                scaleFactor = 16.0f;
                speedX *= scaleFactor;
                speedY *= 0.9f*scaleFactor;
                speedZ *= scaleFactor;

                tempTtl = rg.getInt(durationMax/4, durationMax/2);

                tempColor = PARTICLES_COLORS_1[rg.getInt(0, PARTICLES_COLORS_1_SIZE-1)];

                colorR = tempColor.r;
                colorG = tempColor.g;
                colorB = tempColor.b;

                explosionPattern[i] = new ParticleValues(newX, newY, newZ, speedX, speedY, speedZ,
                                           colorR, colorG, colorB, 0.0f, tempTtl);
            }

            cout << "OK\n";

            // Creating all necessary Particles in vector group

            this->timer.reset();
            group.clear();

            for(int i=0; i<precision; ++i) {
                group.push_back(new Particle("particle", "part", *explosionPattern[i]));
            }
        }

        virtual void recalcM(glm::mat4 M)
        {
            worldM=M*getM();
            for(int i=0;i<group.size();i++)
            {
                group[i]->recalcM(worldM);
            }
        }

        void setAngle2(float xAngle, float yAngle, float zAngle) {
            // user-friendly version of setAngle
            this->setAngle(1, xAngle);
            this->setAngle(2, yAngle);
            this->setAngle(3, zAngle);
        }

        int getQuater(float x_, float z_) {
            if(x_ > 0.0f && z_ > 0.0f) return 1;
            if(x_ < 0.0f && z_ > 0.0f) return 2;
            if(x_ < 0.0f && z_ < 0.0f) return 3;
            if(x_ > 0.0f && z_ < 0.0f) return 4;
            return 0;
        }

        void kaboom(float x, float y, float z, float delayTime) {
            // let's start fireworks!
            this->activated = true;
            this->delay = delayTime;
            this->stage = true;
            this->timer.reset();

            this->duration = durationMax;

            this->setAngle(1, 0);
            this->setAngle(2, -90.0f);
            this->setAngle(3, 0);

            /* ****************** Rotate if necessary ******************** */
                direction = cam->getViewer();
                expLocation = this->getWorldLocation();

                a = abs(direction.x - expLocation.x);
                if(a < epsilon) a = 0.0f;
                h = abs(direction.z - expLocation.z);
                if(h < epsilon) h = 0.0f;

                if(direction.x - expLocation.x == 0.0f || abs(direction.x-expLocation.x) < epsilon) {
                    this->setAngle2(0.0f, -90.0f, 0.0f);
                }
                else if(direction.z - expLocation.z == 0.0f || abs(direction.z - expLocation.z) < epsilon) {
                    this->setAngle2(0.0f, 90.0f, 0.0f);
                }
                else {
                    if(a == 0 || h == 0) cout << "ZERO_ERR!";

                    if((direction.x < expLocation.x && direction.z < expLocation.z)
                            || (direction.x > expLocation.x && direction.z > expLocation.z)) {
                                this->setAngle2(0.0f, radiansToDegrees(atan2(a, h)), 0.0f);
                            }
                    else {
                        this->setAngle2(0.0f, -radiansToDegrees(atan2(a, h)), 0.0f);
                    }
                }

                /* *********************************************************** */

            for(int i=0; i<precision; ++i) {
                group[i]->setValues(*explosionPattern[i]);
                group[i]->initMove(this->rg.getFloat(0.0f, 0.9f));
            }

            this->setLocation(x, y, z);
        }

        void drawObject()
        {
            if(!isActivated()) {
                return;
            }

            if(this->delay > 0) { // delay in progress
                if(timer.getTotal()>=delay) { // end delay
                    delay = -1.0f;
                }
                else {
                    return;
                }
            }

            if(duration == 0) {
                duration = -1;
                activated = false;
                timer.reset();
                return;
            }

            --duration;

            if(stage) {


                /* ****************** Rotate if necessary ******************** */
                direction = cam->getViewer();
                expLocation = this->getWorldLocation();

                a = abs(direction.x - expLocation.x);
                if(a < epsilon) a = 0.0f;
                h = abs(direction.z - expLocation.z);
                if(h < epsilon) h = 0.0f;

                if(direction.x - expLocation.x == 0.0f || abs(direction.x-expLocation.x) < epsilon) {
                    this->setAngle2(0.0f, -90.0f, 0.0f);
                }
                else if(direction.z - expLocation.z == 0.0f || abs(direction.z - expLocation.z) < epsilon) {
                    this->setAngle2(0.0f, 90.0f, 0.0f);
                }
                else {
                    if(a == 0 || h == 0) cout << "ZERO_ERR!";

                    if((direction.x < expLocation.x && direction.z < expLocation.z)
                            || (direction.x > expLocation.x && direction.z > expLocation.z)) {
                                this->setAngle2(0.0f, radiansToDegrees(atan2(a, h)), 0.0f);
                            }
                    else {
                        this->setAngle2(0.0f, -radiansToDegrees(atan2(a, h)), 0.0f);
                    }
                }

                /* *********************************************************** */

                for(int i=0; i<precisionHalf; ++i) {
                    group[i]->Particle::drawObject2();
                }
                for(int i=precisionHalf; i<precision; ++i) {
                    group[i]->Particle::drawObject();
                }
            }
            else {
                for(int i=0; i<precisionHalf; ++i) {
                    group[i]->Particle::drawObject();
                }
                for(int i=precisionHalf; i<precision; ++i) {
                    group[i]->Particle::drawObject2();
                }
            }

            stage = !stage;
        }

        bool isActivated() {
            return activated;
        }

        RandomGenerator rg;
};
const float Explosion::epsilon = 0.1f;

Explosion* explosionSingle;
Explosion* explosionDouble1;
Explosion* explosionDouble2;
Explosion* explosionGun;

unsigned int actualTurretMode = 0;
    // 0 - single
    // 1 - double
    // 2 - gatling
    // 3 - prism

void nextActualTurretMode() {
    ++actualTurretMode;
    if(actualTurretMode == 4) actualTurretMode = 0;
}

#endif
