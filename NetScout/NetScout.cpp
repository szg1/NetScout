// netdet.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <direct.h>
#include <string>
#include <thread>
#include <chrono>
#include <vector>
#include <regex>

void writepingfile()
{
    std::ofstream pingfile;
    _mkdir("c:\\Programs\\netdet");
    _mkdir("C:\\Programs\\netdet\\reports");
    pingfile.open("C:\\Programs\\netdet\\ping.cmd");
    pingfile << "ping %1 -a -n 5 > C:\\Programs\\netdet\\reports\\%1.report\nexit";
    pingfile.close();
}


std::vector<std::string> ssplit(std::string text, std::string delim)
{
    std::vector<std::string> res;
    for (int i = 0; i < text.length() - delim.length(); i++)
    {
        if (text.substr(i, delim.length()) == delim) {
            res.push_back(text.substr(0, i));
            text.erase(0, i + delim.length());
            i = 0;
        }
    }
    res.push_back(text);
    return(res);
}

std::string getdefgw()
{
    system("ipconfig | findstr /c:\"Default Gateway\" > C:\\Programs\\netdet\\reports\\defgw");
    std::ifstream defgw;
    defgw.open("C:\\Programs\\netdet\\reports\\defgw");
    std::string dat;
    getline(defgw, dat);
    //std::cout << dat << std::endl;
    dat = ssplit(dat, ":")[1];
    //std::cout << dat << std::endl;
    std::vector<std::string> ipparts = ssplit(dat, ".");
    dat = ipparts[0] + "." + ipparts[1] + "." + ipparts[2] + ".";
    //std::cout << dat << std::endl;
    return(dat);
}

void ping(std::string host)
{
    std::string pipa = "start C:\\Programs\\netdet\\ping.cmd " + host;
    system(pipa.c_str());

}

int main()
{
    std::string gateway = getdefgw();
    writepingfile();
    std::vector<std::thread> pingsvect;
    for (int i = 0; i < 5; i++)
    {
        std::string ip = gateway + std::to_string(i);
        pingsvect.push_back(std::thread(ping, ip));
    }

    for (int i = 0; i < 5; i++)
    {
        pingsvect[i].join();
    }

}
