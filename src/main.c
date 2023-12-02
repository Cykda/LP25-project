#include <stdio.h>
#include <stdlib.h>
#include "copyfile.h"





int main(int argc, char** argv)
{
    
    
    copyfile("./assets/out.txt", "./assets/out2.txt"); // fonction qui les copies
    
    system("md5sum ./assets/out.txt"); // faites pas gaffe à ça c'est juste pour les comparer
    system("md5sum ./assets/out2.txt");
    
    return 0;
}
