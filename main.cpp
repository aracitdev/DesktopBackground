#include <iostream>
#include <shlobj.h>
#include <time.h>
#include <cstdlib>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <Magick++.h>
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>

#include "Utility.h"
#include "Timer.h"
#include "DesktopEntry.h"
#include "Thread.h"

#define _WIN32_WINNT 0x500
#include <windows.h>
#include <mutex>

std::mutex stdMutex;

int EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
    if(FindWindowEx(hwnd, nullptr,  "SHELLDLL_DefView", nullptr) != nullptr)
        *((HWND*)(lParam)) =FindWindowEx(nullptr,hwnd, "WorkerW", nullptr);
    return true;
}

HWND GetWorkerW(void)
{
    HWND ProgMan=FindWindow("ProgMan", nullptr);
    if(!ProgMan)
    {
        std::cout <<"Progman could not be found\n";
        return nullptr;
    }
    SendMessageTimeout(ProgMan, 0x052C, 0, 0, SMTO_NORMAL, 1000, nullptr);
    HWND WorkerW = nullptr;
    LPARAM Param = (LPARAM)&WorkerW;
    EnumWindows(EnumWindowsProc, Param);
    return WorkerW;
}

void LoadConfigSegment(tinyxml2::XMLElement* element, bool& useMouse, size_t& timeBetweenFrames, size_t& timeToDisplay)
{
    if(element == nullptr)
        return;

    tinyxml2::XMLElement* useMouseElement=element->FirstChildElement("UseMouse");
    if(useMouseElement)
    {
        useMouse=useMouseElement->BoolText(useMouse);
    }

    tinyxml2::XMLElement* frameTimeElement=element->FirstChildElement("TimeBetweenFrames");
    if(frameTimeElement)
    {
        timeBetweenFrames=frameTimeElement->Unsigned64Text(timeBetweenFrames);
    }

    tinyxml2::XMLElement* displayTimeElement=element->FirstChildElement("TimeToDisplay");
    if(displayTimeElement)
    {
        timeToDisplay=displayTimeElement->Unsigned64Text(timeToDisplay);
    }
}

bool LoadConfig(const std::string& filename, std::vector<DesktopEntry*>& entries)
{
    tinyxml2::XMLDocument doc;

    if(doc.LoadFile(filename.c_str()) !=tinyxml2::XML_SUCCESS)
        return false;

    tinyxml2::XMLElement* baseElement;
    baseElement=doc.FirstChildElement();
    if(!baseElement)
    {
        std::cout <<"Failed to find base element.\n";
        return false;
    }

    //default settings are provided if none are specified in the XML.
    bool defaultUseMouse = false;
    size_t defaultTimeBetweenFrames=80;
    size_t defaultTimeToDisplay=30000; //30 seconds

    LoadConfigSegment(baseElement, defaultUseMouse, defaultTimeBetweenFrames, defaultTimeToDisplay);    //load the defaults

    tinyxml2::XMLElement* fileElement=baseElement->FirstChildElement("Files");
    if(!fileElement)
    {
        std::cout <<"Failed to find file list element.\n";
        return false;
    }

    for(tinyxml2::XMLElement* currentFile=fileElement->FirstChildElement(); currentFile != nullptr; currentFile=currentFile->NextSiblingElement())  //loop through each element under files
    {
        bool useMouse = defaultUseMouse;
        size_t timeBetweenFrames = defaultTimeBetweenFrames;
        size_t timeToDisplay = defaultTimeToDisplay;
        LoadConfigSegment(currentFile, useMouse, timeBetweenFrames, timeToDisplay);

        std::string textureFile;
        std::string shaderFile;

        tinyxml2::XMLElement* shaderFileElement=currentFile->FirstChildElement("Shader");
        if(shaderFileElement)
            shaderFile=shaderFileElement->GetText();

        tinyxml2::XMLElement* textureFileElement=currentFile->FirstChildElement("Texture");
        if(textureFileElement)
            textureFile=textureFileElement->GetText();
        std::cout <<textureFile <<" " <<shaderFile<<"\n";

        sf::Shader* shader=nullptr;
        if(!shaderFile.empty() && Exist(shaderFile))
        {

            if(!shaderFile.empty())
            {
                shader= new sf::Shader();
                if(!shader->loadFromFile(shaderFile, sf::Shader::Fragment))
                {
                    delete shader;
                    shader=nullptr;
                    std::cout <<"Failed to load the shader " <<shaderFile<<"\n";
                }
            }
        }

        if(Exist(textureFile) && IsDirectory(textureFile))  //this should only occur if this is not a single texture file and the texture file exists
        {
            std::vector<std::string> textureFileList = GetFilesInDirectory(textureFile);    //loop through textures in the folder
            for(uint32_t counter=0; counter < textureFileList.size(); counter++)
            {
                entries.push_back(new DesktopEntry(textureFileList[counter], shader, counter == 0, useMouse, timeBetweenFrames, timeToDisplay));
            }
        }
        else    //single image or shader only background
        {
            entries.push_back(new DesktopEntry(textureFile, shader, true, useMouse, timeBetweenFrames, timeToDisplay));
        }
    }

    return true;
}


