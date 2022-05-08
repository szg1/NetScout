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
#include <sstream>
#include <Windows.h>
#include <filesystem>

namespace fs = std::filesystem;

void writepingfile()
{
    std::filesystem::remove_all("C:\\Programs\\netdet\\reports");
    std::ofstream pingfile;
    _mkdir("C:\\Programs\\netdet");
    _mkdir("C:\\Programs\\netdet\\reports");
    pingfile.open("C:\\Programs\\netdet\\ping.cmd");
    pingfile << "ping %1 -a -n 5 > C:\\Programs\\netdet\\reports\\%1.report\n\"done\" > C:\\Programs\\netdet\\reports\\%1.done\nexit";
    pingfile.close();
}

std::vector<std::string> readlines(std::string path) {
    std::vector<std::string> lines;
    std::ifstream file(path);
    if (file.is_open())
    {
        std::string line;
        while (std::getline(file, line))
        {
            lines.push_back(line);
        }
    }
    return(lines);
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


void ping(std::string host, int thrid)
{
    std::string pipa = "start C:\\Programs\\netdet\\ping.cmd " + host;
    system(pipa.c_str());
}
//MAIN//MAIN//MAIN//MAIN//MAIN//MAIN//MAIN//MAIN//MAIN//MAIN//MAIN//MAIN//MAIN//
static int iprange = 256;
static int threadcount = 25;
std::vector<std::string> fps;
int main()
{
    writepingfile();
    std::string gateway = getdefgw();
    std::string ip;
    std::vector<std::thread> pingsvect;
    for (int i = 0; i < iprange; i++)
    {
        ip = gateway + std::to_string(i - threadcount);
        std::ifstream f(("C:\\Programs\\netdet\\reports\\" + ip.substr(1, ip.length()) + ".done").c_str());
        std::cout << std::endl;
        while (i > threadcount)
        {
            f.close();
            f.open(("C:\\Programs\\netdet\\reports\\" + ip.substr(1, ip.length()) + ".done").c_str());
            std::cout << "\rC:\\Programs\\netdet\\reports\\" + ip.substr(1, ip.length()) + ".done";
            if (f.good()) {
                std::cout << std::endl;
                break;
            }
        }
        f.close();
        ip = gateway + std::to_string(i);
        pingsvect.push_back(std::thread(ping, ip, i));
        pingsvect[i].join();
    }
    ip = gateway + std::to_string(iprange - 1);

    while (true) {
        std::ifstream f(("C:\\Programs\\netdet\\reports\\" + ip.substr(1, ip.length()) + ".done").c_str());
        std::cout << "\rC:\\Programs\\netdet\\reports\\" + ip.substr(1, ip.length()) + ".done";
        if (f.good()) {
            std::cout << std::endl;
            f.close();
            break;
        }
        f.close();
    }

    for (int i = 0; i < iprange; i++)
    {
        std::string ip = gateway + std::to_string(i);
        std::string path = "C:\\Programs\\netdet\\reports\\" + ip.erase(0, 1) + ".report";
        //std::cout << path << std::endl;
        std::vector<std::string> thisf = readlines(path);
        std::string thisl = thisf[9];
        std::vector<std::string> thisls = ssplit(thisl, "=");
        std::string avgrpas = "";
        int avgrpai = 0;
        try
        {
            try
            {
                if (thisf.size() > 10) {
                    std::string avgrp = ssplit(thisf[thisf.size() - 1], "=")[2];
                    //std::cout << avgrp << std::endl;
                    for (int j = 0; j < avgrp.length(); j++)
                    {
                        if (isdigit(avgrp[j]))
                        {
                           // std::cout << avgrp[j];
                            avgrpas += avgrp[j];
                            avgrpai = stoi(avgrpas);
                        }
                    }
                }
            }
            catch (const std::exception& e) 
            {
                std::cout << "error occured: " << e.what() << std::endl;
                std::cout << avgrpas;
                return false;
            }
            int sent = std::stoi(&thisls[1][1]);
            int rece = std::stoi(&thisls[2][1]);
            if (avgrpai > 0)
            {
                std::cout << gateway + std::to_string(i) << ": \n\t" << rece << "/" << sent << " packages retreeived\n\t avg response time: " << avgrpai;
                std::cout << std::endl;
            }

        }
        catch (const std::exception& e) 
        {
            std::cout << "error occured: " << e.what() << std::endl;
            std::cout << avgrpas;
            return false;
        }

   }
}
