#ifndef UTILITY_H_INCLUDED
#define UTILITY_H_INCLUDED

/**************************************************************************************************
* \file Utility.h
* \author Sean C
* Defines some helpful functions for dealing with windows.
***************************************************************************************************/

#include <string>
#include <vector>
#include <filesystem>
#include <system_error>

/**************************************************************************************************
* \fn bool DiretoryExists(const std::string& dirName)
* \param dirName The directory to check for.
* \return Returns true if dirName exists, false otherwise.
* Checks whether or not the directory dirName exists.
***************************************************************************************************/
bool DirectoryExists(const std::string& dirName);

/**************************************************************************************************
* \fn bool Exists(const std::string& filename)
* \param filename The file to check for.
* \return Returns true if filename exists, false otherwise.
* Returns whether or not the file exists.
***************************************************************************************************/
bool Exist(const std::string& filename);

/***************************************************************************************************
* \fn std::vector<std::string> GetFilesInDirectory(const std::string& folder)
* \param folder The folder to look for files in.
* \return Returns a list of files inside the folder.
* Searches for and creates a list of files inside of the folder.
***************************************************************************************************/
std::vector<std::string> GetFilesInDirectory(std::string folder);

/***************************************************************************************************
* \fn std::string GetFileName(const std::string& Directory)
* \param Directory the directory to clip.
* \return The filename.
* Gets the filename from the full directory, clipping off the path.
***************************************************************************************************/
std::string GetFileName(const std::string& directory);


/***************************************************************************************************
* \fn std::string RemoveExtension(const std::string& in)
* \param in The string to clip.
* \return The clipped string.
* Gets the filename without the filetype extension.
***************************************************************************************************/
std::string RemoveExtension(const std::string& in);

/***************************************************************************************************
* \fn std::string GetExtension(const std::string& path)
* \param path The directory to clip.
* \return The file extension.
* Gets the file extension from the path.
***************************************************************************************************/
std::string GetExtension(const std::string& path);

/***************************************************************************************************
* \fn void DeleteDirectory(const std::string& directory)
* \param directory The directory to delete.
* Deletes a specified directory and any subdirectories, including the files contained.
***************************************************************************************************/
void DeleteDirectory(const std::string &directory);

/***************************************************************************************************
* \fn void Copy(const std::string& from, const std::string& to)
* \param from The file/path to copy from.
* \param to The file/path to copy to.
* Copies a path or file.
***************************************************************************************************/
void Copy(const std::string& from, const std::string& to);

/***************************************************************************************************
* \fn void Copy(const std::string& from, const std::string& to)
* \param from The file/path to copy from.
* \param to The file/path to copy to.
* Copies a path or file.
***************************************************************************************************/
void Copy(const std::string& from, const std::string& to);

/***************************************************************************************************
* \fn void MakeDirectory(const std::string& path)
* \param path The path to create.
* Creates a path.
***************************************************************************************************/
bool MakeDirectory(const std::string& path);

/***************************************************************************************************
* \fn std::string GetTemporaryPath(void)
* \return The path to the temporary directory.
* Gets the path to the temporary directory.
***************************************************************************************************/
std::string GetTemporaryPath(void);

/***************************************************************************************************
* \fn bool IsDirectory(const std::string& path)
* \return Whether or not the provided path is a directory.
* Checks if the path provided is a directory.
***************************************************************************************************/
bool IsDirectory(const std::string& path);

/***************************************************************************************************
* \fn std::string ClipString(const std::string& str, uint32_t size)
* \param str The string to clip.
* \param size The max size of the string.
* \return The clipped string.
* Clips a string down to size, or does nothing to it if the string is already shorter.
***************************************************************************************************/
std::string ClipString(const std::string& str, uint32_t size);

/***************************************************************************************************
* \fn void RunCommand(const std::string& cmd)
* \param cmd The command to run.
* Runs a command that is provided.
* \warning This currently uses system(), so keep that in mind. Might be replaced later to use windows API commands.
***************************************************************************************************/
void RunCommand(const std::string&);

/***************************************************************************************************
* \fn std::string FixedLengthStringNumber(uint32_t num, uint32_t length )
* \param num The number to convert to a string.
* \param length The output length of the string.
* \return The number as a string of size length.
* Creates a string of the passed in number, and pads the front with zeros until it reaches the expected length.
* If the string of the number is already greater than or equal to length, returns it without padding.
***************************************************************************************************/
std::string FixedLengthStringNumber(uint32_t num, uint32_t length );

#endif // UTILITY_H_INCLUDED
