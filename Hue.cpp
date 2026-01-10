#include "Hue.h"

char *pProgramPath;

int main(int argc, char *argv[])
{   
    int len, j;
    int action = 0;

//    std::filesystem::path cwd = std::filesystem::current_path();
//    string str = cwd.string() + "/";
//    j = str.length(); 
//   pProgramPath = new char[j+2];
//
    j = strlen(argv[0]);
    pProgramPath = new char[j+2];    
    strcpy(pProgramPath, argv[0]);
    for( ; j > 0; j-- )
    {
        if(pProgramPath[j-1] == '/')
        {
            pProgramPath[j] = 0;
            break;
        }
    }     
    len = argc;
    if(len == 1) // kein Argument
        action = 2; // Get Lights and Groups
    else
    {
        len = strlen(argv[1]);
        if(len == 1)
            action = atoi(argv[1]);
        if(action < 1 || action > 3)
        {   cout << "Falscher Parameter\n";
            exit(1); 
        }
    }       
    CHue *pHue = new CHue;
    pHue->Start(action);
    return 01;
}