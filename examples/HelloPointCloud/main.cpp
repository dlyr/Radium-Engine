// Include Radium base application and its simple Gui
#include <Gui/BaseApplication.hpp>
#include <Gui/RadiumWindow/SimpleWindow.hpp>
#include <Gui/RadiumWindow/SimpleWindowFactory.hpp>
#include <Gui/Utils/KeyMappingManager.hpp>
#include <Gui/Viewer/Viewer.hpp>

#include <Ponca/Ponca>
#include <Ponca/SpatialPartitioning>
#include <iostream>
#include <vector>

// include the Engine/entity/component interface
#include <Core/Geometry/MeshPrimitives.hpp>
#include <Engine/Scene/EntityManager.hpp>
#include <Engine/Scene/GeometryComponent.hpp>
#include <Engine/Scene/GeometrySystem.hpp>

#include <IO/TinyPlyLoader/TinyPlyFileLoader.hpp>

#include <QEvent>
#include <QTimer>

using PoncaPoint     = Ponca::PointPosition<Scalar, 3>;
using PoncaKdTree    = Ponca::KdTreeDense<PoncaPoint>;
using PointContainer = PoncaKdTree::PointContainer;

int main( int argc, char* argv[] ) {
    //! [Creating the application]
    Ra::Gui::BaseApplication app( argc, argv );
    glbinding::Version glVersion { 4, 4 };
    app.initialize( Ra::Gui::SimpleWindowFactory {}, glVersion );
    // app.initialize( DemoWindowFactory {}, glVersion );

    /*std::vector<DataPoint> points;

    points.emplace_back( DataPoint::VectorType( 0.f, 0.f, 0.f ) );
    points.emplace_back( DataPoint::VectorType( 1.f, 0.f, 0.f ) );
    points.emplace_back( DataPoint::VectorType( 0.f, 1.f, 0.f ) );
    points.emplace_back( DataPoint::VectorType( 0.f, 0.f, 1.f ) );

    Ponca::KdTreeDense<DataPoint> kdtree( points );


    std::cout << "[Ponca] KdTree built with " << kdtree.pointCount() << " points and "
              << kdtree.nodeCount() << " nodes." << std::endl;
*/
    app.addRadiumMenu();
    //! [Creating the application]

    //! [Verifying the OpenGL version available to the engine]
    if ( glVersion != app.m_engine->getOpenGLVersion() ) {
        LOG( Ra::Core::Utils::logWARNING )
            << "OpenGL version mismatch : requested " << glVersion.toString() << " -- available "
            << app.m_engine->getOpenGLVersion().toString() << std::endl;
    }
    //! [Verifying the OpenGL version available to the engine]

    //! [Create the engine entity for the point cloud]
    auto e = app.m_engine->getEntityManager()->createEntity( "point cloud" );

    Ra::Engine::Scene::PointCloudComponent* c;
    const bool stage1 = false;

    if ( stage1 ) {
        // stage 1
        // create point cloud by hand
        Ra::Core::Geometry::PointCloud pointCloud;
        pointCloud.setVertices( { Ra::Core::Vector3( 0.0, 0.0, 0.0 ),
                                  Ra::Core::Vector3( 1.0, 0.0, 0.0 ),
                                  Ra::Core::Vector3( 0.0, 1.0, 0.0 ),
                                  Ra::Core::Vector3( 0.0, 0.0, 1.0 ) } );
        c = new Ra::Engine::Scene::PointCloudComponent(
            "manual point cloud", e, std::move( pointCloud ) );
    }
    else {

        // load point cloud from file
        const std::string filename = "/home/jcai/Documents/pointcloud_50k/cow_50000 - Cloud.ply";

        Ra::IO::TinyPlyFileLoader loader;
        auto fileData     = loader.loadFile( filename );
        auto geometryData = fileData->getGeometryData()[0];

        c = new Ra::Engine::Scene::PointCloudComponent( "loaded point cloud", e, geometryData );
    }

    const auto& vertices = c->getGeometry()->getCoreGeometry().vertices();
    PoncaKdTree kdtree( vertices );

    if ( !kdtree.valid() ) { return 1; }

    const PoncaKdTree::IndexType queryIndex = 0;
    const PoncaKdTree::IndexType k          = 10;

    std::cout << "[Ponca] KdTree built with " << kdtree.pointCount() << " points and "
              << kdtree.nodeCount() << " nodes." << std::endl;

    std::cout << "[Ponca] " << k << " nearest neighbors of point " << queryIndex << " : ";

    PoncaKdTree::IndexType firstNeighbor = -1;
    PoncaKdTree::IndexType neighborCount = 0;

    for ( const auto neighborIndex : kdtree.kNearestNeighbors( queryIndex, k ) ) {
        std::cout << neighborIndex << " ";
        ++neighborCount;
    }
    std::cout << std::endl;
    if ( neighborCount == 0 || firstNeighbor < 0 ) { return 1; }




    c->setSplatSize( 0.01f );

    //! [Register the entity/component association to the geometry system ]
    auto geometrySystem = app.m_engine->getSystem( "GeometrySystem" );
    geometrySystem->addComponent( e, c );
    //! [Register the entity/component association to the geometry system ]
    //! [Tell the window that something is to be displayed]
    app.m_mainWindow->prepareDisplay();
    //! [Tell the window that something is to be displayed]

    app.getViewer()->addCustomAction(
        "SPLAT_UP",
        Ra::Gui::KeyMappingManager::createEventBindingFromStrings( "", "", "Key_U" ),
        [&c]( QEvent* event ) {
            if ( event->type() == QEvent::KeyPress && c != nullptr ) {
                c->setSplatSize( c->getSplatSize() + 0.0015f );
            }
        } );

    app.getViewer()->addCustomAction(
        "SPLAT_DOWN",
        Ra::Gui::KeyMappingManager::createEventBindingFromStrings( "", "", "Key_D" ),
        [&c]( QEvent* event ) {
            if ( event->type() == QEvent::KeyPress && c != nullptr ) {
                c->setSplatSize( c->getSplatSize() - 0.0015f );
            }
        } );

    app.getViewer()->addCustomAction(
        "SPLAT_RESET",
        Ra::Gui::KeyMappingManager::createEventBindingFromStrings( "", "", "Key_O" ),
        [&c]( QEvent* event ) {
            if ( event->type() == QEvent::KeyPress && c != nullptr ) { c->setSplatSize( 0.01f ); }
        } );

    return app.exec();
}
