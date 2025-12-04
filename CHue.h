/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   CHue.h
 * Author: josefengel
 *
 * Created on 07/10/2025
 */

#ifndef CHUE_H
#define CHUE_H

class CHue {
public:
    CHue();
    ~CHue();
    int Start(int action);
    
private:
    CReadFile *m_pReadFile;
    CURL *m_pCurl;
    string m_strIP;
    string m_strUSER;
    string m_strCLIENTKEY;
    string m_strAuthorization;
    string m_strSessionID;
    int m_iVersion;
    char m_curlErrorBuffer[CURL_ERROR_SIZE];
    vector <std::string > m_strBuffer;

    static size_t write_data(void *contents, size_t size, size_t nmemb, void *stream);  
    int ReadConfig();
    int WriteConfig();
    int CreateUser();
    int ReadUser();
    int ReadGroupsLights(int iTyp);
    string ReplaceBackSlash(string str);
};

#endif /* CHUE_H */