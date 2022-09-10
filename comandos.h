
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
            char part_status;
            char part_fit;
            int part_start;
            int part_s;
            int part_next;
            char part_name[16];
        }ebr;
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

    struct free_space{
            int start;
            int size;
            bool operator < (const free_space& rhs) const {
                return size > rhs.size;
            }
    } ;

    struct used_space{
            int start;
            int size;
            partition part;
            bool operator < (const used_space& rhs) const {
                return start < rhs.start;
            }
    };

    struct montado{
        string id;
        partition particion;
        mbr master;
        string path;
    };



    
        
public:

    int id_mount = 1;
    partition part_null = {0,'P','F',-1,-1,"NULL"};
    vector<montado> mounted;

    void crearDisco(int size, string unit, string fit, string path);
    
    void mostrar(mbr master, string path);
    void mostrarExtendida(partition extendida, string path);


    int getSize(int size, string unit);
    void borrarDisco(string path);

    bool puedeCrearPart(mbr master);
    
    void crearParticion(int size, string unit, string fit, string path, string tipo, string name);
    void crearParticionLogica(string path, mbr master, int size, char fit, string name);

    void borrarParticion(string path, string name);

    bool existeExtendida(mbr master);
    bool nombreRepetido(FILE file, mbr master, char nombre[]);

    partition getPartExtendida(mbr master){
        if(master.part_1.type == 'E') return master.part_1;
        if(master.part_2.type == 'E') return master.part_2;
        if(master.part_3.type == 'E') return master.part_3;
        return master.part_4;
    }

    void mount(string path, string nombre_s, string id);

    void unmount(string id){
        int index = -1;
        int i = 0;
        for (montado m : mounted){
            if (m.id == id){
                index = i;
            }
            i++;
        }
        if(index == -1) {
            cout<<"[MIA]@Proyecto1:~$ La particion no ha sido montada"<<endl;
            return;
        }
        mounted.erase(mounted.begin() + index);
        cout<<"[MIA]@Proyecto1:~$ Se ha desmontado la particion: " <<id<<endl;
    }

    void repDisco(string id, string path);
    string repDiscoEx(string path, mbr master, int size_total);


    void repMBR(string id, string path);

    void getMount(){
        cout<<"Particiones Montadas:" <<endl;
        for (montado m : mounted){
            cout<<"-- ID: "<<m.id<<endl;
        }
    }

    void actualizarMount(string path, string nombre, mbr master);

    void reporteMBR(string path, string id);

    string reporteEBR(string path, mbr master);

    void createDir(string path);

    bool existeNombre(string path, string nombre_s);

};

#endif // COMANDO_H