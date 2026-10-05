# Project README

## Overview
A implementation of the marching cubes algorithm as a graphical representation for a 2D height map.

## Features
- **Marching Cubes**: generating a 3D mesh for the pipeline representing the terrain
- **Real-time Rendering**: Uses a basic 3D rendering engine to display the terrain in a window.
- **Camera Control**: Allows user interaction with the camera to rotate, zoom, and pan through the simulation.
- **Debugging Information**: Displays the current position of the camera and some statistics about the rendering process.

## Project Structure
```
Gui_Marching_Cubes/
├── build/              # .exe files produced by Main.c
├── src/                # Source code directory
│   └── Main.c          # Entry point of the application
├── Makefile.linux      # Linux Build configuration
├── Makefile.windows    # Windows Build configuration
├── Makefile.wine       # Wine Build configuration
├── Makefile.web        # Emscripten Build configuration
└── README.md           # This file
```

### Prerequisites
- C/C++ Compiler and Debugger (GCC, Clang)
- Make utility
- Standard development tools
- Libraries needed in specific projects (X11 for Linux, user32, gdi32, winmm for Windows)

## Build & Run
The project supports building on different platforms using the provided Makefiles.
These commands will set up and compile the project, producing the necessary executable file(s) for the specified platform.

### Building on Linux
```sh
cd Gui_Marching_Cubes/
make -f Makefile.linux all

# To run the application:
./build/Main
```

### Building on Windows
```sh
cd Gui_Marching_Cubes/
make -f Makefile.windows all

# To run the application:
.\build\Main.exe
```

### Building on Wine
```sh
cd Gui_Marching_Cubes/
make -f Makefile.wine all

# To run the application:
wine build/Main.exe
```

### Building for WebAssembly
```sh
cd Gui_Marching_Cubes/
make -f Makefile.web all

# To run the application in a web browser:
emrun --no_browser --port 8080 build/index.html
```