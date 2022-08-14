#ifndef ANALIZADOR_H
#define ANALIZADOR_H

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

using namespace std;

class Analizador{
    private:
        

    public:
        vector<string> split_txt(string entrada);
        string replace_txt(string str, const string& from, const string& to);
        void analizar(string entrada);
        void identificar(string comando, vector<string> parametros);
};




#endif // ANALIZADOR_H
