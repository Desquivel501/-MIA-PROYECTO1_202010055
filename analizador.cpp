#include "analizador.h"


vector<string> Analizador::split_txt(string text){
    
    string segment;
    string path, cont, name, destino= "";
    vector<string> splited;
    int first, last;

    first = text.find("-path->\"");
    if(first != std::string::npos){
        last = text.find("\"", first+8);
        path = text.substr(first,last-first);
        text = replace_txt(text, path, "");
    }

    first = text.find("-cont->\"");
    if(first != std::string::npos){
        last = text.find("\"", first+8);
        cont = text.substr(first,last-first);
        text = replace_txt(text, cont, "");
    }

    first = text.find("-name->\"");
    if(first != std::string::npos){
        last = text.find("\"", first+8);
        name = text.substr(first,last-first);
        text = replace_txt(text, name, "");
    }

    first = text.find("-destino->\"");
    if(first != std::string::npos){
        last = text.find("\"", first+11);
        destino = text.substr(first,last-first);
        text = replace_txt(text, destino, "");
    }

    stringstream text_to_split(text);

    while(std::getline(text_to_split, segment, ' ')){
       splited.push_back(segment);
    }

    if(path != "") splited.push_back(path);
    if(cont != "") splited.push_back(cont);
    if(name != "") splited.push_back(name);
    if(destino != "") splited.push_back(destino);

    return splited;
}

string Analizador::replace_txt(string str, const string& from, const string& to) {
    size_t index = 0;
    while (true) {
        index = str.find(from, index);
        if (index == std::string::npos) break;
        str.replace(index, from.length(), to);
        index += to.length();
    }
    return str;
}


void Analizador::analizar_comandos(string path){
    ifstream file;
    file.open(path);
    string line;

    while(std::getline(file, line))
    {
        if(line.length() > 0){
            if(line.at(0) != '#') {
                analizar(line);
            }
        }
    }
}

void Analizador::analizar(string entrada){
    // transform(entrada.begin(),entrada.end(),entrada.begin(), ::tolower);
    vector<string> cmd_list = split_txt(entrada);
    vector<string> parametros;
    string comando;

    for(int i = 0; i < cmd_list.size(); i++){
        if(i == 0){
            comando = cmd_list.at(i);;
        }else{
            parametros.push_back(cmd_list.at(i));
        }
    }
    identificar(comando, parametros);
}

