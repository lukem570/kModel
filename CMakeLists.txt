cmake_minimum_required(VERSION 3.25)
project(kmodel VERSION 0.2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(GNUInstallDirs)

# --- Qt6 + Kirigami -----------------------------------------------------------
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Quick QuickControls2 ShaderTools)
find_package(Qt6GuiPrivate REQUIRED)
find_package(KF6Kirigami REQUIRED)

qt_standard_project_setup()

# --- Vulkan SDK / Slang (optional for now) ------------------------------------
set(VULKAN_SDK_PATH "$ENV{VULKAN_SDK}" CACHE PATH "Path to Vulkan SDK")
find_program(SLANGC slangc HINTS "${VULKAN_SDK_PATH}/bin")

# Custom function: compile .slang -> .spv
function(compile_slang TARGET)
    set(SHADER_OUTPUT_DIR "${CMAKE_BINARY_DIR}/shaders")
    file(MAKE_DIRECTORY "${SHADER_OUTPUT_DIR}")

    foreach(SHADER_SRC ${ARGN})
        get_filename_component(SHADER_NAME "${SHADER_SRC}" NAME_WE)
        # Determine stage from filename convention: name.vert.slang / name.frag.slang
        string(REGEX MATCH "\\.(vert|frag|comp)" STAGE_MATCH "${SHADER_SRC}")
        string(SUBSTRING "${STAGE_MATCH}" 1 -1 STAGE) # drop leading dot

        set(SPV_OUTPUT "${SHADER_OUTPUT_DIR}/${SHADER_NAME}.${STAGE}.spv")
        add_custom_command(
            OUTPUT "${SPV_OUTPUT}"
            COMMAND "${SLANGC}"
                    -target spirv
                    -stage "${STAGE}"
                    -entry main
                    -o "${SPV_OUTPUT}"
                    "${CMAKE_SOURCE_DIR}/${SHADER_SRC}"
            DEPENDS "${CMAKE_SOURCE_DIR}/${SHADER_SRC}"
            COMMENT "Compiling Slang shader: ${SHADER_SRC} -> ${SPV_OUTPUT}"
            VERBATIM
        )
        list(APPEND SPV_OUTPUTS "${SPV_OUTPUT}")
    endforeach()

    add_custom_target(${TARGET}_shaders ALL DEPENDS ${SPV_OUTPUTS})
    add_dependencies(${TARGET} ${TARGET}_shaders)
endfunction()

add_subdirectory(src)
