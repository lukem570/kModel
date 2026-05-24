# kModel — 3D Model Viewer for KDE

## Project Overview
A KDE-native 3D model viewer. Currently supports STL files, with plans to expand to additional formats.

## Tech Stack
- **UI**: Qt6 + Kirigami (KDE Frameworks 6)
- **Rendering**: Vulkan (via Qt6 Vulkan integration)
- **Shaders**: Slang (compiled with `slangc` from Vulkan SDK)
- **Build**: CMake

## Build

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
./build/src/kmodel
```

## Environment
- Vulkan SDK: `$VULKAN_SDK` (currently `/home/lukem/vulkan/1.3.296.0/x86_64`)
- Slang compiler: `$VULKAN_SDK/bin/slangc`

## Project Structure
```
src/           — Application source code
shaders/       — Slang shader sources (.slang)
resources/     — QML UI files and Qt resources
build/         — Build output (gitignored)
```

## Conventions
- Use KDE/Qt naming conventions (camelCase for methods, PascalCase for classes)
- Kirigami components for all UI chrome
- QML for UI, C++ for rendering and model loading
