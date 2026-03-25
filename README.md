# Backrans

Un motor de generación procedural de entornos 3D escrito en C++ y OpenGL. Este proyecto explora la creación de "chunks" de habitaciones y pasillos infinitos, similar al concepto de los "Backrooms".

## Características

*   **Generación Procedural**: Creación infinita de secciones interconectadas basada en semillas.
*   **Sistema de Chunks**: Carga y descarga dinámica de geometría alrededor del jugador para optimizar el rendimiento.
*   **Cámara 3D**: Movimiento libre con estilo FPS (First Person Shooter) y modo vuelo.
*   **Renderizado OpenGL**: Uso de shaders modernos (Core Profile 3.3) y texturizado.

## Requisitos

Para compilar este proyecto necesitarás:

*   **CMake** (Versión 3.10 o superior)
*   **Compilador compatible con C++17** (MSVC, GCC, Clang)
*   Drivers de tarjeta gráfica compatibles con **OpenGL 3.3**

El proyecto utiliza las siguientes librerías (incluidas en la carpeta `Libraries`):
*   [GLFW](https://www.glfw.org/) - Gestión de ventanas e input.
*   [GLAD](https://glad.dav1d.de/) - Carga de funciones de OpenGL.
*   [GLM](https://github.com/g-truc/glm) - Matemáticas para gráficos (vectores, matrices).
*   [stb_image](https://github.com/nothings/stb) - Carga de imágenes para texturas.

## Compilación y Ejecución

Este proyecto utiliza el sistema de construcción CMake.

1.  **Clonar el repositorio**:
    ```bash
    git clone https://github.com/tu-usuario/backrans.git
    cd backrans
    ```

2.  **Configurar y construir**:
    ```bash
    mkdir build
    cd build
    cmake ..
    cmake --build . --config Release
    ```

3.  **Ejecutar**:
    El ejecutable se generará en la carpeta `build/Release` (o `build` en Linux). Asegúrate de ejecutarlo desde una ubicación donde pueda encontrar los shaders (`3.3.shader.vs`, `3.3.shader.fs`) y las texturas (`pared.jpg`), o copia esos archivos junto al ejecutable.

    ```bash
    ./Program
    ```

## Controles

*   **W, A, S, D**: Movimiento horizontal.
*   **Espacio**: Subir (Volar verticalmente).
*   **Shift Izquierdo**: Bajar (Volar verticalmente).
*   **Mouse**: Mirar alrededor.
*   **ESC**: Salir del programa.

## Documentación Técnica

El proyecto ha sido diseñado de manera modular. Para obtener detalles profundos sobre la arquitectura, las clases (`Model`, `ChunkLoader`, `Camera`, etc.) y sus funciones, por favor consulta el archivo **[MANUAL.md](MANUAL.md)**.