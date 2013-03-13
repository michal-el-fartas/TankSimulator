#ifndef RANDOM_GENERATOR_H
#define RANDOM_GENERATOR_H
#include <cstdlib>
#include <ctime>

using namespace std;

class RandomGenerator {
    public:
        RandomGenerator() {
            srand(time(0));
        }

        void start() {
            srand(time(0));
        }

        int getInt(int mini, int maxi) {
            rand();
            return (rand()%(maxi+1))+mini;
        }

        float getFloat0_1() {
            rand();
            return (float)rand()/(float)RAND_MAX;
        }

        float getFloat(float mini, float maxi) {
            rand();
            //return (static_cast<float>((rand()%(static_cast<int>(maxi)+1))+static_cast<int>(mini))
            //        + (float)rand()/(float)RAND_MAX);
            return ((float)rand()/((float)RAND_MAX/(maxi-mini)))+mini;
        }

        float getSign() {
            rand();
            if ((static_cast<float>((rand()%(1+1))-1)
                    + (float)rand()/(float)RAND_MAX) < 0) return -1.0f;
            else return 1.0f;
        }
};

#endif

