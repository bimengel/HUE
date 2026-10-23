#include "Hue.h"

CHue::CHue()
{
    m_iVersion = 1;
}

CHue::~CHue()
{
    curl_easy_cleanup(m_pCurl);
}

int CHue::Start()
{
    int iDef, i, iRet = 0;
    string str;

    iDef = ReadConfig();
    i = iDef & 0b01;
    if(i != 0b01)
    {
        cout << "IP-Adresse der Bridge nicht definiert!";
        return 0;
    }
    i = iDef & 0b11;
    if(i == 0b01)
    {
        iRet = CreateUser();
        if(iRet == 1)
        {
            cout << "Bitte Taster auf der Bridge drücken und danach ENTER drücken" << endl;
            cin.get();
            iRet = ReadUser();
            if(iRet == 1)
                i = 0b11;
            else
                cout << "Error: siehe " + CReadFile::GetName(pProgramPath, 2, 0) << endl;
        }
    }
    if(i == 0b11)
    {
        ReadBridgeVersion();
        iRet = ReadGroupsLights(1);
        iRet += ReadGroupsLights(2);
        if(iRet == 2)
        {
            WriteConfig();
            cout <<  "Sie finden das Resultat in " << CReadFile::GetName(pProgramPath, 3, 0) << endl;
        }
    }

    return iRet;
}

