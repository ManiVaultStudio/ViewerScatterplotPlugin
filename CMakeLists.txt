cmake_minimum_required(VERSION 3.17)

set(PROJECT "ViewerScatterplotPlugin")

PROJECT(${PROJECT})

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3 /DWIN32 /EHsc /MP /permissive- /Zc:__cplusplus")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:LIBCMT")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
endif(MSVC)

file(TO_CMAKE_PATH $ENV{HDPS_INSTALL_DIR} INSTALL_DIR)

find_package(Qt6 COMPONENTS Widgets WebEngineWidgets OpenGL OpenGLWidgets REQUIRED)

set(PLUGIN
    src/Common.h
    src/ViewerScatterplotPlugin.h
    src/ViewerScatterplotPlugin.cpp
)

set(UI
    src/ViewerScatterplotWidget.h
    src/ViewerScatterplotWidget.cpp
    src/ExportImageDialog.h
    src/ExportImageDialog.cpp
)

set(Actions
    src/ColoringAction.h
    src/ColoringAction.cpp
    src/DensityPlotAction.h
    src/DensityPlotAction.cpp
    src/ManualClusteringAction.h
    src/ManualClusteringAction.cpp
    src/MiscellaneousAction.h
    src/MiscellaneousAction.cpp
    src/PlotAction.h
    src/PlotAction.cpp
    src/PluginAction.h
    src/PluginAction.cpp
    src/PointPlotAction.h
    src/PointPlotAction.cpp
    src/PositionAction.h
    src/PositionAction.cpp
    src/RenderModeAction.h
    src/RenderModeAction.cpp
    src/ScalarAction.h
    src/ScalarAction.cpp
    src/ScalarSourceAction.h
    src/ScalarSourceAction.cpp
    src/SelectionAction.h
    src/SelectionAction.cpp
    src/SettingsAction.h
    src/SettingsAction.cpp
    src/SubsetAction.h
    src/SubsetAction.cpp
    src/ExportImageAction.h
    src/ExportImageAction.cpp
    src/LoadedDatasetsAction.h
    src/LoadedDatasetsAction.cpp
)

set(Models
    src/ColorSourceModel.h
    src/ColorSourceModel.cpp
    src/ScalarSourceModel.h
    src/ScalarSourceModel.cpp
)

set(SHADERS
    res/shaders/SelectionTool.frag
    res/shaders/SelectionTool.vert
)

set(AUX
    res/Resources.qrc
    src/ViewerScatterplotPlugin.json
)

set(SOURCES ${PLUGIN} ${UI} ${Actions} ${Models})

source_group(Plugin FILES ${PLUGIN})
source_group(UI FILES ${UI})
source_group(Actions FILES ${Actions})
source_group(Models FILES ${Models})
source_group(Shaders FILES ${SHADERS})
source_group(Aux FILES ${AUX})

add_library(${PROJECT} SHARED ${SOURCES} ${SHADERS} ${AUX})

target_include_directories(${PROJECT} PRIVATE "${INSTALL_DIR}/$<CONFIGURATION>/include/")

target_compile_features(${PROJECT} PRIVATE cxx_std_17)

target_link_libraries(${PROJECT} PRIVATE Qt6::Widgets)
target_link_libraries(${PROJECT} PRIVATE Qt6::WebEngineWidgets)
target_link_libraries(${PROJECT} PRIVATE Qt6::OpenGL)
target_link_libraries(${PROJECT} PRIVATE Qt6::OpenGLWidgets)

set(HDPS_LINK_PATH "${INSTALL_DIR}/$<CONFIGURATION>/lib")
set(PLUGIN_LINK_PATH "${INSTALL_DIR}/$<CONFIGURATION>/$<IF:$<CXX_COMPILER_ID:MSVC>,lib,Plugins>")
set(HDPS_LINK_SUFFIX $<IF:$<CXX_COMPILER_ID:MSVC>,${CMAKE_LINK_LIBRARY_SUFFIX},${CMAKE_SHARED_LIBRARY_SUFFIX}>)

set(HDPS_LINK_LIBRARY "${HDPS_LINK_PATH}/${CMAKE_SHARED_LIBRARY_PREFIX}HDPS_Public${HDPS_LINK_SUFFIX}")
set(POINTDATA_LINK_LIBRARY "${PLUGIN_LINK_PATH}/${CMAKE_SHARED_LIBRARY_PREFIX}PointData${HDPS_LINK_SUFFIX}") 
set(CLUSTERDATA_LINK_LIBRARY "${PLUGIN_LINK_PATH}/${CMAKE_SHARED_LIBRARY_PREFIX}ClusterData${HDPS_LINK_SUFFIX}") 

target_link_libraries(${PROJECT} PRIVATE "${HDPS_LINK_LIBRARY}")
target_link_libraries(${PROJECT} PRIVATE "${POINTDATA_LINK_LIBRARY}")
target_link_libraries(${PROJECT} PRIVATE "${CLUSTERDATA_LINK_LIBRARY}")

install(TARGETS ${PROJECT}
    RUNTIME DESTINATION Plugins COMPONENT PLUGINS # Windows .dll
    LIBRARY DESTINATION Plugins COMPONENT PLUGINS # Linux/Mac .so
)

add_custom_command(TARGET ${PROJECT} POST_BUILD
    COMMAND "${CMAKE_COMMAND}"
        --install ${CMAKE_CURRENT_BINARY_DIR}
        --config $<CONFIGURATION>
        --prefix ${INSTALL_DIR}/$<CONFIGURATION>
)

# Automatically set the debug environment (command + working directory) for MSVC
if(MSVC)
    set_property(TARGET ${PROJECT} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY $<IF:$<CONFIG:DEBUG>,${INSTALL_DIR}/debug,${INSTALL_DIR}/release>)
    set_property(TARGET ${PROJECT} PROPERTY VS_DEBUGGER_COMMAND $<IF:$<CONFIG:DEBUG>,${INSTALL_DIR}/debug/HDPS.exe,${INSTALL_DIR}/release/HDPS.exe>)
endif()
