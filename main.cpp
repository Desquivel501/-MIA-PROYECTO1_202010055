#include "analizador.cpp"

int main(){
    Analizador analisis;
    string entradacmd = "";
    while(entradacmd != "exit"){
        cout << "||========================================================||" << endl;
        cout << "||                    [MIA] PROYECTO 1                    ||" << endl;
        cout << "||========================================================||" << endl;
        cout << "[MIA]@Proyecto1:~$ ";
        getline(cin,entradacmd);   //command -> exec -path=/home/desquivel/Desktop/prueba.algo
        analisis.analizar(entradacmd);

        //mkdisk -s->3 -u->m -path->/home/desquivel/Disco1.dsk
        //fdisk -s->300 -path->/home/desquivel/Disco1.dsk -name->Particion1 
        //fdisk -s->500 -path->/home/desquivel/Disco1.dsk -name->Particion2 
    }
    return 0;
}
