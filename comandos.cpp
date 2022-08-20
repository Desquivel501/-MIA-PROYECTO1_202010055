#include "comandos.h"


void Comandos::crearDisco(int size, string unit, string fit, string path){

    if(size == -1 || path == ""){
        cout<<"[MIA]@Proyecto1:~$ Cantdad de parametros incorrecto en: MKDISK"<<endl;
        return;
    }
    if(unit == ""){
        unit = "k";
    }

    int size_file = 0;

    if(unit == "k" || unit == "K"){
        size_file = size;
    }
    else if(unit == "m" || unit == "M"){
        size_file = size * 1024;
    }
    else if(unit == "g" || unit == "G"){
        size_file = size*1024*1024;
    }

    char bloque[1024];
    for (int i = 0; i < 1024; i++)
    {
        bloque[i] = '\0';
    }

    FILE *archivo = fopen(path.c_str(), "wb+");

    int limite = 0;
    while (limite != size_file)
    {
        fwrite(&bloque, 1024, 1, archivo);
        limite++;
    }

    srand(time(0));
    auto t_actual = std::chrono::system_clock::now();
    time_t t = std::chrono::system_clock::to_time_t(t_actual);
    int signature = rand() + 1;
    char fit_char = 'f';
    partition empty = {0,'P','F',-1,-1,{}};
    mbr master = {size_file,t,signature,fit_char,empty,empty,empty,empty};

    fseek(archivo, 0, SEEK_SET);
    fwrite(&master, sizeof(mbr), 1, archivo);
    mostrar(master);
    fclose(archivo);
    
}

void Comandos::crearParticion(int size, string unit, string fit, string path, string tipo, string name){

    if(size == -1 || path == "" || name == ""){
        cout<<"[MIA]@Proyecto1:~$ Cantdad de parametros incorrecto en: FDISK"<<endl;
        return;
    }
    if(unit == ""){
        unit = "k";
    }

    FILE *archivo = fopen(path.c_str(), "rb+");
    mbr master;
    if(archivo == NULL){
        cout<<"[MIA]@Proyecto1:~$ No se ha podido abrir el archivo: "<<path<<endl;
        return;
    }else{
        fseek(archivo, 0, SEEK_SET);
        fread(&master,sizeof(mbr),1,archivo);
        // mostrar(master);
        fclose(archivo);

        partition nueva;

        int size_file = getSize(size, unit);

        strcpy(nueva.part_name, name.c_str());
        nueva.type = 'P';
        nueva.part_fit = 'F';
        nueva.status = 1;
        nueva.part_s = size_file;
        if(nueva.part_s == -1){
            return;
        }

        vector<used_space> usado;
        usado.push_back({0, sizeof(mbr)});
        

        if(master.part_1.part_start != -1) usado.push_back({master.part_1.part_start, master.part_1.part_s});
        if(master.part_2.part_start != -1) usado.push_back({master.part_2.part_start, master.part_2.part_s});
        if(master.part_3.part_start != -1) usado.push_back({master.part_3.part_start, master.part_3.part_s});
        if(master.part_4.part_start != -1) usado.push_back({master.part_4.part_start, master.part_4.part_s});


        vector<free_space> libre;
  
        for(int i = 0; i < usado.size(); i++){

            used_space inicio = usado[i];

            if(i + 1 != usado.size()){
                used_space fin = usado[i];

                int ini = inicio.start + inicio.size+1;
                int end = fin.start - 1;
                int size = end - ini;
                libre.push_back({ini,size});
                cout<<"Espacio libre: "<<size<<endl;
            }else{
                int ini = inicio.start + inicio.size+1;
                int end = master.size - 1;
                int size = end - ini;
                libre.push_back({ini,size});
                cout<<"Espacio libre: "<<size<<endl;
            }
        }

        nueva.part_start = -1;
        for(int i = 0; i < libre.size(); i++){
            if(libre[i].size > size_file){
                nueva.part_start = libre[i].start;
            }
        }
        if(nueva.part_start == -1){
            cout<<"[MIA]@Proyecto1:~$ No hay espacio para crear la particion"<<endl;
            return;
        }

        archivo = fopen(path.c_str(), "wb+");

        mbr nuevo_master = master;
        nuevo_master.dsk_fit = 'x';
        

        if(nuevo_master.part_1.part_start == -1){
            nuevo_master.part_1 = nueva;
            fseek(archivo, 0, SEEK_SET);
            fwrite(&nuevo_master, sizeof(mbr), 1, archivo);


        }else if(nuevo_master.part_2.part_start == -1){
            nuevo_master.part_2 = nueva;
            fseek(archivo, 0, SEEK_SET);
            fwrite(&nuevo_master, sizeof(mbr), 1, archivo);


        }else if(nuevo_master.part_3.part_start == -1){
            nuevo_master.part_3 = nueva;
            fseek(archivo, 0, SEEK_SET);
            fwrite(&nuevo_master, sizeof(mbr), 1, archivo);


        }else if(nuevo_master.part_4.part_start == -1){
            nuevo_master.part_4 = nueva;
            fseek(archivo, 0, SEEK_SET);
            fwrite(&nuevo_master, sizeof(mbr), 1, archivo);

        }else{
            cout<<"[MIA]@Proyecto1:~$ No se puede crear otra particion"<<endl;
            return;
        }

        mostrar(nuevo_master);

        fclose(archivo);

    }

}

int Comandos::getSize(int size, string unit){
    int size_file = 0;

    if(unit == "k" || unit == "K"){
                size_file = size;
    }
    else if(unit == "m" || unit == "M"){
                size_file = size * 1024;
    }
    else if(unit == "g" || unit == "G"){
                size_file = size*1024*1024;
    }else{
        cout<<"[MIA]@Proyecto1:~$ Dimension no valida"<<endl;
        return -1;
    } 
    return size_file;
}



void Comandos::mostrar(mbr master){
    cout<<"[MIA]@Proyecto1:~$ ";
    if(master.size != 0 && master.dsk_signature != 0){
        cout<<"MBR -> ";
        cout<<"SIZE: " <<master.size<<", ";
        cout<<"TIME: " <<ctime(&master.time)<<", ";
        cout<<"SIGNATURE: " <<master.dsk_signature<<", ";
        cout<<"FIT: "  << master.dsk_fit<<endl;

        cout<<"PARTITIONS: "<<endl;
        cout<<"-- Name "<<master.part_1.part_name<<", Size: "<<master.part_1.part_s<<", Start"<< master.part_1.part_start<<endl;
        cout<<"-- Name "<<master.part_2.part_name<<", Size: "<<master.part_2.part_s<<", Start"<< master.part_2.part_start<<endl;
        cout<<"-- Name "<<master.part_3.part_name<<", Size: "<<master.part_3.part_s<<", Start"<< master.part_3.part_start<<endl;
        cout<<"-- Name "<<master.part_4.part_name<<", Size: "<<master.part_4.part_s<<", Start"<< master.part_4.part_start<<endl;
        
    }else{
        cout<<"Error al leer el archivo"<<endl;
    }
};