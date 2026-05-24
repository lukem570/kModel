# kModel

A KDE-native 3D model viewer with Vulkan rendering.

## Features

- STL file viewing (binary and ASCII)
- Vulkan-based rendering via Qt RHI
- PBR-style lighting with 3-point light setup
- Environment reflections with procedural sky dome
- ACES tone mapping
- Orbit camera with mouse controls
- Kirigami UI with file browser integration

## Screenshot

*Coming soon*

## Installation

### Arch Linux (AUR)

```bash
yay -S kmodel
```

### Building from source

**Dependencies:**

- Qt 6 (Base, Declarative, Quick, ShaderTools)
- KDE Frameworks 6 (Kirigami)
- Vulkan headers and loader
- CMake
- Extra CMake Modules

**Build:**

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
./build/src/kmodel
```

Or using the Makefile:

```bash
make run
```

## Controls

- **Left mouse drag** - Orbit camera
- **Scroll wheel** - Zoom in/out

## License

GPL-3.0-or-later
