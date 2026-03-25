# Manual Técnico - Backrans

Este documento ofrece una guía completa sobre la estructura del código, las clases implementadas y cómo utilizarlas.

## Índice
1. [Camera](#class-camera)
2. [ChunkLoader](#class-chunkloader)
3. [Main](#archivo-maincpp)
4. [Model](#class-model)
5. [Paint](#class-paint)
6. [Seeder](#class-seeder)
7. [Shader](#class-shader)
8. [Texture](#class-texture)
9. [Shaders (GLSL)](#shaders-glsl)

---

## Class: Camera
**Archivo**: `camera.h`

### Descripción
Maneja la lógica de la cámara en un espacio 3D, incluyendo posición, vectores de dirección (frente, arriba, derecha) y matrices de vista para OpenGL.

### Instanciación
```cpp
Camera cam;
// Opcionalmente configurar posición inicial accediendo a cam.position
```

### Funciones Principales

- **`glm::mat4 getViewMatrix()`**
  - **Función**: Calcula y devuelve la matriz `lookAt` necesaria para shaders.
  - **Uso**: `shader.setMat4("view", cam.getViewMatrix());`

- **`void updatePos(State state, float dt)`**
  - **Función**: Actualiza la posición de la cámara basado en la dirección input y el delta time.
  - **Parámetros**:
    - `state`: Enum `States` (`FORWARD`, `BACKWARD`, `LEFT`, `RIGHT`, `FLY`, `DOWN`).
    - `dt`: Delta time (tiempo entre frames).

- **`void updateAngle(float xoffset, float yoffset)`**
  - **Función**: Recalcula los vectores de dirección (Front, Right) basado en el movimiento del mouse.

### Posibles Errores
- Si `dt` es 0 o muy grande, el movimiento será errático.
- Si no se llama a `updateAngle`, la cámara no rotará aunque se mueva el mouse.

---

## Class: ChunkLoader
**Archivo**: `chunkLoader.h`

### Descripción
Sistema de gestión procedural del mundo. Carga y descarga "chunks" (habitaciones) dinámicamente según la posición del jugador para optimizar memoria.

### Instanciación
```cpp
chunkLoader *loader = new chunkLoader(wallShader, wallTexture);
```

### Funciones Principales

- **`void loadChunk(GLFWwindow *window, Camera &cam)`**
  - **Función**: Método central. Calcula en qué celda está la cámara, genera los modelos de las celdas vecinas (radio definido), conecta pasillos entre ellas y las renderiza. También gestiona la limpieza de memoria de chunks lejanos.
  - **Parámetros**: Ventana actual y referencia a la cámara.

### Posibles Errores
- **Fuga de Memoria**: Si se modifican los parámetros de limpieza sin cuidado, el mapa `mundialMap` puede crecer indefinidamente.
- **Acceso a Shader/Texture nulos**: Si se pasa `nullptr` en el constructor, crasheará al intentar pintar.

---

## Archivo: main.cpp

### Descripción
Punto de entrada. Configura el entorno OpenGL (GLFW, GLAD), inicializa los sistemas principales y ejecuta el bucle de juego.

### Funciones Globales
- **`start`**: Inicializa ventana y contexto. Si falla, retorna `nullptr`.
- **`processKeyboardInput`**: Mapea teclas a acciones de la cámara.
- **`mouse_callback`**: Captura movimiento del ratón para rotar la cámara.

### Flujo Principal
1. Inicializar ventana.
2. Compilar shaders y cargar texturas.
3. Instanciar `chunkLoader`.
4. Loop: Limpiar buffers -> Configurar Matrices -> `loader->loadChunk()` -> Input -> Swap Buffers.

---

## Class: Model
**Archivo**: `model.h`, `model.cpp`

### Descripción
Define la estructura lógica de una habitación. Decide dimensiones y aperturas (puertas) de forma determinista usando una semilla.

### Funciones Principales

- **`void processSeed(int seed, int cellx, int cellz)`**
  - **Función**: Genera la habitación. Usa hashes para decidir si tiene puertas en sus 4 lados, asegurando consistencia con vecinos (ej. si mi vecino tiene puerta hacia mí, yo debo tener puerta hacia él).
  
- **`void generateCorridor(DoorPoints& p1, DoorPoints& p2)`**
  - **Función**: Crea la geometría (vértices e índices) para un pasillo que une dos puertas.

- **`void updatePointers()`**
  - **Función**: Actualiza los punteros crudos que usa OpenGL apuntando a los vectores dinámicos `std::vector`. Importante llamar tras modificar la geometría.

---

## Class: Paint
**Archivo**: `paint.h`

### Descripción
Wrapper de OpenGL. Gestiona los VAO, VBO y EBO para subir geometría a la GPU y dibujarla.

### Instanciación
```cpp
Paint* p = new Paint("textura.jpg", shader, texture);
```

### Funciones Principales

- **`void configBuffers(Model &room)`**
  - **Función**: Sube los vértices e índices del Modelo a la GPU.
  
- **`void draw(glm::vec3 pos, float rot)`**
  - **Función**: Dibuja el objeto. Configura la matriz `model` en el shader y hace la llamada `glDrawElements`.

---

## Class: Seeder
**Archivo**: `seeder.h`

### Descripción
Utilidad simple para generar números pseudo-aleatorios.

### Funciones
- **`int generateSeed(int a, int b)`**: Retorna un int entre `a` y `b`.

---

## Class: Shader
**Archivo**: `shader.h`

### Descripción
Carga, compila y enlaza shaders Vertex y Fragment.

### Instanciación
```cpp
Shader* s = new Shader("path/vert.vs", "path/frag.fs");
```

### Funciones Principales
- **`use()`**: `glUseProgram(ID)`.
- **`set[Type](name, value)`**: Helpers para enviar uniformes (Int, Bool, Float, Mat4) al shader.
- **Errores**: Imprime en consola si falla la compilación o el linkeo.

---

## Class: Texture
**Archivo**: `texture.h`

### Descripción
Carga imágenes (jpg, png) usando `stb_image` y crea texturas OpenGL.

### Instanciación
```cpp
Texture* t = new Texture("filename.jpg", shader);
```

### Funciones
- **`use(int unit)`**: Activa la textura en la unidad especificada.

---

## Shaders (GLSL)
**Archivos**: `3.3.shader.vs`, `3.3.shader.fs`

### Descripción
Programas que se ejecutan en la GPU para el renderizado.

- **Vertex Shader (`.vs`)**: Transforma los vértices del espacio local al espacio de pantalla (clip space) usando las matrices MVP (Model, View, Projection).
- **Fragment Shader (`.fs`)**: Determina el color de cada píxel, en este caso aplicando una textura.

### Uniforms Utilizados
- `model`, `view`, `projection`: Matrices de transformaciones 4x4.
- `text`: Sampler 2D para la textura mapeada.
