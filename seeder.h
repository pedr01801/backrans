#include <cstdlib>

class Seeder 
{
    public:
        int semilla;

        int generateSeed(int a, int b){
            if (a > b) return a;
            return a + (std::rand() % (b - a + 1));
        }
};