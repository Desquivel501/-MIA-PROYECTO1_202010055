#include "comandos.h"


void Comandos::crearDisco(int size, string unit, string fit, string path){


    if(size < 0){
        cout<<"[MIA]@Proyecto1:~$ Tamaño incorrecto en: MKDISK"<<endl;
        return;
    }

    if(path == ""){
        cout<<"[MIA]@Proyecto1:~$ No se ha definido ruta para MKDISK"<<endl;
        return;
    }

    if(unit == ""){
        unit = "k";
    }

    char fit_char = 'F';

    if(fit == "BF"){
        fit_char = 'B';
    }else if (fit == "WF"){
        fit_char = 'W';
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

    createDir(path);

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
    
    partition empty = {'0','P','F',-1,-1,"none"};
    mbr master = {size_file*1024,t,signature,fit_char,empty,empty,empty,empty};

    fseek(archivo, 0, SEEK_SET);
    fwrite(&master, sizeof(mbr), 1, archivo);
    fclose(archivo);


    int index = path.find_last_of('/');
    string nombre_d = path.substr(index + 1);
    cout<<"[MIA]@Proyecto1:~$ Disco '" + nombre_d + "' creado exitosamente "<<endl;
    
}

void Comandos::borrarDisco(string path){
    if( remove(path.c_str())!= 0 )
        cout<<"[MIA]@Proyecto1:~$ Error al eliminar el disco: "<<path<<endl;
    else{
        int index = path.find_last_of('/');
        string nombre_d = path.substr(index + 1);
        cout<<"[MIA]@Proyecto1:~$ Disco '" + nombre_d + "' eliminado exitosamente "<<endl;
    }
}


void Comandos::crearParticion(int size, string unit, string fit, string path, string tipo, string name){

    if(size < 0){
        cout<<"[MIA]@Proyecto1:~$ Tamaño incorrecto en: FDISK"<<endl;
        return;
    }

    if(path == ""){
        cout<<"[MIA]@Proyecto1:~$ No se ha definido ruta para FDISK"<<endl;
        return;
    }

    if(name == ""){
        cout<<"[MIA]@Proyecto1:~$ No se ha definido ruta para FDISK"<<endl;
        return;
    }

    if(unit == ""){
        unit = "k";
    }

    char fit_char = 'W';
    if(fit == "BestFit" || fit == "bf" || fit == "BF"){
        fit_char = 'B';
    }else if (fit == "FirstFit" || fit == "ff" || fit == "FF"){
        fit_char = 'F';
    }

    char tipo_char = 'P';
    if(tipo == "E"){
        tipo_char = 'E';
    }else if (tipo == "L"){
        tipo_char = 'L';
    }

    FILE *archivo = fopen(path.c_str(), "rb+");
    mbr master;
    if(archivo == NULL){
        cout<<"[MIA]@Proyecto1:~$ No se ha podido abrir el archivo: "<<path<<endl;
        return;
    }else{

        if(existeNombre(path, name)){
            fclose(archivo);
            cout<<"[MIA]@Proyecto1:~$ Ya existe una particion con ese nombre"<<endl;
            return;
        }
        
        rewind(archivo);
        fread(&master,sizeof(mbr),1,archivo);

        
        partition nueva;

        int size_file = getSize(size, unit);

        if (tipo_char == 'L'){
            fclose(archivo);

            if(existeExtendida(master)){
                crearParticionLogica(path, master, size_file,fit_char,name);
                mostrar(master, path);
            }else{
                cout<<"[MIA]@Proyecto1:~$ No existe ninguna particion extendida"<<endl;
            }
            return;
        }

        if (tipo_char == 'E'){
            if(existeExtendida(master)){
                cout<<"[MIA]@Proyecto1:~$ Solo puede existir una particion extendida"<<endl;
                return;
            }
            
        }

        

        vector<used_space> usado;
        usado.push_back({0, sizeof(mbr)});

        if(master.part_1.status != '0') usado.push_back({master.part_1.part_start, master.part_1.part_s, master.part_1});
        if(master.part_2.status != '0') usado.push_back({master.part_2.part_start, master.part_2.part_s, master.part_2});
        if(master.part_3.status != '0') usado.push_back({master.part_3.part_start, master.part_3.part_s, master.part_3});
        if(master.part_4.status != '0') usado.push_back({master.part_4.part_start, master.part_4.part_s, master.part_4});

        sort(usado.begin(), usado.end());

        vector<free_space> libre;
    

        for(int i = 0; i < usado.size(); i++){

            used_space inicio = usado[i];

            if((i + 1) != usado.size()){
                used_space fin = usado[i+1];

                int ini = inicio.start + inicio.size + 1;
                int end = fin.start - 1;
                int size = end - ini;
                
                if (size > 0){
                    libre.push_back({ini,size});
                }

            }else{
                int ini = inicio.start + inicio.size+1;
                int end = master.size - 1;
                int size = end - ini;
                if (size > 0){
                    libre.push_back({ini,size});
                }
            }
        }

        sort(libre.begin(), libre.end());

        strcpy(nueva.part_name, name.c_str());
        nueva.type = tipo_char;
        nueva.part_fit = fit_char;
        nueva.status = '1';
        nueva.part_s = size_file;


        nueva.part_start = -1;
        for(int i = 0; i < libre.size(); i++){
            if(libre[i].size > size_file){
                nueva.part_start = libre[i].start;
                break;
            }
        }

        if(nueva.part_start == -1){
            cout<<"[MIA]@Proyecto1:~$ No hay espacio para crear la particion"<<endl;
            return;
            }

        mbr nuevo_master = master; 

        if(nuevo_master.part_1.status == '0') nuevo_master.part_1 = nueva;
        else if(nuevo_master.part_2.status == '0') nuevo_master.part_2 = nueva;
        else if(nuevo_master.part_3.status == '0') nuevo_master.part_3 = nueva;
        else if(nuevo_master.part_4.status == '0') nuevo_master.part_4 = nueva;
        else{
            cout<<"[MIA]@Proyecto1:~$ No se puede crear otra particion"<<endl;
            return;
        }

        rewind(archivo);
        fwrite(&nuevo_master, sizeof(mbr), 1, archivo);

        if(tipo_char == 'E'){
            ebr inicio;
            inicio.part_status = '0';
            inicio.part_next = -1;
            strcpy(inicio.part_name, "none");

            fseek(archivo, nueva.part_start, SEEK_SET);
            fwrite(&inicio, sizeof(ebr), 1, archivo);
        }
        fclose(archivo);
        mostrar(master, path);
        cout<<"[MIA]@Proyecto1:~$ Particion '" + name + "' creada exitosamente "<<endl;
    }

}


void Comandos::crearParticionLogica(string path, mbr master, int size, char fit, string name){
    FILE *archivo = fopen(path.c_str(), "rb+");
    partition partExtendida = getPartExtendida(master);
    int inicio_part = partExtendida.part_start;
    int fin_part = inicio_part + partExtendida.part_s - 1;

    fseek(archivo, inicio_part, SEEK_SET);

    ebr ebr_it;
    fread(&ebr_it, sizeof(ebr), 1, archivo);

    int inicio_libre = inicio_part;
    int limite_libre = fin_part;

    bool puedeCrear = false;
    bool primero = true;

    while(ebr_it.part_next != -1){

        if(ebr_it.part_status == '1'){
            inicio_libre = ebr_it.part_start + ebr_it.part_s;
        }

        limite_libre = ebr_it.part_next - 1;

        fseek(archivo, ebr_it.part_next, SEEK_SET);
        fread(&ebr_it, sizeof(ebr), 1, archivo);

        if( (limite_libre - inicio_libre + 1 >= size) && (size > (int)sizeof(ebr) ) ){
            puedeCrear = true;
            break;
        }
        primero = false;
    }

    if(!puedeCrear){
        if(ebr_it.part_status == '0'){
            inicio_libre = inicio_part;
        }else{
            inicio_libre = ebr_it.part_start + ebr_it.part_s;
            primero = false;
        }
        limite_libre = fin_part;

        if( (limite_libre - inicio_libre + 1 >= size) && (size > (int)sizeof(ebr) ) ){
            puedeCrear = true;
        }
    }

    if(puedeCrear){
        ebr nuevo_ebr;
        nuevo_ebr.part_status = '1';
        nuevo_ebr.part_fit = fit;
        strcpy(nuevo_ebr.part_name, name.c_str());
        nuevo_ebr.part_next = -1;
        nuevo_ebr.part_start = inicio_libre + (int)sizeof(ebr);
        nuevo_ebr.part_s = size - sizeof(ebr);

        if(primero){
            fseek(archivo, inicio_part, SEEK_SET);
            fread(&ebr_it, sizeof(ebr), 1, archivo);

            if(ebr_it.part_status == '0'){
                nuevo_ebr.part_next = ebr_it.part_next;
            }else{
                nuevo_ebr.part_next = ebr_it.part_next;
                ebr_it.part_next = inicio_libre;
                fseek(archivo, inicio_part, SEEK_SET);
                fwrite(&ebr_it, sizeof(ebr), 1, archivo);
            }
        }else{
            nuevo_ebr.part_next = ebr_it.part_next;
            ebr_it.part_next = nuevo_ebr.part_start - (int)sizeof(ebr);

            int start = ebr_it.part_start - (int)sizeof(ebr);

            fseek(archivo, start , SEEK_SET);
            fwrite(&ebr_it, sizeof(ebr), 1, archivo);
        }

        fseek(archivo, inicio_libre, SEEK_SET);
        fwrite(&nuevo_ebr, sizeof(ebr), 1, archivo);
        cout<<"[MIA]@Proyecto1:~$ Particion '" + name + "' creada exitosamente "<<endl;

    }else{
        cout<<"[MIA]@Proyecto1:~$ No se pudo crear la particion"<<endl;
    }
    fclose(archivo);
}


int Comandos::getSize(int size, string unit){
    int size_file = 0;

    if(unit == "k" || unit == "K"){
                size_file = size*1024;
    }
    else if(unit == "m" || unit == "M"){
                size_file = size * 1024*1024; 
    }
    else if(unit == "g" || unit == "G"){
                size_file = size*1024*1024*1024;
    }else{
        cout<<"[MIA]@Proyecto1:~$ Dimension no valida"<<endl;
        return -1;
    } 
    return size_file;
}


void Comandos::mostrar(mbr master, string path){
    cout<<"[MIA]@Proyecto1:~$ ";
    if(master.size != 0 && master.dsk_signature != 0){
        cout<<"MBR -> ";
        cout<<"SIZE: " <<master.size<<", ";
        cout<<"TIME: " <<ctime(&master.time)<<", ";
        cout<<"SIGNATURE: " <<master.dsk_signature<<", ";
        cout<<"FIT: "  << master.dsk_fit<<endl;

        cout<<"PARTITIONS: "<<endl;
        cout<<"-- Name "<<master.part_1.part_name<<", Size: "<<master.part_1.part_s<<", Start: "<< master.part_1.part_start<<endl;
        if(master.part_1.type == 'E') mostrarExtendida(master.part_1, path);
        cout<<"-- Name "<<master.part_2.part_name<<", Size: "<<master.part_2.part_s<<", Start: "<< master.part_2.part_start<<endl;
        if(master.part_2.type == 'E') mostrarExtendida(master.part_2, path);
        cout<<"-- Name "<<master.part_3.part_name<<", Size: "<<master.part_3.part_s<<", Start: "<< master.part_3.part_start<<endl;
        if(master.part_3.type == 'E') mostrarExtendida(master.part_3, path);
        cout<<"-- Name "<<master.part_4.part_name<<", Size: "<<master.part_4.part_s<<", Start: "<< master.part_4.part_start<<endl;
        if(master.part_4.type == 'E') mostrarExtendida(master.part_4, path);
        
    }else{
        cout<<"Error al leer el archivo"<<endl;
    }
}


void Comandos::mostrarExtendida(partition extendida, string path){
    FILE *archivo = fopen(path.c_str(), "rb+");

    fseek(archivo, extendida.part_start, SEEK_SET);

    ebr ebr_it;
    fread(&ebr_it, sizeof(ebr), 1, archivo);

    cout<<"---- Name "<<ebr_it.part_name<<", Size: "<<ebr_it.part_s<<", Start: "<< ebr_it.part_start<<endl;

    while(ebr_it.part_next != -1){
        fseek(archivo, ebr_it.part_next, SEEK_SET);
        fread(&ebr_it, sizeof(ebr), 1, archivo);

        if(ebr_it.part_status == '1'){
            cout<<"---- Name "<<ebr_it.part_name<<", Size: "<<ebr_it.part_s<<", Start: "<< ebr_it.part_start<<endl;
        }
    }

    fclose(archivo);

}


bool Comandos::existeExtendida(mbr master){
    if(master.part_1.type == 'E') return true;
    if(master.part_2.type == 'E') return true;
    if(master.part_3.type == 'E') return true;
    if(master.part_4.type == 'E') return true;
    return false;
}


void Comandos::mount(string path, string nombre_s, string nombre_d){

    FILE *archivo = fopen(path.c_str(), "rb+");
    mbr master;

    string num = to_string(Comandos::id_mount);

    if(archivo == NULL){
        cout<<"[MIA]@Proyecto1:~$ No se ha podido abrir el archivo: "<<path<<endl;
        return; 
    }else{

        rewind(archivo);
        fread(&master,sizeof(mbr),1,archivo);

        string part_name = master.part_1.part_name;
        if(part_name == nombre_s){
            string id = "55" + num + nombre_d;
            Comandos::id_mount++;
            montado mount = {id, master.part_1, master, path};
            mounted.push_back(mount);
            fclose(archivo);
            cout<<"[MIA]@Proyecto1:~$ Particion '" + nombre_s + "' montada con id: " <<id<<endl;
            return;
        }
        part_name = master.part_2.part_name;
        if(part_name == nombre_s){
            string id = "55" + num + nombre_d;
            Comandos::id_mount++;
            montado mount = {id, master.part_2, master, path};
            mounted.push_back(mount);
            fclose(archivo);
            cout<<"[MIA]@Proyecto1:~$ Particion '" + nombre_s + "' montada con id: " <<id<<endl;
            return;
        }
        part_name = master.part_3.part_name;
        if(part_name == nombre_s){
            string id = "55" + num + nombre_d;
            Comandos::id_mount++;
            montado mount = {id, master.part_3, master, path};
            mounted.push_back(mount);
            fclose(archivo);
            cout<<"[MIA]@Proyecto1:~$ Particion '" + nombre_s + "' montada con id: " <<id<<endl;
            return;
        }
        part_name = master.part_4.part_name;
        if(part_name == nombre_s){
            string id = "55" + num + nombre_d;
            Comandos::id_mount++;
            montado mount = {id, master.part_4, master, path};
            mounted.push_back(mount);
            fclose(archivo);
            cout<<"[MIA]@Proyecto1:~$ Particion '" + nombre_s + "' montada con id: " <<id<<endl;
            return;
        }

        if (existeExtendida(master)){
            partition partExtendida = getPartExtendida(master);                
            fseek(archivo, partExtendida.part_start, SEEK_SET);

            ebr ebr_it;
            fread(&ebr_it, sizeof(ebr), 1, archivo);
                
            part_name = ebr_it.part_name;
            if( part_name == nombre_s){
                partition partL = {1, 'L', ebr_it.part_fit, ebr_it.part_start, ebr_it.part_s, {}};
                strcpy(partL.part_name, part_name.c_str());
                string id = "55" + num + nombre_d;
                Comandos::id_mount++;
                montado mount = {id, partL, master, path};
                mounted.push_back(mount);
                fclose(archivo);
                cout<<"[MIA]@Proyecto1:~$ Particion '" + nombre_s + "' montada con id: " <<id<<endl;
                return;
            }

            while(ebr_it.part_next != -1){
                fseek(archivo, ebr_it.part_next, SEEK_SET);
                fread(&ebr_it, sizeof(ebr), 1, archivo);

                if(ebr_it.part_status == '1'){
                    part_name = ebr_it.part_name;
                    if( part_name == nombre_s){
                        partition partL = {1, 'L', ebr_it.part_fit, ebr_it.part_start, ebr_it.part_s, {}};
                        strcpy(partL.part_name, part_name.c_str());
                        string id = "55" + num + nombre_d;
                        Comandos::id_mount++;
                        montado mount = {id, partL, master, path};
                        mounted.push_back(mount);
                        fclose(archivo);
                        cout<<"[MIA]@Proyecto1:~$ Particion '" + nombre_s + "' montada con id: " <<id<<endl;
                        return;
                    }
                }
            }

        }
        cout<<"[MIA]@Proyecto1:~$ No se podido montar la particion"<<endl;
    }
}

void Comandos::reporteMBR(string path, string id){
    for (montado m : Comandos::mounted){
        if(m.id == id){
            createDir(path);
            ofstream dot;
            dot.open("auxiliar.dot");
            string cmd = "dot -Tjpg auxiliar.dot -o '" + path + "'";

            dot << "digraph G {\n"; 
            dot << "fontname=\"Helvetica,Arial,sans-serif\"\n"; 
            dot << "node [fontname=\"Helvetica,Arial,sans-serif\"]\n"; 
            dot << "rankdir=TB;\n"; 
            dot << "node [shape=record];\n"; 
            dot << "a[label = <<table border=\"0\" cellborder=\"1\" cellspacing=\"0\" cellpadding=\"4\">\n"; 
            dot << "            <tr> <td bgcolor=\"#800080\">  <font color=\"white\"> <b>REPORTE MBR</b> </font> </td> <td bgcolor=\"#800080\"></td> </tr>\n"; 
            dot << "            <tr> <td> <b>mbr_tamano</b> </td> <td> <b>" + to_string(m.master.size) + "</b> </td> </tr>\n"; 
            string time = ctime(&m.master.time);
            dot << "            <tr> <td bgcolor=\"#f2e5f2\"> <b>mbr_fecha_creacion</b> </td> <td> <b>" + time + "</b> </td> </tr>\n"; 
            dot << "            <tr> <td> <b>mbr_disk_signature</b> </td> <td> <b>" + to_string(m.master.dsk_signature) + "</b> </td> </tr>\n"; 


            dot << "            <tr> <td bgcolor=\"#800080\">  <font color=\"white\"> <b>PARTICION</b> </font> </td> <td bgcolor=\"#800080\"></td> </tr>\n"; 
            dot << "            <tr> <td> <b>part_status</b> </td> <td> <b>" + string(1, m.master.part_1.status) + "</b> </td> </tr>\n"; 
            dot << "            <tr> <td bgcolor=\"#f2e5f2\"> <b>part_type</b> </td> <td bgcolor=\"#f2e5f2\"> <b>" + string(1, m.master.part_1.type) + "</b> </td> </tr>\n"; 
            dot << "            <tr> <td> <b>part_fit</b> </td> <td> <b>" + string(1, m.master.part_1.part_fit) + "</b> </td> </tr>\n"; 
            dot << "            <tr> <td bgcolor=\"#f2e5f2\"> <b>part_start</b> </td> <td bgcolor=\"#f2e5f2\"> <b>" + to_string(m.master.part_1.part_start) + "</b> </td> </tr>\n"; 
            dot << "            <tr> <td> <b>part_size</b> </td> <td> <b>" + to_string(m.master.part_1.part_s) + "</b> </td> </tr>\n";
            string name_1 = m.master.part_1.part_name;
            dot << "            <tr> <td bgcolor=\"#f2e5f2\"> <b>part_name</b> </td> <td bgcolor=\"#f2e5f2\"> <b>" + name_1 + "</b> </td> </tr>\n"; 
            if(m.master.part_1.type == 'E') dot << reporteEBR(m.path, m.master);


            dot << "            <tr> <td bgcolor=\"#800080\">  <font color=\"white\"> <b>PARTICION</b> </font> </td> <td bgcolor=\"#800080\"></td> </tr>\n"; 
            dot << "            <tr> <td> <b>part_status</b> </td> <td> <b>" + string(1, m.master.part_2.status) + "</b> </td> </tr>\n"; 
            dot << "            <tr> <td bgcolor=\"#f2e5f2\"> <b>part_type</b> </td> <td bgcolor=\"#f2e5f2\"> <b>" + string(1, m.master.part_2.type) + "</b> </td> </tr>\n"; 
            dot << "            <tr> <td> <b>part_fit</b> </td> <td> <b>" + string(1, m.master.part_2.part_fit) + "</b> </td> </tr>\n"; 
            dot << "            <tr> <td bgcolor=\"#f2e5f2\"> <b>part_start</b> </td> <td bgcolor=\"#f2e5f2\"> <b>" + to_string(m.master.part_2.part_start) + "</b> </td> </tr>\n"; 
            dot << "            <tr> <td> <b>part_size</b> </td> <td> <b>" + to_string(m.master.part_2.part_s) + "</b> </td> </tr>\n";
             name_1 = m.master.part_2.part_name;
            dot << "            <tr> <td bgcolor=\"#f2e5f2\"> <b>part_name</b> </td> <td bgcolor=\"#f2e5f2\"> <b>" + name_1 + "</b> </td> </tr>\n"; 
            if(m.master.part_2.type == 'E') dot << reporteEBR(m.path, m.master);


            dot << "            <tr> <td bgcolor=\"#800080\">  <font color=\"white\"> <b>PARTICION</b> </font> </td> <td bgcolor=\"#800080\"></td> </tr>\n"; 
            dot << "            <tr> <td> <b>part_status</b> </td> <td> <b>" + string(1, m.master.part_3.status) + "</b> </td> </tr>\n"; 
            dot << "            <tr> <td bgcolor=\"#f2e5f2\"> <b>part_type</b> </td> <td bgcolor=\"#f2e5f2\"> <b>" + string(1, m.master.part_3.type) + "</b> </td> </tr>\n"; 
            dot << "            <tr> <td> <b>part_fit</b> </td> <td> <b>" + string(1, m.master.part_3.part_fit) + "</b> </td> </tr>\n"; 
            dot << "            <tr> <td bgcolor=\"#f2e5f2\"> <b>part_start</b> </td> <td bgcolor=\"#f2e5f2\"> <b>" + to_string(m.master.part_3.part_start) + "</b> </td> </tr>\n"; 
            dot << "            <tr> <td> <b>part_size</b> </td> <td> <b>" + to_string(m.master.part_3.part_s) + "</b> </td> </tr>\n";
             name_1 = m.master.part_3.part_name;
            dot << "            <tr> <td bgcolor=\"#f2e5f2\"> <b>part_name</b> </td> <td bgcolor=\"#f2e5f2\"> <b>" + name_1 + "</b> </td> </tr>\n"; 
            if(m.master.part_3.type == 'E') dot << reporteEBR(m.path, m.master);

            dot << "            <tr> <td bgcolor=\"#800080\">  <font color=\"white\"> <b>PARTICION</b> </font> </td> <td bgcolor=\"#800080\"></td> </tr>\n"; 
            dot << "            <tr> <td> <b>part_status</b> </td> <td> <b>" + string(1, m.master.part_4.status) + "</b> </td> </tr>\n"; 
            dot << "            <tr> <td bgcolor=\"#f2e5f2\"> <b>part_type</b> </td> <td bgcolor=\"#f2e5f2\"> <b>" + string(1, m.master.part_4.type) + "</b> </td> </tr>\n"; 
            dot << "            <tr> <td> <b>part_fit</b> </td> <td> <b>" + string(1, m.master.part_4.part_fit) + "</b> </td> </tr>\n"; 
            dot << "            <tr> <td bgcolor=\"#f2e5f2\"> <b>part_start</b> </td> <td bgcolor=\"#f2e5f2\"> <b>" + to_string(m.master.part_4.part_start) + "</b> </td> </tr>\n"; 
            dot << "            <tr> <td> <b>part_size</b> </td> <td> <b>" + to_string(m.master.part_4.part_s) + "</b> </td> </tr>\n";
             name_1 = m.master.part_4.part_name;
            dot << "            <tr> <td bgcolor=\"#f2e5f2\"> <b>part_name</b> </td> <td bgcolor=\"#f2e5f2\"> <b>" + name_1 + "</b> </td> </tr>\n"; 
            if(m.master.part_4.type == 'E') dot << reporteEBR(m.path, m.master);

            dot << "        </table>>\n"; 
            dot << "]\n"; 
            dot << "}\n"; 
            dot.close();
            system(cmd.c_str());
            return;
        }
    }
}

string Comandos::reporteEBR(string path, mbr master){
    string dot = "";
    FILE *archivo = fopen(path.c_str(), "rb+");
    partition partExtendida = getPartExtendida(master);                
    fseek(archivo, partExtendida.part_start, SEEK_SET);

    ebr ebr_it;
    fread(&ebr_it, sizeof(ebr), 1, archivo);

    dot += "            <tr> <td bgcolor=\"#ffa500\">  <font color=\"white\"> <b>PARTICION LOGICA</b> </font> </td> <td bgcolor=\"#ffa500\"></td> </tr>\n"; 
    dot += "            <tr> <td> <b>part_status</b> </td> <td> <b>" + string(1, ebr_it.part_status) + "</b> </td> </tr>\n"; 
    dot += "            <tr> <td bgcolor=\"#fff6e5\"> <b>part_next</b> </td> <td bgcolor=\"#fff6e5\"> <b>" + to_string(ebr_it.part_next) + "</b> </td> </tr>\n"; 
    dot += "            <tr> <td> <b>part_fit</b> </td> <td> <b>" + string(1, ebr_it.part_fit) + "</b> </td> </tr>\n"; 
    dot += "            <tr> <td bgcolor=\"#fff6e5\"> <b>part_start</b> </td> <td bgcolor=\"#fff6e5\"> <b>" + to_string(ebr_it.part_start) + "</b> </td> </tr>\n"; 
    dot += "            <tr> <td> <b>part_size</b> </td> <td> <b>" + to_string(ebr_it.part_s) + "</b> </td> </tr>\n"; 
    string name = ebr_it.part_name;
    dot += "            <tr> <td bgcolor=\"#fff6e5\"> <b>part_name</b> </td> <td bgcolor=\"#fff6e5\"> <b>" + name + "</b> </td> </tr>\n"; 


    while(ebr_it.part_next != -1){
        fseek(archivo, ebr_it.part_next, SEEK_SET);
        fread(&ebr_it, sizeof(ebr), 1, archivo);

        dot += "            <tr> <td bgcolor=\"#ffa500\">  <font color=\"white\"> <b>PARTICION LOGICA</b> </font> </td> <td bgcolor=\"#ffa500\"></td> </tr>\n"; 
        dot += "            <tr> <td> <b>part_status</b> </td> <td> <b>" + string(1, ebr_it.part_status) + "</b> </td> </tr>\n"; 
        dot += "            <tr> <td bgcolor=\"#fff6e5\"> <b>part_next</b> </td> <td bgcolor=\"#fff6e5\"> <b>" + to_string(ebr_it.part_next) + "</b> </td> </tr>\n"; 
        dot += "            <tr> <td> <b>part_fit</b> </td> <td> <b>" + string(1, ebr_it.part_fit) + "</b> </td> </tr>\n"; 
        dot += "            <tr> <td bgcolor=\"#fff6e5\"> <b>part_start</b> </td> <td bgcolor=\"#fff6e5\"> <b>" + to_string(ebr_it.part_start) + "</b> </td> </tr>\n"; 
        dot += "            <tr> <td> <b>part_size</b> </td> <td> <b>" + to_string(ebr_it.part_s) + "</b> </td> </tr>\n"; 
        string name = ebr_it.part_name;
        dot += "            <tr> <td bgcolor=\"#fff6e5\"> <b>part_name</b> </td> <td bgcolor=\"#fff6e5\"> <b>" + name + "</b> </td> </tr>\n"; 
    }

    fclose(archivo);
    return dot;
}

void Comandos::repDisco(string path, string id){
    for (montado m : Comandos::mounted){
        if(m.id == id){
            createDir(path);
            FILE *archivo = fopen(m.path.c_str(), "rb+");
            mbr master;
            if(archivo == NULL){
                cout<<"[MIA]@Proyecto1:~$ No se ha podido abrir el archivo: "<<path<<endl;
                return;
            }else{
                fseek(archivo, 0, SEEK_SET);
                fread(&master,sizeof(mbr),1,archivo);
            }

            int index = m.path.find_last_of('/');
            string nombre_d = m.path.substr(index + 1);

            int size = master.size;

            ofstream dot;
            dot.open("auxiliar_disk.dot");
            string cmd = "dot -Tjpg auxiliar_disk.dot -o '" + path + "'";

            dot << "digraph G {\n"; 
            dot << "fontname=\"Helvetica,Arial,sans-serif\"\n"; 
            dot << "node [fontname=\"Helvetica,Arial,sans-serif\"]\n"; 
            dot << "rankdir=TB;\n"; 
            dot << " node [shape=record];\n"; 
            dot << "label=\"" + nombre_d + "\"\n"; 

            dot << "a[label = \" "; 

            dot << "mbr"; 

            int size_disco = master.size;

            vector<used_space> usado;
            usado.push_back({0, sizeof(mbr)});

            if(master.part_1.status != '0') usado.push_back({master.part_1.part_start, master.part_1.part_s, master.part_1});
            if(master.part_2.status != '0') usado.push_back({master.part_2.part_start, master.part_2.part_s, master.part_2});
            if(master.part_3.status != '0') usado.push_back({master.part_3.part_start, master.part_3.part_s, master.part_3});
            if(master.part_4.status != '0') usado.push_back({master.part_4.part_start, master.part_4.part_s, master.part_4});

            for(int i = 0; i < usado.size(); i++){
                used_space inicio = usado[i];

                if((i + 1) != usado.size()){
                    used_space fin = usado[i+1];

                    int ini = inicio.start + inicio.size + 1;
                    int end = fin.start - 1;
                    int size = end - ini;
                    
                    if (size > 0){
                        
                        float ocupado = ( static_cast<float>(size) / size_disco) * 100;
                        std::stringstream ss;
                        ss << std::fixed << std::setprecision(2) << ocupado;
                        dot << "| Libre \\n" + ss.str() +" %"; 
                    }

                    if(fin.part.type == 'E'){
                        
                        float ocupado_ = (static_cast<float>(fin.size) / size_disco) *100;
                        std::stringstream ss;
                        ss << std::fixed << std::setprecision(2) << ocupado_;
                        dot << "| {Extendida | {";
                        dot << repDiscoEx(m.path,master,size_disco);
                        dot << "}}"; 
                    }else{
                        float ocupado_ = (static_cast<float>(fin.size) / size_disco) *100;
                        std::stringstream ss;
                        ss << std::fixed << std::setprecision(2) << ocupado_;
                        dot << "| Primaria \\n" + ss.str() +" %"; 
                    }

                }else{
                    int ini = inicio.start + inicio.size+1;
                    int end = master.size - 1;
                    int size = end - ini;
                    if (size > 0){
                        float ocupado = ( static_cast<float>(size) / size_disco) * 100;
                        std::stringstream ss;
                        ss << std::fixed << std::setprecision(2) << ocupado;
                        dot << "| Libre \\n" + ss.str() +" %"; 
                    }
                }

            }
            

            dot << "\"]"; 
            dot << "}\n"; 
            dot.close();
            system(cmd.c_str());
            return;
        }
    }
}

string Comandos::repDiscoEx(string path, mbr master, int size_total){

    string dot = "";

    FILE *archivo = fopen(path.c_str(), "rb+");
    partition partExtendida = getPartExtendida(master);
    int inicio_part = partExtendida.part_start;
    int fin_part = inicio_part + partExtendida.part_s - 1;

    fseek(archivo, inicio_part, SEEK_SET);

    ebr ebr_it;
    fread(&ebr_it, sizeof(ebr), 1, archivo);

    int inicio_libre = inicio_part;
    int limite_libre = fin_part;

    bool puedeCrear = false;
    bool primero = true;

    if(ebr_it.part_status == '0'){
        if(ebr_it.part_next == -1){
            float ocupado = ( static_cast<float>(partExtendida.part_s) / size_total) * 100;
            std::stringstream ss;
            ss << std::fixed << std::setprecision(2) << ocupado;
            dot += "Libre \\n" + ss.str() +" %"; 
            return dot;
        }else{
            float ocupado = ( static_cast<float>(ebr_it.part_s) / size_total) * 100;
            std::stringstream ss;
            ss << std::fixed << std::setprecision(2) << ocupado;
            dot += "Libre \\n" + ss.str() +" %"; 
        }
        
    }else{
        float ocupado = ( static_cast<float>(ebr_it.part_s) / size_total) * 100;
        std::stringstream ss;
        ss << std::fixed << std::setprecision(2) << ocupado;
        dot += "Logica \\n" + ss.str() +" %"; 
    }


    while(ebr_it.part_next != -1){

        if(ebr_it.part_status == '1'){
            inicio_libre = ebr_it.part_start + ebr_it.part_s;
        }

        limite_libre = ebr_it.part_next - 1;

        if( (limite_libre - inicio_libre) > 0){
            float ocupado = ( static_cast<float>(limite_libre - inicio_libre) / size_total) * 100;
            std::stringstream ss;
            ss << std::fixed << std::setprecision(2) << ocupado;
            dot += "| Libre \\n" + ss.str() +" %"; 
        }

        fseek(archivo, ebr_it.part_next, SEEK_SET);
        fread(&ebr_it, sizeof(ebr), 1, archivo);

        float ocupado = ( static_cast<float>(ebr_it.part_s) / size_total) * 100;
        std::stringstream ss;
        ss << std::fixed << std::setprecision(2) << ocupado;
        dot += "| Logica \\n" + ss.str() +" %"; 
    }

    if((fin_part - (ebr_it.part_start + ebr_it.part_s)) > 0){
        float ocupado = ( static_cast<float>((fin_part - (ebr_it.part_start + ebr_it.part_s))) / size_total) * 100;
        std::stringstream ss;
        ss << std::fixed << std::setprecision(2) << ocupado;
        dot += "| Libre \\n" + ss.str() +" %"; 
    }

    fclose(archivo);
    return dot;
}


void Comandos::createDir(string path){
    int index = path.find_last_of('/');
    string nombre_dir = path.substr(0, index);
    string cmd = "mkdir -p '" + nombre_dir +"'";
    system(cmd.c_str());
}


bool Comandos::existeNombre(string path, string nombre_s){

    FILE *archivo = fopen(path.c_str(), "rb+");
    mbr master;

    string num = to_string(Comandos::id_mount);

    if(archivo == NULL){
        cout<<"[MIA]@Proyecto1:~$ No se ha podido abrir el archivo: "<<path<<endl;
        return true; 
    }else{

        rewind(archivo);
        fread(&master,sizeof(mbr),1,archivo);

        string part_name = master.part_1.part_name;
        if(part_name == nombre_s){
            return true;
        }
        part_name = master.part_2.part_name;
        if(part_name == nombre_s){
            return true;
        }
        part_name = master.part_3.part_name;
        if(part_name == nombre_s){
            return true;
        }
        part_name = master.part_4.part_name;
        if(part_name == nombre_s){
            return true;
        }

        if (existeExtendida(master)){
            partition partExtendida = getPartExtendida(master);                
            fseek(archivo, partExtendida.part_start, SEEK_SET);

            ebr ebr_it;
            fread(&ebr_it, sizeof(ebr), 1, archivo);
                
            part_name = ebr_it.part_name;
            if( part_name == nombre_s){
                return true;
            }

            while(ebr_it.part_next != -1){
                fseek(archivo, ebr_it.part_next, SEEK_SET);
                fread(&ebr_it, sizeof(ebr), 1, archivo);

                if(ebr_it.part_status == '1'){
                    part_name = ebr_it.part_name;
                    if( part_name == nombre_s){
                        return true;
                    }
                }
            }

        }
    }
    return false;
}


void Comandos::borrarParticion(string path, string nombre_s){

    FILE *archivo = fopen(path.c_str(), "rb+");
    mbr master;

    string num = to_string(Comandos::id_mount);

    if(archivo == NULL){
        cout<<"[MIA]@Proyecto1:~$ No se ha podido abrir el archivo: "<<path<<endl;
        return; 
    }else{

        rewind(archivo);
        fread(&master,sizeof(mbr),1,archivo);

        string part_name = master.part_1.part_name;
        if(part_name == nombre_s && master.part_1.status != '0'){
            master.part_1.status = '0';
            rewind(archivo);
            fwrite(&master, sizeof(mbr), 1, archivo);
            cout<<"[MIA]@Proyecto1:~$ Particion '" + nombre_s + "' ha sido eliminada"<<endl;
            actualizarMount(path,part_name,master);
            fclose(archivo);
            return ;
        }
        part_name = master.part_2.part_name;
        if(part_name == nombre_s && master.part_2.status != '0'){
            master.part_2.status = '0';
            rewind(archivo);
            fwrite(&master, sizeof(mbr), 1, archivo);
            cout<<"[MIA]@Proyecto1:~$ Particion '" + nombre_s + "' ha sido eliminada"<<endl;
            actualizarMount(path,part_name,master);
            fclose(archivo);
            return ;
        }
        part_name = master.part_3.part_name;
        if(part_name == nombre_s && master.part_3.status != '0'){
            master.part_3.status = '0';
            rewind(archivo);
            fwrite(&master, sizeof(mbr), 1, archivo);
            cout<<"[MIA]@Proyecto1:~$ Particion '" + nombre_s + "' ha sido eliminada"<<endl;
            actualizarMount(path,part_name,master);
            fclose(archivo);
            return ;
        }
        part_name = master.part_4.part_name;
        if(part_name == nombre_s && master.part_4.status != '0'){
            master.part_4.status = '0';
            rewind(archivo);
            fwrite(&master, sizeof(mbr), 1, archivo);
            cout<<"[MIA]@Proyecto1:~$ Particion '" + nombre_s + "' ha sido eliminada"<<endl;
            actualizarMount(path,part_name,master);
            fclose(archivo);
            return ;
        }

        if (existeExtendida(master)){
            partition partExtendida = getPartExtendida(master);                
            fseek(archivo, partExtendida.part_start, SEEK_SET);

            ebr ebr_it;
            fread(&ebr_it, sizeof(ebr), 1, archivo);
                
            part_name = ebr_it.part_name;
            if( part_name == nombre_s && ebr_it.part_status != '0'){
                ebr_it.part_status = '0';
                fseek(archivo, partExtendida.part_start, SEEK_SET);
                fwrite(&ebr_it, sizeof(ebr), 1, archivo);
                cout<<"[MIA]@Proyecto1:~$ Particion '" + nombre_s + "' ha sido eliminada"<<endl;
                actualizarMount(path,part_name,master);
                fclose(archivo);
                return ;
            }

            while(ebr_it.part_next != -1){
                fseek(archivo, ebr_it.part_next, SEEK_SET);
                fread(&ebr_it, sizeof(ebr), 1, archivo);
                part_name = ebr_it.part_name;
                if(ebr_it.part_status == '1' && part_name == nombre_s ){
                    part_name = ebr_it.part_name;
                    if( part_name == nombre_s){
                        ebr_it.part_status = '0';
                        fseek(archivo, ebr_it.part_next - sizeof(ebr), SEEK_SET);
                        fwrite(&ebr_it, sizeof(ebr), 1, archivo);
                        cout<<"[MIA]@Proyecto1:~$ Particion '" + nombre_s + "' ha sido eliminada"<<endl;
                        actualizarMount(path,part_name,master);
                        fclose(archivo);
                        return ;
                    }
                }
            }

        }
    }
    fclose(archivo);
}


void Comandos::actualizarMount(string path, string nombre, mbr master){
    for (montado& m : Comandos::mounted){
        cout<< m.path << " -- " <<path <<endl;
        if(m.path == path){
            string part_name = m.particion.part_name;
            if(part_name == nombre){
                unmount(nombre);
            }else{
                cout<<"here"<<endl;
                m.master = master;
            }

        }
    }

 

}