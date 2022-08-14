#include "analizador.cpp"

int main(){
    Analizador analisis;
    string entradacmd = "";
    while(entradacmd != "exit"){
        cout << "||========================================================||" << endl;
        cout << "||                     [MIA] TAREA 2                      ||" << endl;
        cout << "||========================================================||" << endl;
        cout << "[MIA]@Ejemplo3:~$ ";
        getline(cin,entradacmd);   //command -> exec -path=/home/desquivel/Desktop/prueba.algo
        analisis.analizar(entradacmd);
    }
    return 0;
}
