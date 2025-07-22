# Progetto C++ SDL2 + OpenGL

[![CI Tests](https://github.com/pingumen96/cpp-sdl-audio/actions/workflows/ci.yml/badge.svg)](https://github.com/pingumen96/cpp-sdl-audio/actions/workflows/ci.yml)

Questo progetto è una sandbox che mi permette di sperimentare con C++ e SDL2 nel contesto dello sviluppo di videogames, ora migrato per utilizzare **OpenGL** per il rendering moderno.

## Architettura

Allo stato attuale, vengono implementati i pattern State e Command per la gestione degli input, con l'obiettivo di creare un sistema di input flessibile e facilmente estendibile.

### Componenti Principali

- **Pattern State**: Stati di gioco (Menu, Playing, Paused) e stati del player (Standing, Jumping, Ducking)
- **Pattern Command**: Sistema di input flessibile con comandi mappabili
- **Rendering OpenGL**: Renderer moderno basato su OpenGL 3.3+
- **Sistema Matematico Custom**: Vettori e matrici 4x4 header-only per grafica con unit test completi
- **Sistema Audio**: Supporto per effetti sonori e musica (SDL_mixer)

## Tecnologie Utilizzate

- **C++20**: Standard moderno del linguaggio
- **SDL2**: Windowing, input, audio
- **OpenGL**: Rendering moderno 3D/2D
- **GLEW**: Gestione estensioni OpenGL
- **Sistema Math Custom**: Libreria matematica interna
- **vcpkg**: Gestione delle dipendenze
- **CMake**: Build system
- **Catch2**: Framework di testing## Struttura del Progetto

```
src/
├── core/           # Sistema core (Window, Renderer, OpenGL)
│   ├── GLContext.h/.cpp    # Gestione contesto OpenGL
│   ├── Renderer.h/.cpp     # Renderer OpenGL
│   └── Window.h/.cpp       # Window con supporto OpenGL
├── math/           # Sistema matematico custom
│   ├── Vec.h               # Vettori n-dimensionali
│   ├── Matrix.h            # Matrici 4x4 per grafica
│   └── Rect.h              # Rettangoli axis-aligned
├── game/           # Logica di gioco
│   ├── state/      # Stati di gioco (Menu, Playing, Paused)
│   ├── input/      # Sistema di input con pattern Command
│   └── player_state/ # Stati del personaggio
├── gfx/            # Sistema grafico
│   └── Texture.h/.cpp      # Gestione texture OpenGL
└── tests/          # Test unitari
```

## Build e Dipendenze

### Requisiti
- **C++20** compatible compiler
- **vcpkg** per la gestione delle dipendenze
- **CMake** 3.21+

### Dipendenze vcpkg
```json
{
  "dependencies": [
    "sdl2", "sdl2-image", "sdl2-mixer", "sdl2-ttf",
    "opengl", "glew",
    "nlohmann-json"
  ]
}
```

### Sistema Matematico Interno

Il progetto implementa un **sistema matematico custom** completamente header-only:

- **`math::Vec<N,T>`**: Vettori n-dimensionali con supporto constexpr
- **`math::Matrix4<T>`**: Matrici 4x4 per trasformazioni grafiche 3D/2D
- **Funzioni di utilità**: `dot()`, `cross()`, `normalize()`, etc.
- **Trasformazioni**: Projection matrici (ortho, perspective), rotazioni, traslazioni
- **Compatibilità OpenGL**: Layout column-major e accesso diretto ai dati

### Compilazione
```bash
# Configurazione
cmake -G "MinGW Makefiles" -S . -B ./build -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=path/to/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-mingw-static

# Build
cmake --build ./build --config Debug

# Esecuzione
./build/sdl_app.exe
```

## Controlli

- **Menu**: ↑/↓ per navigare, Enter per selezionare
- **Gioco**:
  - **ESC**: Pausa/Menu
  - **Input configurabili**: Vedere `data/keybindings.json`

## Migrazione da GLM al Sistema Math Custom

Il progetto ora utilizza un **sistema matematico completamente personalizzato** invece di GLM:

### Vantaggi del Sistema Custom:
- **🎯 Tailored**: Progettato specificamente per le esigenze del progetto
- **📦 Zero dipendenze**: Nessuna libreria esterna per la matematica
- **⚡ Header-only**: Compilazione veloce e facile integrazione
- **🔧 Controllo totale**: Possibilità di ottimizzazioni specifiche
- **📚 Educativo**: Comprensione completa dell'algebra lineare

### API del Sistema Math:
```cpp
// Vettori
math::Vec3f position(1.0f, 2.0f, 3.0f);
math::Vec4f color(1.0f, 0.0f, 0.0f, 1.0f);

// Matrici
math::Mat4 projection = math::Mat4::ortho(0, 800, 600, 0, -1, 1);
math::Mat4 view = math::Mat4::lookAt(eye, center, up);
math::Mat4 model = math::Mat4::translation(x, y, z) * math::Mat4::scale(2.0f);

// Trasformazioni
math::Vec3f result = matrix * vector;
```

## Unit Testing

Il progetto include un **sistema completo di unit testing** per garantire la qualità del codice:

### Framework di Testing
- **Catch2**: Framework moderno per unit testing in C++
- **Copertura completa**: 112 asserzioni in 9 test case per il sistema matematico
- **CI/CD**: Test automatici ad ogni push su GitHub Actions

### Esecuzione dei Test
```bash
# Build dei test
cmake --build build --target sdl_appTests

# Esecuzione
./build/sdl_appTests.exe
```

### Test Coverage del Sistema Math
- **Costruzione matrici**: Default, diagonale, copia
- **Operazioni aritmetiche**: Addizione, sottrazione, moltiplicazione
- **Trasformazioni**: Scaling, rotazione (X/Y/Z), traslazione
- **Proiezioni**: Ortografica e prospettica
- **Utility**: Trasposta, operatori di confronto
- **Edge cases**: Valori zero, grandi, negativi
- **Trasformazioni combinate**: Pipeline complete

I test garantiscono la correttezza delle convenzioni OpenGL (coordinate right-handed, matrici column-major, rotazioni counter-clockwise).