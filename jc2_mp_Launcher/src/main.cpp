#include <iostream>
#include <string>
#include <stdexcept>
#include <fstream>

#include "CProc_Interface.h"

using namespace std;

int main(int argc, char **argv, char **envp)
{
  try
    {
      std::string cfgFile; //the path to the config file

      if (argc > 1) //If we have an argument it's the config file
        cfgFile = argv[1];
      else //else we take the default name
        cfgFile = "config.txt";

      ifstream config(cfgFile.c_str(), ios::in); //We open in read mode the cfgfile
      if(!config) //If open was successful
        {
          cout << "Unable to open " << cfgFile << endl;
          return (1);
        }
      string strJc2Path; //the path to the game
      string strDllPath; //the path to the dll
      getline(config, strJc2Path);
      getline(config, strDllPath);

      Proc_Interface game("", 0, strJc2Path); //We launch the game in pause mode
      game.insertDll(strDllPath); //Inject dll
      game.resume_process(); //unpause process

      config.close(); //Close configu file
      Sleep(5000);
    }
  catch(std::exception const& e)
    {
      std::ofstream out("fatal.txt");
      cout << "Fatal exception: " << e.what() << std::endl;
      out << "Fatal exception: " << e.what() << std::endl;
    }
  return (0);
}