bool ParseArguments(int argc, char* argv[], std::string& configFile, uint32_t& threadCount)
{
    for(int counter=1; counter < argc; counter++)
    {
        if(std::string(argv[counter]) == "--clear-cache")
        {
            std::cout <<"Deleting background cache\n";
            if(Exist(GetTemporaryPath() + std::string("DesktopBackgroundChanger/")))
                DeleteDirectory(GetTemporaryPath() + std::string("DesktopBackgroundChanger/"));
        }
        else
        if(std::string(argv[counter]) == "--help")
        {
            std::cout <<"DesktopBackgroundChanger [options]\n";
            std::cout <<"options\n--clear-cache Clear the image cache (do this if images aren't updating when you change them)\n";
            std::cout <<"--config [filename] Set the config xml file\n--help Display this message.\n";
            std::cout <<"Default config file is Config.xml\n";
            return false;
        }
        else
        if(std::string(argv[counter]) == "--config")
        {
            counter++;
            if(counter >= argc)
            {
                std::cout <<"Error: no file specified.\n";
                return false;
            }
            configFile = std::string(argv[counter]);
        }
        else
        if(std::string(argv[counter]) == "--threads")
        {
            counter++;
            if(counter >= argc)
            {
                std::cout <<"Error: Thread count not specified\n";
                return false;
            }
            threadCount = std::stoi(std::string(argv[counter]));
            if(threadCount > 4 || threadCount < 1)
            {
                std::cout <<"Error: Expected thread count is expected between 1 and 4\n";
                threadCount=1;
            }
        }
        else
        {
            std::cout <<"No option " << argv[counter]<<"\n";
            std::cout <<"Use DesktopBackgroundChanger --help for information.\n";
            return false;
        }
    }
    return true;
}

void terminate(void)
{
    while(1)
    {
        sf::sleep(sf::seconds(1));
    }
}


void RunThread(void* ptr)
{
    std::vector<DesktopEntry*>* entries = (std::vector<DesktopEntry*>*)ptr;
    stdMutex.lock();
    std::cout <<"Thread working on " <<(*entries).size()<<" backgrounds\n";
    stdMutex.unlock();
    for(uint32_t counter=0; counter < entries->size(); counter++)
    {
        stdMutex.lock();
        std::cout <<"Converting " <<(*entries)[counter]->GetTextureFile()<<"\n";
        stdMutex.unlock();
        (*entries)[counter]->Convert();
    }

    stdMutex.lock();
    std::cout <<"Thread finished.\n";
    stdMutex.unlock();

    delete entries;
}


int main(int argc, char* argv[])
{
    std::string configFile = "Config.xml";
    uint32_t threadCount=2;
    if(!ParseArguments(argc,argv, configFile,threadCount))
        return 0;

    std::string tempPath =GetTemporaryPath() + std::string("DesktopBackgroundChanger\\");
    std::cout <<"Temp Path is " <<tempPath<<"\n";
    if(!Exist(tempPath))
        MakeDirectory(tempPath); //create a folder if it doesn't already exist

    HWND device= GetWorkerW();
    if(device == nullptr)
    {
        std::cout <<"Failed to get the WorkerW.\n";
        return 0;
    }

    std::vector<DesktopEntry*> backgrounds;
    if(!LoadConfig(configFile, backgrounds))
    {
        std::cout <<"Failed to load the config file " <<configFile<<".\n";
        return 0;
    }
    std::cout <<"Loading backgrounds.\n";

    std::vector<DesktopEntry*> mainThreadProcessing=backgrounds;

    std::vector<Thread*> threadList;
    if(threadCount > 1)
    {
        uint32_t backgroundsPerThread = mainThreadProcessing.size() / threadCount;
        std::cout <<"Instancing " <<threadCount-1 <<" threads with " <<backgroundsPerThread <<" backgrounds per thread.\n";
        if(backgroundsPerThread)
        {
            threadList.resize(threadCount-1);
            for(uint32_t counter=0; counter < threadCount-1; counter++)
            {
                std::vector<DesktopEntry*>* threadBackgrounds = new std::vector<DesktopEntry*>();
                threadBackgrounds->insert(threadBackgrounds->begin(), mainThreadProcessing.begin(), mainThreadProcessing.begin() + (backgroundsPerThread - 1)); //each thread takes some from the main
                mainThreadProcessing.erase(mainThreadProcessing.begin(), mainThreadProcessing.begin() + (backgroundsPerThread -1)); //and erases them from the main backgrounds

                std::cout <<"Spawning new thread.\n";
                threadList[counter] = new Thread(RunThread, (void*)threadBackgrounds);
            }
        }
    }

    stdMutex.lock();
    std::cout <<"Main thread converting " <<mainThreadProcessing.size()<<" backgrounds\n";
    stdMutex.unlock();
    for(uint32_t counter=0; counter < mainThreadProcessing.size(); counter++)
    {
        stdMutex.lock();
        std::cout <<"Converting " <<mainThreadProcessing[counter]->GetTextureFile()<<"\n";
        stdMutex.unlock();
        mainThreadProcessing[counter]->Convert();    //this copies files and converts gifs
    }
    stdMutex.lock();
    std::cout <<"Main thread finished\n";
    stdMutex.unlock();

    for(uint32_t counter=0; counter < threadList.size(); counter++)
    {
        threadList[counter]->Join();    //ensure all are processed
        delete threadList[counter];
    }
    threadList.clear();

    std::cout <<"Loading backgrounds\n";
    for(uint32_t counter=0; counter < backgrounds.size(); counter++)
    {
        std::cout <<"Loading " <<backgrounds[counter]->GetTextureFile()<<"\n";
        backgrounds[counter]->LoadFiles();
    }
    std::cout <<"Loaded " <<backgrounds.size() <<" backgrounds.\n";

    sf::RenderWindow window(device);
    uint32_t currentBackground=0;
    while(1)
    {
        backgrounds[currentBackground]->RunEntry(window);
        currentBackground=(currentBackground+1)%backgrounds.size();
    }

}
