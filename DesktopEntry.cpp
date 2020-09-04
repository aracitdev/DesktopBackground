#include "DesktopEntry.h"
#include "Utility.h"
#include "Timer.h"

DesktopEntry::DesktopEntry()
{
    shader=nullptr;
}

DesktopEntry::DesktopEntry(const std::string& texture, sf::Shader* shad, bool isMan, bool use, size_t timeBetween, size_t timeDisplay)
{
    shader=shad;
    textureFile=texture;
    useMouse=use;
    timeBetweenFrames=timeBetween;
    timeToDisplay=timeDisplay;
    managedShader=isMan;
}


DesktopEntry::~DesktopEntry()
{
    for(uint32_t counter=0; counter < textures.size(); counter++)
        delete textures[counter];
    if(managedShader && shader) //we do it this way so multiple entries can shader the same shader
        delete shader;
}

void DesktopEntry::ExternalConvert(void)
{
    if(textureFile.empty() || !Exist(textureFile))
        return; //texture file not found
    std::string targetPath =  GetTemporaryPath() + std::string("DesktopBackgroundChanger\\")+ClipString(RemoveExtension(GetFileName(textureFile)), 10)+std::string("\\");
    MakeDirectory(targetPath);    //create a new temp directory for it

    if(GetExtension(textureFile) != std::string("gif"))
    {
        //not a gif, so just copy the file directly
        Copy(textureFile, targetPath);
        return;
    }

    std::string cmd = std::string("Magik\\convert -coalesce -quality 0 ") + textureFile +std::string(" ") + targetPath+std::string("t.png");
    std::cout <<"Command " <<cmd<<"\n";
    RunCommand(cmd);
}

void DesktopEntry::Convert(void)
{
    if(textureFile.empty() || !Exist(textureFile)) //it seems the texture file cannot be found
    {
        return;
    }
    std::string targetPath =  GetTemporaryPath() + std::string("DesktopBackgroundChanger\\")+ClipString(RemoveExtension(GetFileName(textureFile)), 10)+std::string("\\");
    if(Exist(targetPath))   //the targetpath already exists
        return;
    MakeDirectory(targetPath);    //create a new temp directory for it

    if(GetExtension(textureFile) != std::string("gif"))
    {
        //not a gif, so just copy the file directly
        Copy(textureFile, targetPath);
        return;
    }
    //only necessary if the file is a gif
    std::vector<Magick::Image> imageList;

    Magick::readImages( &imageList, textureFile );  //read frames and coalesce them
    Magick::coalesceImages( &imageList, imageList.begin(), imageList.end());
    for(unsigned int i = 0; i < imageList.size(); i++)  //write them
        imageList[i].write(targetPath + FixedLengthStringNumber(i,3) + std::string(".bmp"));

    delays.resize(imageList.size());
    for(uint32_t counter=0; counter < imageList.size(); counter++)
        delays[counter]=imageList[counter].animationDelay();

    WriteDelays();  //write delays
}

void DesktopEntry::WriteDelays(void)
{
    std::string path= GetTemporaryPath() + std::string("DesktopBackgroundChanger\\") + ClipString(RemoveExtension(GetFileName(textureFile)),10) + std::string(".delay");
    std::ofstream out(path.c_str(), std::ofstream::out | std::ofstream::trunc | std::ofstream::binary);
    if(!out.is_open())
        return;
    size_t sz=delays.size();    //write the number of delays
    out.write((char*)&sz, sizeof(sz));
    for(uint32_t counter=0; counter < delays.size(); counter++)
        out.write((char*)(&delays[counter]), sizeof(delays[counter]));  //write each delay
    out.close();
}

void DesktopEntry::ReadDelays(void)
{
    std::string path= GetTemporaryPath() + std::string("DesktopBackgroundChanger\\") + ClipString(RemoveExtension(GetFileName(textureFile)),10) + std::string(".delay");
    std::ifstream in(path.c_str(), std::ofstream::in | std::ifstream::binary);
    if(!in.is_open())
    {
        delays.clear();
        return;
    }

    size_t sz;
    in.read((char*)&sz, sizeof(sz));    //read the number of delays and prepare a vector for them
    delays.resize(sz);
    for(uint32_t counter=0; counter < delays.size(); counter++)
    {
        size_t delay=0;
        in.read((char*)&delay, sizeof(delay));  //read each of them
        delays[counter]=delay;
    }
}

