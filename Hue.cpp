#include "Hue.h"

char *pProgramPath;

int main(int argc, char *argv[])
{   
    int j;

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
    CHue *pHue = new CHue;
    pHue->Start();
    return 01;
}