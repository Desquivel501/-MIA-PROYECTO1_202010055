
#ifndef COMANDO_H
#define COMANDO_H

#include <algorithm>
#include <string>
#include <vector>
#include <sstream>
#include <bits/stdc++.h>
#include <ctime>
#include <chrono>
#include <time.h>
#include <cstdlib>
#include <fstream>
#include <vector>

using namespace std;

class Comandos
{
private:
    typedef struct {
            char status;
            char type;
            char part_fit;
            int part_start;
            int part_s;
            char part_name[16];
        }partition;
    typedef struct {
            int size;
            time_t time;
            int dsk_signature;
            char dsk_fit;
            partition part_1;
            partition part_2;
            partition part_3;
            partition part_4;
        }mbr;

    typedef struct{
            int start;
            int size;
    } free_space;

    typedef struct{
            int start;
            int size;
    } used_space;

    
        
public:
    void crearDisco(int size, string unit, string fit, string path);
    void crearParticion(int size, string unit, string fit, string path, string tipo, string name);
    void mostrar(mbr master);
    int getSize(int size, string unit);
};





#endif // COMANDO_H