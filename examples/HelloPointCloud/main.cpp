// Include Radium base application and its simple Gui
#include <Gui/BaseApplication.hpp>
#include <Gui/RadiumWindow/SimpleWindowFactory.hpp>

// include the Engine/entity/component interface
#include <Core/Geometry/MeshPrimitives.hpp>
#include <Engine/Scene/EntityManager.hpp>
#include <Engine/Scene/GeometryComponent.hpp>
#include <Engine/Scene/GeometrySystem.hpp>

#include <QTimer>

int main( int argc, char* argv[] ) {
    //! [Creating the application]
    Ra::Gui::BaseApplication app( argc, argv );
    glbinding::Version glVersion { 4, 4 };
    app.initialize( Ra::Gui::SimpleWindowFactory {}, glVersion );
    app.addRadiumMenu();
    //! [Creating the application]

    //! [Verifying the OpenGL version available to the engine]
    if ( glVersion != app.m_engine->getOpenGLVersion() ) {
        LOG( Ra::Core::Utils::logWARNING )
            << "OpenGL version mismatch : requested " << glVersion.toString() << " -- available "
            << app.m_engine->getOpenGLVersion().toString() << std::endl;
    }
    //! [Verifying the OpenGL version available to the engine]


    // todo
    // stage 1
    // create point cloud by hand
    Ra::Core::Geometry::PointCloud pointCloud;
    pointCloud.setVertices( { Ra::Core::Vector3( 0.0, 0.0, 0.0 ),
                              Ra::Core::Vector3( 1.0, 0.0, 0.0 ),
                              Ra::Core::Vector3( 0.0, 1.0, 0.0 ),
                              Ra::Core::Vector3( 0.0, 0.0, 1.0 ) } );





    // stage 2
    // load point cloud from file

    //! [Create the engine entity for the point cloud]
    auto e = app.m_engine->getEntityManager()->createEntity( "point cloud" );

    //todo
    // create point cloud component
    // auto c =
    auto c = new Ra::Engine::Scene::PointCloudComponent( "manual point cloud",
                                                         e,
                                                         std::move( pointCloud ) );
    c->setSplatSize( 0.05f );
    //! [Register the entity/component association to the geometry system ]
    auto geometrySystem = app.m_engine->getSystem( "GeometrySystem" );
        geometrySystem->addComponent( e, c );
    //! [Register the entity/component association to the geometry system ]

    //! [Tell the window that something is to be displayed]
    app.m_mainWindow->prepareDisplay();
    //! [Tell the window that something is to be displayed]

    return app.exec();
}
