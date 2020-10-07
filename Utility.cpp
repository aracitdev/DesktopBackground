#include "Utility.h"
#include <filesystem>

bool DirectoryExists(const std::string& filename)
{
    return std::filesystem::exists(filename);
}

bool Exist(const std::string& filename)
{
    return std::filesystem::exists(filename);
}

std::vector<std::string> GetFilesInDirectory(std::string folder)
{
    std::vector<std::string> returnV;
    for(auto& p: std::filesystem::directory_iterator(folder))
        returnV.push_back(p.path().string());
    return returnV;
}

std::string GetFileName(const std::string& Directory)
{
    std::string returnV;
    size_t pos=Directory.find_last_of("\\/");   //search for directory indicators
    returnV= Directory.substr(pos +1);  //cut it off
    pos = Directory.find_last_of('.');
    return returnV.substr(0,pos -1);
}

std::string GetTemporaryPath(void)
{
    return std::filesystem::temp_directory_path().string();
}

void DeleteDirectory(const std::string &dir)
{
    std::filesystem::remove_all(dir);
}


std::string GetExtension(const std::string& path)
{
    size_t pos=path.find_last_of('.');
    return path.substr(pos+1);
}


std::string RemoveExtension(const std::string& in)
{
    std::string path=GetFileName(in);
    size_t pos=path.find_last_of('.');
    return path.substr(0,pos-1);
}


bool MakeDirectory(const std::string& path)
{
    return std::filesystem::create_directory(path);
}

void Copy(const std::string& from, const std::string& to)
{
    std::filesystem::copy(from, to, std::filesystem::copy_options::recursive);
}

bool IsDirectory(const std::string& in)
{
    return std::filesystem::is_directory(in);
}

std::string ClipString(const std::string& in, uint32_t size)
{
    if(in.size() > size)
        return in.substr(0, size);
    return in;
}

void RunCommand(const std::string& cmd)
{
    //bad
    system(cmd.c_str());
}




std::string FixedLengthStringNumber(uint32_t num, uint32_t length )
{
    std::string returnV=std::to_string(num);
    if(returnV.length() >= length)
        return returnV;
    returnV.insert(returnV.begin(), length - returnV.length(), '0');
    return returnV;
}
