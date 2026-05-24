qt_add_executable(kmodel main.cpp)

set_source_files_properties(../resources/Main.qml PROPERTIES QT_RESOURCE_ALIAS Main.qml)
set_source_files_properties(../resources/WelcomePage.qml PROPERTIES QT_RESOURCE_ALIAS WelcomePage.qml)
set_source_files_properties(../resources/ViewportPage.qml PROPERTIES QT_RESOURCE_ALIAS ViewportPage.qml)

qt_add_qml_module(kmodel
    URI org.kde.kmodel
    VERSION 1.0
    RESOURCE_PREFIX /qt/qml
    QML_FILES
        ../resources/Main.qml
        ../resources/WelcomePage.qml
        ../resources/ViewportPage.qml
    SOURCES
        stlloader.h
        stlloader.cpp
        vulkanviewport.h
        vulkanviewport.cpp
)

qt_add_shaders(kmodel "kmodel_shaders"
    PREFIX "/shaders"
    BASE ../shaders
    FILES
        ../shaders/model.vert
        ../shaders/model.frag
)

target_link_libraries(kmodel PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::GuiPrivate
    Qt6::Widgets
    Qt6::Quick
    Qt6::QuickControls2
    KF6::Kirigami
)
