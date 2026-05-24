qt_add_executable(kmodel main.cpp)

qt_add_qml_module(kmodel
    URI org.kde.kmodel
    VERSION 1.0
    QML_FILES
        ../resources/Main.qml
        ../resources/WelcomePage.qml
    SOURCES
        stlloader.h
        stlloader.cpp
)

target_link_libraries(kmodel PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Quick
    Qt6::QuickControls2
    KF6::Kirigami
)

# When we add Vulkan rendering, uncomment:
# find_package(Qt6 REQUIRED COMPONENTS Gui)  # for Vulkan support
# find_package(Vulkan REQUIRED)
# target_link_libraries(kmodel PRIVATE Vulkan::Vulkan)
# compile_slang(kmodel shaders/model.vert.slang shaders/model.frag.slang)
