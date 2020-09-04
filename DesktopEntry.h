/**************************************************************************************************
* \file DesktopEntry.h
* \author Sean C
* Holds the desktop entry class.
* Designed to function as a manager for the textures/shader in a background.
***************************************************************************************************/

#ifndef DESKTOPENTRY_H_INCLUDED
#define DESKTOPENTRY_H_INCLUDED
#include <tinyxml2.h>
#include <string>
#include <vector>
#include <SFML/Graphics.hpp>
#include <Magick++.h>
#include <iostream>
#include <fstream>

/**************************************************************************************************
* \class DesktopEntry
* A container for textures and shaders used in a single background entry.
***************************************************************************************************/
class DesktopEntry
{
public:

/**************************************************************************************************
* \fn DesktopEntry(const std::string& textureFile, sf::Shader* shader, bool isManaged, bool use, size_t timeBetween, size_t timeDisplay)
* \param textureFile The associated texture (animated or static)
* \param shader A pointer to the associated shader
* \param isManaged Whether or not this instance should manage the shader pointer (and delete it when finished)
* \param use Whether or not the shader should use the mouse
* \param timeBetween the ammount of time between frames (in milliseconds)
* \param timeDisplay the ammount of time to display this background (in milliseconds)
* Constructor for a functioning background.
***************************************************************************************************/
    DesktopEntry(const std::string& textureFile, sf::Shader* shader, bool isManaged, bool use, size_t timeBetween, size_t timeDisplay);


/**************************************************************************************************
* \fn ~DesktopEntry()
* Destructor.
* Cleans up textures, and shader if this background manages the shader.
***************************************************************************************************/
    ~DesktopEntry();


/**************************************************************************************************
* \fn void LoadFiles(void)
* Loads all associated textures of the shader and the delays.
***************************************************************************************************/
    void LoadFiles(void);


/**************************************************************************************************
* \fn void Convert(void)
* Converts and copies the given file into a usable format.
* Gifs are converted to a series of frames and delays are written.
* Files copied to the temp file for this background.
* Conversion will be skipped if the directory for the background already exists to save time.
***************************************************************************************************/
    void Convert(void);


/**************************************************************************************************
* \fn void RunEntry(sf::RenderWindow& window)
* \param window The window to render the background to.
* Plays the background for timeToDisplay milliseconds.
* Calls runEmptyEntry if there is no texture.
***************************************************************************************************/
    void RunEntry(sf::RenderWindow& window);


/**************************************************************************************************
* \fn void RunEmptyEntry(sf::RenderWindow& window)
* \param window The window to render the background to.
* Run a background that has no textures (shader only)
* \note This is separate because we still need a shape drawn to the whole screen to get the shader effect.
***************************************************************************************************/
    void RunEmptyEntry(sf::RenderWindow& window);


/**************************************************************************************************
* \fn void ExternalConvert(void)
* Acts exactly like convert, however, uses an external Magick convert command.
* \note This is not prefered to be used, however it may be faster on some systems.
***************************************************************************************************/
    void ExternalConvert(void);



/**************************************************************************************************
* \fn void WriteDelays(void)
* Writes the delays to a temporary file.
* Used so we don't have to reload a gif if we want to get the delays.
***************************************************************************************************/
    void WriteDelays(void);


/**************************************************************************************************
* \fn void ReadDelays(void)
* Reads the delays from a temporary file.
* Used so we don't have to reload a gif if we want to get the delays.
***************************************************************************************************/
    void ReadDelays(void);

/**************************************************************************************************
* \fn std::string GetTextureFile(void)
* An accessor to get the texture file.
***************************************************************************************************/
    std::string GetTextureFile(void);

private:
    std::string textureFile;
    std::vector<sf::Texture*> textures;
    std::vector<size_t> delays;
    sf::Shader* shader;
    bool managedShader;
    bool useMouse;
    size_t timeBetweenFrames;
    size_t timeToDisplay;
    DesktopEntry();
};

#endif // DESKTOPENTRY_H_INCLUDED
