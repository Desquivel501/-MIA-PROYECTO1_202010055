#include "analizador.cpp"

int main(){
    Analizador analisis;
    string entradacmd = "";
    while(entradacmd != "exit"){
        cout << "||========================================================||" << endl;
        cout << "||                    [MIA] PROYECTO 1                    ||" << endl;
        cout << "||========================================================||" << endl;
        cout << "[MIA]@Proyecto1:~$ ";
        getline(cin,entradacmd);   //command -> exec -path->/home/desquivel/Desktop/script.mia
        analisis.analizar(entradacmd);

        //mkdisk -s->3 -u->m -path->/home/desquivel/Desktop/Disco1.dsk
        //rmdisk -path->/home/desquivel/Desktop/Disco1.dsk
        //fdisk -s->1000 -path->/home/desquivel/Desktop/Disco1.dsk -name->Particion1 
        //fdisk -s->1500 -path->/home/desquivel/Desktop/Disco1.dsk -name->Particion2 
        //fdisk -s->400 -path->/home/desquivel/Desktop/Disco1.dsk -name->Particion3
        //fdisk -s->100 -path->/home/desquivel/Desktop/Disco1.dsk -name->Particion4

        // mount -path->/home/desquivel/Desktop/Disco1.dsk -name->Particion1 
        // mount -path->/home/desquivel/Desktop/Disco1.dsk -name->Particion4 

        //fdisk -s->1000 -path->/home/desquivel/Desktop/Disco1.dsk -name->Particion1 -t->E
        //fdisk -s->100 -path->/home/desquivel/Desktop/Disco1.dsk -name->ParticionL1 -t->L
        //fdisk -s->200 -path->/home/desquivel/Desktop/Disco1.dsk -name->ParticionL2 -t->L
        //fdisk -s->200 -path->/home/desquivel/Desktop/Disco1.dsk -name->ParticionL3 -t->L

        //fdisk -s->200 -path->/home/desquivel/Desktop/Disco1.dsk -name->ParticionL5 -t->L
        //rep -id->"551Disco1" -path->/home/desquivel/Desktop/mbr.jpg" -name->mbr
        
    }
    return 0;
}