void Analizador::identificar(string comando, vector<string> parametros){
    string param = "";

    if(comando == "exec"){
        int size = 0;
        string path;

        for(int i = 0; i < parametros.size(); i++){
            param = parametros.at(i);;

            if(param.find("-path->") == 0){
                param = replace_txt(param, "-path->", "");
                param = replace_txt(param, "\"", "");
                cout<<"Ruta: "<<param<<endl;
                path = param;
            }

        }
        analizar_comandos(path);
    }

    else if(comando == "mkdisk"){
        // cout<<"Comando -> mkdisk"<<endl;

        int size = -1;
        string fit, unit, path;

        for(int i = 0; i < parametros.size(); i++){
            param = parametros.at(i);;

            if(param.find("-s->") == 0){
                param = replace_txt(param, "-s->", "");
                param = replace_txt(param, "\"", "");
                // cout<<"Size: "<<param<<endl;
                size =  stoi(param);
            }

            if(param.find("-f->") == 0){
                param = replace_txt(param, "-f->", "");
                param = replace_txt(param, "\"", "");
                // cout<<"Fit: "<<param<<endl;
                fit = param;
            }

            if(param.find("-u->") == 0){
                param = replace_txt(param, "-u->", "");
                param = replace_txt(param, "\"", "");
                // cout<<"Unidad: "<<param<<endl;
                unit = param;
            }

            if(param.find("-path->") == 0){
                param = replace_txt(param, "-path->", "");
                param = replace_txt(param, "\"", "");
                // cout<<"Ruta: "<<param<<endl;
                path = param;
            }
        }
        cmd.crearDisco(size,unit,fit,path);
    }

    else if(comando == "fdisk"){
        // cout<<"Comando -> "<<comando<<endl;

        int size = 0;
        string fit, unit, path, type, name ;
        bool del = false, add = false;


        for(int i = 0; i < parametros.size(); i++){
            param = parametros.at(i);;

            if(param.find("-s->") == 0){
                param = replace_txt(param, "-s->", "");
                param = replace_txt(param, "\"", "");
                size = stoi(param);
            }

            if(param.find("-u->") == 0){
                param = replace_txt(param, "-u->", "");
                param = replace_txt(param, "\"", "");
                unit = param;
            }

            if(param.find("-path->") == 0){
                param = replace_txt(param, "-path->", "");
                param = replace_txt(param, "\"", "");
                path = param;
            }

            if(param.find("-t->") == 0){
                param = replace_txt(param, "-t->", "");
                param = replace_txt(param, "\"", "");
                type = param;
            }

            if(param.find("-f->") == 0){
                param = replace_txt(param, "-f->", "");
                param = replace_txt(param, "\"", "");
                fit = param.at(0);
            }

            if(param.find("-delete->") == 0){
                param = replace_txt(param, "-delete->", "");
                param = replace_txt(param, "\"", "");
                del = true;
            }

            if(param.find("-name->") == 0){
                param = replace_txt(param, "-name->", "");
                param = replace_txt(param, "\"", "");
                name = param;
            }

            if(param.find("-add->") == 0){
                param = replace_txt(param, "-add->", "");
                param = replace_txt(param, "\"", "");
                add = true;
            }
        }

        if (!del && !add){
            cmd.crearParticion(size,unit,fit,path,type,name);
        }

        else if (del && !add){
            if(name == ""){
                cout<<"[MIA]@Proyecto1:~$ No se ha especificado el nombre de particion a borrar"<<endl;
            }else{
                cmd.borrarParticion(path, name);
            }


        }

        else if (!del && add){
            cout<<"[MIA]@Proyecto1:~$ Add"<<endl;

        }else{
            cout<<"[MIA]@Proyecto1:~$ FDISK no puede tener comandos 'add' y 'delete'"<<endl;
        }






        

    }

    else if(comando == "rmdisk"){
        // cout<<"Comando -> "<<comando<<endl;

        string path;

        for(int i = 0; i < parametros.size(); i++){
            param = parametros.at(i);;

            if(param.find("-path->") == 0){
                param = replace_txt(param, "-path->", "");
                param = replace_txt(param, "\"", "");
                // cout<<"Ruta: "<<param<<endl;
                path = param;
            }
        }
        cmd.borrarDisco(path);
    }

    else if(comando == "mount"){
        string path;
        string part;
        string nombre_d;

        for(int i = 0; i < parametros.size(); i++){
            param = parametros.at(i);;

            if(param.find("-path->") == 0){
                param = replace_txt(param, "-path->", "");
                param = replace_txt(param, "\"", "");
                path = param;
            }

            if(param.find("-name->") == 0){
                param = replace_txt(param, "-name->", "");
                param = replace_txt(param, "\"", "");
                part = param;
            }
        }
        int index = path.find_last_of('/');
        nombre_d = path.substr(index + 1);

        int index_p = nombre_d.find_last_of('.');
        nombre_d = nombre_d.substr(0, index_p);

        cmd.mount(path, part, nombre_d);
    }

    else if(comando == "unmount"){
        string id;

        for(int i = 0; i < parametros.size(); i++){
            param = parametros.at(i);;

            if(param.find("-id->") == 0){
                param = replace_txt(param, "-id->", "");
                param = replace_txt(param, "\"", "");
                id = param;
            }
        }
        cmd.unmount(id);
    }

    else if(comando == "rep"){
        string path, id, name;

        for(int i = 0; i < parametros.size(); i++){
            param = parametros.at(i);;

            if(param.find("-id->") == 0){
                param = replace_txt(param, "-id->", "");
                param = replace_txt(param, "\"", "");
                id = param;
            }

            if(param.find("-name->") == 0){
                param = replace_txt(param, "-name->", "");
                param = replace_txt(param, "\"", "");
                name = param;
            }

            if(param.find("-path->") == 0){
                param = replace_txt(param, "-path->", "");
                param = replace_txt(param, "\"", "");
                path = param;
            }
        }

        if(name == "mbr"){
            cmd.reporteMBR(path, id);
        }

        if(name == "disk"){
            cmd.repDisco(path, id);
        }


    }

    else if(comando == "mkfs"){
        cout<<"Comando -> "<<comando<<endl;

        for(int i = 0; i < parametros.size(); i++){
            param = parametros.at(i);;

            if(param.find("-id->") == 0){
                param = replace_txt(param, "-id->", "");
                param = replace_txt(param, "\"", "");
                cout<<"id: "<<param<<endl;
            }

            if(param.find("-type->") == 0){
                param = replace_txt(param, "-type->", "");
                param = replace_txt(param, "\"", "");
                cout<<"Tipo: "<<param<<endl;
            }

            if(param.find("-fs->") == 0){
                param = replace_txt(param, "-fs>", "");
                param = replace_txt(param, "\"", "");
                cout<<"Formato: "<<param<<endl;
            }
        }
    }

    else if(comando == "login"){
        cout<<"Comando -> "<<comando<<endl;

        for(int i = 0; i < parametros.size(); i++){
            param = parametros.at(i);;

            if(param.find("-usr->") == 0){
                param = replace_txt(param, "-usr->", "");
                param = replace_txt(param, "\"", "");
                cout<<"Usuario: "<<param<<endl;
            }

            if(param.find("-pass->") == 0){
                param = replace_txt(param, "-pass->", "");
                param = replace_txt(param, "\"", "");
                cout<<"Contrasena: "<<param<<endl;
            }

            if(param.find("-id->") == 0){
                param = replace_txt(param, "-id->", "");
                param = replace_txt(param, "\"", "");
                cout<<"Id: "<<param<<endl;
            }
        }
    }

    else if(comando == "logout"){
        cout<<"Comando -> "<<comando<<endl;
    }

    else if(comando == "mkgrp"){
        cout<<"Comando -> "<<comando<<endl;

        for(int i = 0; i < parametros.size(); i++){
            param = parametros.at(i);;

            if(param.find("-name->") == 0){
                param = replace_txt(param, "-name->", "");
                param = replace_txt(param, "\"", "");
                cout<<"Nombre: "<<param<<endl;
            }
        }
    }

    else if(comando == "rmgrp"){
        cout<<"Comando -> "<<comando<<endl;

        for(int i = 0; i < parametros.size(); i++){
            param = parametros.at(i);;

            if(param.find("-name->") == 0){
                param = replace_txt(param, "-name->", "");
                param = replace_txt(param, "\"", "");
                cout<<"Nombre: "<<param<<endl;
            }
        }
    }

    else if(comando == "mkusr"){
        cout<<"Comando -> "<<comando<<endl;

        for(int i = 0; i < parametros.size(); i++){
            param = parametros.at(i);;

            if(param.find("-usr->") == 0){
                param = replace_txt(param, "-usr->", "");
                param = replace_txt(param, "\"", "");
                cout<<"Usuario: "<<param<<endl;
            }

            if(param.find("-pass->") == 0){
                param = replace_txt(param, "-pass->", "");
                param = replace_txt(param, "\"", "");
                cout<<"Contrasena: "<<param<<endl;
            }

            if(param.find("-grp->") == 0){
                param = replace_txt(param, "-grp->", "");
                param = replace_txt(param, "\"", "");
                cout<<"Grupo: "<<param<<endl;
            }
        }
    }

    else if(comando == "rmusr"){
        cout<<"Comando -> "<<comando<<endl;

        for(int i = 0; i < parametros.size(); i++){
            param = parametros.at(i);;

            if(param.find("-usr->") == 0){
                param = replace_txt(param, "-usr->", "");
                param = replace_txt(param, "\"", "");
                cout<<"Usuario: "<<param<<endl;
            }
        }
    }

    else if(comando == "chmod"){
        cout<<"Comando -> "<<comando<<endl;

        for(int i = 0; i < parametros.size(); i++){
            param = parametros.at(i);;

            if(param.find("-ugo->") == 0){
                param = replace_txt(param, "-ugo->", "");
                param = replace_txt(param, "\"", "");
                cout<<"Permisos: "<<param<<endl;
            }

            if(param.find("-path->") == 0){
                param = replace_txt(param, "-path->", "");
                param = replace_txt(param, "\"", "");
                cout<<"Ruta: "<<param<<endl;
            }

            if(param.find("-r") == 0){
                cout<<"Recursivo"<<endl;
            }
        }
    }

    else if(comando == "mkfile"){
        cout<<"Comando -> "<<comando<<endl;

        for(int i = 0; i < parametros.size(); i++){
            param = parametros.at(i);;

            if(param.find("-path->") == 0){
                param = replace_txt(param, "-path->", "");
                param = replace_txt(param, "\"", "");
                cout<<"Ruta: "<<param<<endl;
            }

            if(param.find("-r") == 0){
                cout<<"Recursivo"<<endl;
            }

            if(param.find("-s->") == 0){
                param = replace_txt(param, "-s->", "");
                param = replace_txt(param, "\"", "");
                cout<<"Size: "<<param<<endl;
            }

            if(param.find("-cont->") == 0){
                param = replace_txt(param, "-cont->", "");
                param = replace_txt(param, "\"", "");
                cout<<"Contenido: "<<param<<endl;
            }
        }
    }

    else if(comando == "cat"){
        cout<<"Comando -> "<<comando<<endl;

        for(int i = 0; i < parametros.size(); i++){
            param = parametros.at(i);;

            if(param.find("-file") == 0){
                param = replace_txt(param, "-file", "");
                cout<<"File: "<<param.at(0)<<endl;

                string path;
                int first = param.find("\"");
                if(first != std::string::npos){
                    int last = param.find("\"", first+1);
                    path = param.substr(first,last-first);
                }

                path = replace_txt(path, "\"", "");
                cout<<"Ruta: "<<path<<endl;
            }
        }
    }

    else if(comando == "remove"){
        cout<<"Comando -> "<<comando<<endl;

        for(int i = 0; i < parametros.size(); i++){
            param = parametros.at(i);;

            if(param.find("-path->") == 0){
                param = replace_txt(param, "-path->", "");
                param = replace_txt(param, "\"", "");
                cout<<"Ruta: "<<param<<endl;
            }
        }
    }

    else if(comando == "edit"){
        cout<<"Comando -> "<<comando<<endl;

        for(int i = 0; i < parametros.size(); i++){
            param = parametros.at(i);;

            if(param.find("-path->") == 0){
                param = replace_txt(param, "-path->", "");
                param = replace_txt(param, "\"", "");
                cout<<"Ruta: "<<param<<endl;
            }

            if(param.find("-cont->") == 0){
                param = replace_txt(param, "-cont->", "");
                param = replace_txt(param, "\"", "");
                cout<<"Contenido: "<<param<<endl;
            }
        }
    }

    else if(comando == "rename"){
        cout<<"Comando -> "<<comando<<endl;

        for(int i = 0; i < parametros.size(); i++){
            param = parametros.at(i);;

            if(param.find("-path->") == 0){
                param = replace_txt(param, "-path->", "");
                param = replace_txt(param, "\"", "");
                cout<<"Ruta: "<<param<<endl;
            }

            if(param.find("-name->") == 0){
                param = replace_txt(param, "-name->", "");
                param = replace_txt(param, "\"", "");
                cout<<"Nombre: "<<param<<endl;
            }
        }
    }

    else if(comando == "mkdir"){
        cout<<"Comando -> "<<comando<<endl;

        for(int i = 0; i < parametros.size(); i++){
            param = parametros.at(i);;

            if(param.find("-path->") == 0){
                param = replace_txt(param, "-path->", "");
                param = replace_txt(param, "\"", "");
                cout<<"Ruta: "<<param<<endl;
            }

            if(param.find("-p") == 0){
                param = replace_txt(param, "-p", "");
                cout<<"Padres"<<endl;
            }
        }
    }

    else if(comando == "copy"){
        cout<<"Comando -> "<<comando<<endl;

        for(int i = 0; i < parametros.size(); i++){
            param = parametros.at(i);;

            if(param.find("-path->") == 0){
                param = replace_txt(param, "-path->", "");
                param = replace_txt(param, "\"", "");
                cout<<"Ruta: "<<param<<endl;
            }

            if(param.find("-destino->") == 0){
                param = replace_txt(param, "-destino->", "");
                param = replace_txt(param, "\"", "");
                cout<<"Destino: "<<param<<endl;
            }
        }
    }

    else if(comando == "move"){
        cout<<"Comando -> "<<comando<<endl;

        for(int i = 0; i < parametros.size(); i++){
            param = parametros.at(i);;

            if(param.find("-path->") == 0){
                param = replace_txt(param, "-path->", "");
                param = replace_txt(param, "\"", "");
                cout<<"Ruta: "<<param<<endl;
            }

            if(param.find("-name->") == 0){
                param = replace_txt(param, "-name->", "");
                param = replace_txt(param, "\"", "");
                cout<<"Nombre: "<<param<<endl;
            }
        }
    }

    else if(comando == "chown"){
        cout<<"Comando -> "<<comando<<endl;

        for(int i = 0; i < parametros.size(); i++){
            param = parametros.at(i);;

            if(param.find("-path->") == 0){
                param = replace_txt(param, "-path->", "");
                param = replace_txt(param, "\"", "");
                cout<<"Ruta: "<<param<<endl;
            }

            if(param.find("-usr->") == 0){
                param = replace_txt(param, "-usr->", "");
                param = replace_txt(param, "\"", "");
                cout<<"Usuario: "<<param<<endl;
            }

            if(param.find("-r") == 0){
                cout<<"Recursivo"<<endl;
            }
        }
    }

    else if(comando == "chgrp"){
        cout<<"Comando -> "<<comando<<endl;

        for(int i = 0; i < parametros.size(); i++){
            param = parametros.at(i);;

            if(param.find("-grp->") == 0){
                param = replace_txt(param, "-grp->", "");
                param = replace_txt(param, "\"", "");
                cout<<"Grupo: "<<param<<endl;
            }

            if(param.find("-usr->") == 0){
                param = replace_txt(param, "-usr->", "");
                param = replace_txt(param, "\"", "");
                cout<<"Usuario: "<<param<<endl;
            }
        }
    }

    else if(comando == "pause"){
        cout<<"Pause"<<endl;
        cin.ignore();
    }

    else if(comando == "mounted"){
        cmd.getMount();
    }

    else{
        cout<<"[MIA]@Proyecto1:~$ Comando '"<<comando<<"' no esta definido"<<endl;
        return;
    }

}