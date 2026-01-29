# Solids of Revolution

[![C++](https://img.shields.io/badge/std-c%2B%2B20-blue.svg?logo=c%2B%2B&logoColor=white)](https://isocpp.org/)
[![wxWidgets](https://img.shields.io/badge/GUI-wxWidgets-red.svg)](https://www.wxwidgets.org/)
[![OpenMP](https://img.shields.io/badge/Parallel-OpenMP-green.svg)](https://www.openmp.org/)
[![Build System](https://img.shields.io/badge/build-CMake-064f8c?logo=cmake&logoColor=white)](https://cmake.org/)

**Solids of Revolution** is a C++ graphical application that demonstrates the fundamentals of 3D computer graphics from scratch. It features a custom **software rendering engine** capable of generating "Solids of Revolution" from 2D profiles, applying affine transformations, and rasterizing the results using a custom Z-Buffer implementation without relying on hardware acceleration.

## 🚀 Features

*   **Geometry Generation:**
    *   **Solid of Revolution:** Converts 2D coordinate files (`.geo`) into 3D meshes by rotating points around the Z-axis.
    *   **Adjustable Quality:** Control the mesh density (number of segments per rotation) in real-time.
*   **Software Rasterization:**
    *   **Custom Z-Buffer:** Implements a manual depth buffer for proper hidden surface removal.
    *   **Scanline Rasterization:** Fills triangles pixel-by-pixel using CPU math.
    *   **Bresenham's Algorithm:** Draws precise wireframe edges.
    *   **Parallel Processing:** Heavily utilizes **OpenMP** to parallelize rendering across CPU cores for performance.
*   **Transformations:**
    *   Interactive sliders for **Translation**, **Rotation**, and **Scaling** (X, Y, Z axes).
    *   Perspective projection matrix.
*   **I/O:**
    *   Load and Save geometry profiles.
    *   Export high-resolution screenshots (PNG/BMP).

## 🛠️ Tech Stack

*   **Language:** C++20
*   **GUI Framework:** [wxWidgets](https://www.wxwidgets.org/)
*   **Parallelism:** OpenMP
*   **Build System:** CMake
*   **External Libs:** OpenGL (linked but rendering is software-based)

## ⚙️ Prerequisites

Before building, ensure you have the following installed:

1.  **C++ Compiler** compatible with C++20 (GCC, Clang, or MSVC).
2.  **CMake** (Version 3.1.0 or higher).
3.  **wxWidgets**:
    *   *Windows:* The CMake file defaults to searching `C:/Program Files (x86)/wxWidgets`. You may need to edit `CMakeLists.txt` line 39 if installed elsewhere.
    *   *Linux:* Install via package manager (e.g., `sudo apt-get install libwxgtk3.0-gtk3-dev`).

## 🖥️ Building

This project enforces **out-of-source builds**. Do not try to run `cmake .` in the root directory.

### Windows (PowerShell/CMD)

1.  **Create a build directory:**
    ```powershell
    mkdir build
    cd build
    ```

2.  **Configure:**
    ```powershell
    cmake .. -DCMAKE_BUILD_TYPE=Release
    ```

3.  **Build:**
    ```powershell
    cmake --build . --config Release
    ```

4.  **Run:**
    The executable will be generated in the `release` folder in your project root.
    ```powershell
    ..\release\exec.exe
    ```

### Linux / macOS

1.  **Create a build directory:**
    ```bash
    mkdir build && cd build
    ```

2.  **Configure:**
    ```bash
    cmake .. -DCMAKE_BUILD_TYPE=Release
    ```

3.  **Build:**
    ```bash
    make -j$(nproc)
    ```

4.  **Run:**
    ```bash
    ../release/exec
    ```

> **Note on Debugging:** If you build with `-DCMAKE_BUILD_TYPE=Debug`, the executable will be placed in the `debug/` folder instead.

## 🎮 Usage

1.  **Load Geometry:** Click **"Wczytaj Geometrię"** to load a `.geo` text file (format: `X Y Z` coordinates per line).
2.  **Edit View:**
    *   Use the **Translation** sliders to move the object.
    *   Use the **Rotation** sliders to spin the object.
    *   Use the **Scale** sliders to resize.
3.  **Rendering Options:**
    *   **Quality Slider:** Increase/decrease the number of vertices in the generated mesh.
    *   **Render Checkbox:** Toggles between the simplified wireframe view and the full Z-Buffered rasterization.
4.  **Screenshot:** Use the file menu to save the current render to an image file.