//
//  Read bridge version
//
int CHue::ReadBridgeVersion()
{
    int iRet = 0, res, iErr;
    string strUrl, strUserHeader, strReadBuffer, strVersion, strHelp;
    struct curl_slist* headers = NULL;
    CJson *pJson = new CJson;
    CJsonNodeValue JsonNodeValue;
    bool bSuccess;
    long lVersion;

    m_pCurl = curl_easy_init();
    // Timeout auf 10 Sekunden
    curl_easy_setopt(m_pCurl, CURLOPT_CONNECTTIMEOUT, 10L);   
    curl_easy_setopt(m_pCurl, CURLOPT_ERRORBUFFER, m_curlErrorBuffer); 
    // URL verification
    curl_easy_setopt(m_pCurl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(m_pCurl, CURLOPT_SSL_VERIFYHOST, 0L); 
    // URL
    strUrl = "https://" + m_strIP + "/api/config";
    strUserHeader = "hue-application-key:" + m_strUSER;
    headers = curl_slist_append(headers, strUserHeader.c_str()); 
    curl_easy_setopt(m_pCurl, CURLOPT_URL, strUrl.c_str());
    // Headers
    headers = curl_slist_append(headers, "Content-Type:application/json;charset=utf-8");
    curl_easy_setopt(m_pCurl, CURLOPT_HTTPHEADER, headers); 
    // Get
    curl_easy_setopt(m_pCurl, CURLOPT_HTTPGET, 1L);              
    // Empfang
    curl_easy_setopt(m_pCurl, CURLOPT_WRITEFUNCTION, CHue::write_data);
    curl_easy_setopt(m_pCurl, CURLOPT_WRITEDATA, (void *)&strReadBuffer);    
    res = curl_easy_perform(m_pCurl);
    if(res != CURLE_OK)
        cout << strReadBuffer << endl;
    else
    {
        bSuccess = pJson->parse((char *)strReadBuffer.c_str(), strReadBuffer.length(), &iErr);
        if(bSuccess)
        {
            pJson->WriteToFile("Hue", 2, true);         
            pJson->get((char *)"swversion", &JsonNodeValue);
            strVersion = JsonNodeValue.asString();
            lVersion = atol(strVersion.c_str());
            strHelp = "'swversion: " + strVersion;
            if(m_iVersion > 1)
            {
                if(lVersion < 1955082050l)
                    strHelp += " nicht für Version 2 geeignet";
                if(lVersion < 1948086000l)
                    strHelp += " nicht für Entertainment geeignet";
                strHelp += "\n";
            }
            m_strBuffer.push_back(strHelp);            
        }
    }     

    return iRet;
}
//
// Read Groups and Lights
//

int CHue::ReadGroupsLights(int iTyp)
{
    int iRet = 0, res, iErr, iNr;
    string strUrl, strReadBuffer, strName, strID;
    struct curl_slist* headers = NULL;
    CJson *pJson = new CJson;
    CJsonNodeValue JsonNodeValue;
    CJsonNode *pJsonNode;
    string strUserHeader;    
    bool bSuccess;

    m_pCurl = curl_easy_init();
    // Timeout auf 10 Sekunden
    curl_easy_setopt(m_pCurl, CURLOPT_CONNECTTIMEOUT, 10L);   
    curl_easy_setopt(m_pCurl, CURLOPT_ERRORBUFFER, m_curlErrorBuffer); 
    // URL verification
    curl_easy_setopt(m_pCurl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(m_pCurl, CURLOPT_SSL_VERIFYHOST, 0L); 
    // URL
    strUrl = "https://" + m_strIP;
    switch(m_iVersion) {
    case 1:
        if(iTyp == 1)
            strUrl += "/api/" + m_strUSER + "/lights";
        if(iTyp == 2)
            strUrl += "/api/" + m_strUSER + "/groups";        
        break;
    case 2:
        strUrl += "/clip/v2/resource/";
        if(iTyp == 1)
            strUrl += "light";
        if(iTyp == 2)
            strUrl += "room";
        strUserHeader = "hue-application-key:" + m_strUSER;
        headers = curl_slist_append(headers, strUserHeader.c_str());            
        break;
    default:
        strUrl = "";
        break;
    }
    curl_easy_setopt(m_pCurl, CURLOPT_URL, strUrl.c_str());
    // Headers
    headers = curl_slist_append(headers, "Content-Type:application/json;charset=utf-8");
    curl_easy_setopt(m_pCurl, CURLOPT_HTTPHEADER, headers); 
    // Get
    curl_easy_setopt(m_pCurl, CURLOPT_HTTPGET, 1L);              
    // Empfang
    curl_easy_setopt(m_pCurl, CURLOPT_WRITEFUNCTION, CHue::write_data);
    curl_easy_setopt(m_pCurl, CURLOPT_WRITEDATA, (void *)&strReadBuffer);    
    res = curl_easy_perform(m_pCurl);
    if(res != CURLE_OK)
        cout << strReadBuffer << endl;
    else
    {   
        bSuccess = pJson->parse((char *)strReadBuffer.c_str(), strReadBuffer.length(), &iErr);
        if(bSuccess)
        {
            pJson->WriteToFile("Hue", 2, false);           
            switch(m_iVersion) {
            case 1:  // V1
                for(iNr=1; iNr < 256; iNr++)
                {
                    pJsonNode = pJson->get((char *)to_string(iNr).c_str(), &JsonNodeValue);
                    if(JsonNodeValue.m_iTyp)
                    {
                        pJsonNode->get((char*)"name", &JsonNodeValue);
                        if(iTyp == 1)
                            strName = "HUELIGHT:";
                        else
                            strName = "HUEGROUP:";
                        strName += to_string(iNr) + ";256  ' " + JsonNodeValue.asString() + "\n";
                        m_strBuffer.push_back(strName);
                    }
                } 
                iRet = 1;
                break;
            case 2:
                for(iNr=1; iNr < 256; iNr++)
                {
                    pJsonNode = pJson->get((char *)"data", &JsonNodeValue)->get((char *)to_string(iNr).c_str(), &JsonNodeValue);
                    if(JsonNodeValue.m_iTyp == 1)
                    {
                        pJsonNode->get((char*)"metadata", &JsonNodeValue)->get((char *)"name", &JsonNodeValue);
                        strName = JsonNodeValue.asString();
                        pJsonNode->get((char*)"id", &JsonNodeValue);
                        strID = JsonNodeValue.asString();                    
                        if(iTyp == 1)
                        {   
                            pJsonNode->get((char*)"id", &JsonNodeValue);
                            strID = JsonNodeValue.asString();                        
                            strName = "HUELIGHT:" + strID + ";100  '" + strName + "\n";
                        }
                        else
                        {
                            pJsonNode->get((char*)"services", &JsonNodeValue)
                                ->get((char *)to_string(1).c_str(), &JsonNodeValue)
                                ->get((char *)"rid", &JsonNodeValue);
                            strID = JsonNodeValue.asString();
                            strName = "HUEGROUP:"+ strID + ";100  '" + strName + "\n";
                        }
                        m_strBuffer.push_back(strName);
                    }
                } 
                iRet = 1;            
                break;
            }             
        }
        else
            cout << strReadBuffer << endl; 
    }
    curl_easy_cleanup(m_pCurl);    
    return iRet;
}

//
//  ReadConfig
//  iRet 0b00000000

int CHue::ReadConfig()
{
    int iRet = 0;
    char buf[256];

    m_pReadFile = new CReadFile;
    m_pReadFile->OpenRead(pProgramPath, 1); // Hue.config

    for (;;)
    {
        if (m_pReadFile->ReadLine())
        {
            m_pReadFile->ReadBuf(buf, ':');
            if (strncmp(buf, "HUEIP", 5) == 0 && strlen(buf) == 5)
            {
                m_strIP = m_pReadFile->ReadText();
                iRet |= 1;
            } 
            else if (strncmp(buf, "HUEUSER", 7) == 0 && strlen(buf) == 7)
            {
                m_strUSER = m_pReadFile->ReadText();
                iRet |= 2;
            }
            else if(strncmp(buf, "HUEVERSION", 10) == 0 && strlen(buf) == 10)   
            {
                m_iVersion = m_pReadFile->ReadNumber();
                if(m_iVersion < 1 || m_iVersion > 2)
                {
                    cout << "Nur Version 1 und 2 wird unterstützt" << endl;
                    return 0;
                }
            }            
        }
        else
            break;
    }
    m_pReadFile->Close();
    delete m_pReadFile;
    m_pReadFile = NULL;

    return iRet;
}

int CHue::WriteConfig()
{
    string str;
    int iRet = 0;

    m_pReadFile = new CReadFile;
    m_pReadFile->OpenWrite(pProgramPath, 3, 0);
    if(m_iVersion == 2)
    {   str = "HUEVERSION:" + to_string(m_iVersion) + "\n";
        m_pReadFile->WriteLine(str.c_str());
    }
    str = "HUEIP:" + m_strIP + "\n";
    m_pReadFile->WriteLine(str.c_str());
    str = "HUEUSER:" + m_strUSER + "\n";
    m_pReadFile->WriteLine(str.c_str());
    for(auto it = m_strBuffer.begin();it != m_strBuffer.end(); it++)
    {
        str = *it;
        m_pReadFile->WriteLine(str.c_str());
    }
    m_pReadFile->Close();
    delete m_pReadFile;
    m_pReadFile = NULL;  
    return iRet;
}

int CHue::CreateUser()
{
    int iRet = 0, res, iErr;
    string strUrl, strReadBuffer, strBody;
    struct curl_slist* headers = NULL;
    CJson *pJson = new CJson;
    CJsonNodeValue JsonNodeValue;
    CJsonNode *pJsonNode;    
    bool bSuccess;

    m_pCurl = curl_easy_init();
    // Timeout auf 10 Sekunden
    curl_easy_setopt(m_pCurl, CURLOPT_CONNECTTIMEOUT, 10L);   
    curl_easy_setopt(m_pCurl, CURLOPT_ERRORBUFFER, m_curlErrorBuffer); 
    // URL verification
    curl_easy_setopt(m_pCurl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(m_pCurl, CURLOPT_SSL_VERIFYHOST, 0L); 
    // URL
    strUrl = "https://" + m_strIP;
    switch(m_iVersion) {
    case 1:
        strUrl += "/api/newdevelopper";
        break;
    case 2:
        strUrl += "/api/newdevelopper";
        break;
    default:
        strUrl = "";
        break;
    }
    curl_easy_setopt(m_pCurl, CURLOPT_URL, strUrl.c_str());
    // Headers
    headers = curl_slist_append(headers, "Content-Type:application/json;charset=utf-8");
    curl_easy_setopt(m_pCurl, CURLOPT_HTTPHEADER, headers); 
    // Get or Post
    curl_easy_setopt(m_pCurl, CURLOPT_HTTPGET, 1L);              
    // Empfang
    curl_easy_setopt(m_pCurl, CURLOPT_WRITEFUNCTION, CHue::write_data);
    curl_easy_setopt(m_pCurl, CURLOPT_WRITEDATA, (void *)&strReadBuffer);    
    res = curl_easy_perform(m_pCurl);
    if(res != CURLE_OK)
        cout << strReadBuffer << endl;
    else
    {    
        bSuccess = pJson->parse((char *)strReadBuffer.c_str(), strReadBuffer.length(), &iErr);
        if(bSuccess)
        {
            pJsonNode = pJson->get((char *)"", &JsonNodeValue)->get(0, &JsonNodeValue);
            if(JsonNodeValue.m_iTyp != 0)
            {
                pJsonNode->get((char*)"error", &JsonNodeValue)->get((char *)"description", &JsonNodeValue);
                if(JsonNodeValue.asString() == "unauthorized user")
                    iRet = 1;
                else
                {
                    cout << strReadBuffer << endl;
                    cout << "User kann nicht erstellt werden" << endl;
                }
            } 
            else
                cout << strReadBuffer << endl;             
        }
        else
            cout << strReadBuffer << endl; 
    }
    curl_easy_cleanup(m_pCurl);    
    return iRet;
}

int CHue::ReadUser()
{
    int iRet = 0, res, iErr;
    string strUrl, strBody, strReadBuffer, strHeader;
    struct curl_slist* headers = NULL;
    CJson *pJson = new CJson;
    CJsonNodeValue JsonNodeValue;
    bool bSuccess;
    CJsonNode *pJsonNode;

    m_pCurl = curl_easy_init();
    // Timeout auf 10 Sekunden
    curl_easy_setopt(m_pCurl, CURLOPT_CONNECTTIMEOUT, 10L);   
    curl_easy_setopt(m_pCurl, CURLOPT_ERRORBUFFER, m_curlErrorBuffer); 
    // URL verification
    curl_easy_setopt(m_pCurl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(m_pCurl, CURLOPT_SSL_VERIFYHOST, 0L); 
    // Headers
    headers = curl_slist_append(headers, "Content-Type:application/json;charset=utf-8");
    curl_easy_setopt(m_pCurl, CURLOPT_HTTPHEADER, headers); 
    // POST
    curl_easy_setopt(m_pCurl, CURLOPT_POST, 1L);  
    switch(m_iVersion) {
    case 1: 
        strBody = "{\"devicetype\":\"prowo#prowo\"}";
        strUrl = "https://" + m_strIP + "/api";         
        break;
    case 2:
        strBody = "{\"devicetype\":\"prowo#prowo\", \"generateclientkey\":true}";
        strUrl = "https://" + m_strIP + "/clip/v2"; 
        break;
    default:
        strBody = "";
        strUrl = "";
        break;
    }
    // URL
    curl_easy_setopt(m_pCurl, CURLOPT_URL, strUrl.c_str());    
    // Body
    curl_easy_setopt(m_pCurl, CURLOPT_POSTFIELDS, strBody.c_str());     
    // Empfang
    curl_easy_setopt(m_pCurl, CURLOPT_WRITEFUNCTION, &CHue::write_data);
    curl_easy_setopt(m_pCurl, CURLOPT_WRITEDATA, (void *)&strReadBuffer);    
    res = curl_easy_perform(m_pCurl);
    if(res != CURLE_OK)  
        cout << strReadBuffer << endl;
    else
    {    
        bSuccess = pJson->parse((char *)strReadBuffer.c_str(), strReadBuffer.length(), &iErr);
        if(bSuccess)
        {
            pJson->WriteToFile("Hue", 2, true);
            pJsonNode = pJson->get((char *)"", &JsonNodeValue)->get(0, &JsonNodeValue);
            if(JsonNodeValue.m_iTyp != 0)
            {
                pJsonNode = pJsonNode->get((char*)"success", &JsonNodeValue);
                if(JsonNodeValue.m_iTyp != 0)
                {
                    pJsonNode->get((char *)"username", &JsonNodeValue);
                    m_strUSER = JsonNodeValue.asString();
                    pJsonNode->get((char *)"clientkey", &JsonNodeValue);
                    m_strCLIENTKEY = JsonNodeValue.asString();
                    iRet = 1;
                }
                else
                    cout << strReadBuffer << endl;
            }
        }  
        else
            cout << strReadBuffer << endl; 
    }
    curl_easy_cleanup(m_pCurl);
    return iRet;
}

size_t CHue::write_data(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

string CHue::ReplaceBackSlash(string str)
{
    string newStr;
    int i, len;
    char ch;

    len = str.length();
    for(i=0; i < len; i++)
    {
        ch = str.at(i);
        if(ch != 0x5c)
            newStr += ch;
    }
    return newStr;
}