void DesktopEntry::LoadFiles(void)
{
    std::string targetPath =  GetTemporaryPath() + std::string("DesktopBackgroundChanger\\")+ClipString(RemoveExtension(GetFileName(textureFile)),10) + std::string("\\");
    if(textureFile.empty() || !Exist(targetPath))
        return;

    std::vector<std::string> fileList=GetFilesInDirectory(targetPath);  //list of textures in a directory
    textures.resize(fileList.size());   //prepare a vector for each texture

    for(uint32_t counter=0; counter < fileList.size(); counter++)
    {
        sf::Texture* Tex=new sf::Texture();
        if(!Tex->loadFromFile(fileList[counter]))
        {
            delete Tex;
            Tex= nullptr;
        }
        textures[counter]=Tex;  //load each, set to null if it fails
    }

    ReadDelays();   //read the delays
}


void DesktopEntry::RunEntry(sf::RenderWindow& window)
{
    sf::Vector2u windowSize = window.getSize();
    uint32_t currentTexture=0;
    sf::Sprite displaySprite;

    if(shader)
    {
        //shadertoy style uniforms for resolution and the texture, respectively
        shader->setUniform("iResolution", sf::Vector2f(windowSize.x, windowSize.y));
        shader->setUniform("iChannel0", sf::Shader::CurrentTexture);
    }

    if(!textures.size())    //must be a shader only background
    {
        RunEmptyEntry(window);  //so run it differently (may not be necessary)
        return;
    }

    Timer clock;
    while(clock.GetTime() < timeToDisplay)  //loop until the time elapses
    {
        displaySprite.setScale(1,1);    //do this just to reset the scale (probably wouldn't hurt)
        displaySprite.setTexture(*textures[currentTexture]);
        displaySprite.setScale((float)windowSize.x / displaySprite.getLocalBounds().width,
                               (float)windowSize.y / displaySprite.getLocalBounds().height);    //scale to fit
        window.clear();

        if(shader)
        {
            sf::Vector2f mousePos = sf::Vector2f(0,0);  //default mouse position
            float time = ((float)clock.GetTime())/1000.0f;
            if(useMouse)
                mousePos=sf::Vector2f(sf::Mouse::getPosition().x, sf::Mouse::getPosition().y);  //mouse enabled, use actual
            shader->setUniform("iTime", time);  //shadertoy mouse and time uniforms respectively
            shader->setUniform("iMouse", mousePos);
            window.draw(displaySprite, shader);
        }
        else
            window.draw(displaySprite);
        window.display();

        if(delays.size() > currentTexture)
            sf::sleep(sf::milliseconds(delays[currentTexture] * 10));    //if we have a valid delay value, wait that long
        else
            sf::sleep(sf::milliseconds(timeBetweenFrames)); //otherwise sleep

        currentTexture=(currentTexture+1)%textures.size();  //wrap around
        while(textures[currentTexture] == nullptr)
        {
            currentTexture=(currentTexture+1)%textures.size();  //skip any null textures
        }
    }
}

void DesktopEntry::RunEmptyEntry(sf::RenderWindow& window)
{
    sf::Vector2u windowSize = window.getSize();
    sf::RectangleShape shape(sf::Vector2f(windowSize.x, windowSize.y));
    Timer clock;
    while(clock.GetTime() < timeToDisplay)
    {
        window.clear();
        sf::Vector2f mousePos = sf::Vector2f(0,0);
        if(useMouse)
            mousePos=sf::Vector2f(sf::Mouse::getPosition().x, sf::Mouse::getPosition().y);
        float time = ((float)clock.GetTime())/1000.0f;
        if(shader)
        {
            shader->setUniform("iTime", time);
            shader->setUniform("iMouse", mousePos);
            window.draw(shape, shader);
        }
        else
            window.draw(shape);
        window.display();
        sf::sleep(sf::milliseconds(timeBetweenFrames)); //shader only backgrounds use the default delay
    }
}

std::string DesktopEntry::GetTextureFile(void)
{
    return textureFile;
}
