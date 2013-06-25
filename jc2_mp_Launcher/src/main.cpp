#include "CProc_Interface.h"

#include <iostream>
#include <string>
#include <fstream>


using namespace std;


int main()
{
     ifstream config("config.txt", ios::in);  // on ouvre en lecture le fichier de config

    if(!config)  // si l'ouverture a fonctionné
    {
        cout << "Unable to open the config.txt file !" << endl;
        return 1;
    }
    string strJc2Path; //the path to the game
    string strDllPath; //the path to the dll

    getline(config, strJc2Path);
    getline(config, strDllPath);

    //on lance le jeux en pause
    Proc_Interface jeux("",0,strJc2Path);
    //on injecte la dll
    jeux.insertDll(strDllPath);
    //on unpause le process
    jeux.resume_process();

    config.close();

    Sleep(5000);

    return 0;
}
