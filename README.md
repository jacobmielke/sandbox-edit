# Sandbox Editor
A final project for ECE30834. Load in your heightmaps and edit them to create a fun world! If I had more than two weeks there would have been more custom loading. This project serves as great base for game engine editors. There are plenty of hidden features waiting to be finished and as of 05/03/2022 this project is finished.

## File organization
The project has both GLFW and GLAD within the project repository. glfw.lib was built specifically for my home machine and may need to be replaced to work for you. The open asset loader dll needs to be in the working directory for the program to function correctly.
```
sandbox-edit
├── inc
│   ├── GLFW
│   │   └── . . .
│   ├── KHR
│   │   └── . . .
│   └── glad
│       └── . . .
├── lib
│   └── glfw3.lib
├── sandbox-edit.sln
└── sandbox-edit
    ├── main.cpp
    ├── models
    │   └── object.obj
    ├── textures
        └── texture.png 
```

## References
- Learn OpenGL | [Book Link](https://www.amazon.com/gp/product/9090332561/ref=as_li_tl?tag=joeydevries-20&ie=UTF8&linkId=7dc8cb69143266ce47e97e21350bbfff&geniuslink=true) ISBN: 9090332561
- [stb_image.h](https://github.com/nothings/stb)
- [GLFW](https://www.glfw.org/download.html)
- [Open Asset Loader](https://assimp-docs.readthedocs.io/en/v5.1.0/)

> This was designed and created to teach myself openGL. Feel free to modify and alter as needed; but, do not mistake your work for others. Stay honest, Boiler Up!
