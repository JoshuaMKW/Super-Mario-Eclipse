#include "includes/eclipse.hxx"
floats my_floats;

void init_floats()
{
    ((int *)&my_floats)[0] = 0x00000000;
    ((int *)&my_floats)[1] = 0x3F000000;
    ((int *)&my_floats)[2] = 0x3F700000;
    ((int *)&my_floats)[3] = 0x3F800000;
    ((int *)&my_floats)[4] = 0x40200000;
    ((int *)&my_floats)[5] = 0x41200000;
    ((int *)&my_floats)[6] = 0x41A00000;
    ((int *)&my_floats)[7] = 0x42960000;
    ((int *)&my_floats)[8] = 0x42C80000;
    ((int *)&my_floats)[9] = 0x43340000;
    ((int *)&my_floats)[10] = 0x43960000;
    ((int *)&my_floats)[11] = 0x49742400;
    ((int *)&my_floats)[12] = 0x40490FDB;
}