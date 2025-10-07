# Introduction
Simple Pong game in C using SDL2 library.

The idea was based on HirschDaniels's youtube series "Coding Pong in C from Scratch" (First video of the series: https://www.youtube.com/watch?v=m_jDyqcvaQU)

The present implementation was not accomplished based on HirschDaniel's work as I only took the idea from the initial moments of the first video and did not see the whole series until I completed this project.


# Pong Game in C - Installation Guide

## Prerequisites

Before you begin, ensure you have the following installed on your system:

- A C compiler (e.g., `gcc` or `clang`)
- CMake (minimum version 3.10)
- Make (or an equivalent build system)

## Installation Steps

1. **Clone the Repository**

   Open a terminal and run the following command to clone the repository:

   ```bash
   git clone https://github.com/kiEryy/PongGame-in-C.git
   ```

2. **Navigate to the Project Directory**

   Change into the project directory:

   ```bash
   cd PongGame-in-C
   ```

3. **Create a Build Directory**

   Create a directory for the build files:

   ```bash
   mkdir build && cd build
   ```

4. **Run CMake**

   Generate the build files using CMake:

   ```bash
   cmake ..
   ```

5. **Build the Project**

   Compile the project using Make:

   ```bash
   make
   ```

6. **Run the Game**

   After the build process completes, create a symbolic link to the base folder:
    ```cd ..
    ln -s build/PongGame
   ```
   you can run the game executable:

   ```bash
   ./PongGame
   ```


## Notes

- Ensure that the `fonts/` directory containing the `OpenSans-Light.ttf` file is in the same directory as the executable, as the game may require it to load fonts. 
- If you encounter any issues, ensure that all dependencies are correctly installed and that you have the necessary permissions to execute the commands.

## License

This project is licensed under the MIT License. See the `LICENSE` file for details.