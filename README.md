# DesktopBackground
A mixture of two prior projects, a program that does animated gif desktop backgrounds and image backgrounds, and a program that allows usage of fragment shaders on the desktop.
Uses config.xml to tell which shaders/textures to use. If no texture is specified, then it uses the shader only, if no shaders are specified, then it uses the image only.
When a path is specified under textures, it will add all of the files in that path.

There are also some commands to help you out, just do DesktopBackground --help to get info on them.

Depends on Magick and tinyxml2.