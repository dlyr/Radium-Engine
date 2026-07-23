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
#include <Engine/Scene/SystemDisplay.hpp>

#include <IO/TinyPlyLoader/TinyPlyFileLoader.hpp>

#include <QEvent>
#include <QTimer>

using PoncaPoint     = Ponca::PointPosition<Scalar, 3>;
using PoncaKdTree    = Ponca::KdTreeDense<PoncaPoint>;
using PointContainer = PoncaKdTree::PointContainer;

/*struct KdTreeNodeBox
{
    PoncaKdTree::NodeIndexType nodeId;
    Ra::Core::Aabb box;
};

static void splitAabbFromNode( const PoncaKdTree::NodeType& node,
                               const Ra::Core::Aabb& parentBox,
                               int maxLevel = 1
                               ) {
    // pritn node box
    RA_DISPLAY_AABB( firstChildBox, Ra::Core::Utils::Color::Green() );

    if(maxLevel == 0 && !node.is_leaf()) return ;
    const int splitDim    = node.inner_split_dim();
    const Scalar splitVal = node.inner_split_value();

    Ra::Core::Aabb firstChildBox;
    Ra::Core::Aabb secondChildBox;

    firstChildBox = parentBox ;
    secondChildBox = parentBox ;

    firstChildBox.max()[splitDim] = splitVal;
    secondChildBox.min()[splitDim] = splitVal;

    RA_DISPLAY_AABB( firstChildBox, Ra::Core::Utils::Color::Green() );
    RA_DISPLAY_AABB( secondChildBox, Ra::Core::Utils::Color::Blue() );

    fristChlid = node.inner_first_child_id();
    fristChlid = node.inner_first_child_id();
    splitAabbFromNode(fristChlid, firstChildBox, maxLevel-1);
    splitAabbFromNode(secondChlid, secondChildBox, maxLevel-1);
} */

static void displayKdTreeBoxes( const PoncaKdTree& kdtree,
                                PoncaKdTree::NodeIndexType nodeId,
                                const Ra::Core::Aabb& nodeBox,
                                int maxLevel ) {
    const auto& nodes = kdtree.nodes();

    if ( static_cast<std::size_t>( nodeId ) >= nodes.size() ) { //(car nodeid ca viens de ponca,ptr un c signed l'autre c unsigned)
        return;
    }

    const auto& node = nodes[nodeId]; //NodeType& node = Base::m_bufs.nodes[node_id];

    RA_DISPLAY_AABB( nodeBox, Ra::Core::Utils::Color::Green() );

    if ( maxLevel == 0 || node.is_leaf() ) {
        return;
    }

    const int splitDim    = node.inner_split_dim();
    const Scalar splitVal = node.inner_split_value(); //node.configure_inner(aabb.center()[split_dim], ..., split_dim);

    Ra::Core::Aabb firstChildBox  = nodeBox;
    Ra::Core::Aabb secondChildBox = nodeBox;

    firstChildBox.max()[splitDim]  = splitVal;
    secondChildBox.min()[splitDim] = splitVal;

    const auto firstChildId = node.inner_first_child_id();

    displayKdTreeBoxes( kdtree, firstChildId, firstChildBox, maxLevel - 1 ); //(split method from ponca )buildRec(node.inner_first_child_id(), start, mid_id, level + 1); buildRec(node.inner_first_child_id() + 1, mid_id, end, level + 1);
    displayKdTreeBoxes( kdtree, firstChildId + 1, secondChildBox, maxLevel - 1 );
}

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

    Ra::Core::Aabb aabb;

    for ( const auto& v : vertices ) {
        aabb.extend( v );
    }

    //const int displayedLevel = 7;
    //displayKdTreeBoxes( kdtree, 0, aabb, displayedLevel );
    int currentLevel = 0;
    const int maxDisplayedLevel = 10;

    displayKdTreeBoxes( kdtree, 0, aabb, currentLevel );

    /*RA_DISPLAY_AABB( aabb, Ra::Core::Utils::Color::Red() );

    const auto& nodes = kdtree.nodes();

    if ( !nodes.empty() ) {
        const auto& root = nodes[0]; //root = root node = first node

        if ( !root.is_leaf() ) {
            const int splitDim    = root.inner_split_dim();
            const Scalar splitVal = root.inner_split_value();

            Ra::Core::Aabb leftBox  = aabb;
            Ra::Core::Aabb rightBox = aabb;

            leftBox.max()[splitDim]  = splitVal;
            rightBox.min()[splitDim] = splitVal;

            RA_DISPLAY_AABB( leftBox, Ra::Core::Utils::Color::Green() );
            RA_DISPLAY_AABB( rightBox, Ra::Core::Utils::Color::Blue() );
        } */

    //if ( !kdtree.valid() ) { return 1; }

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
    //if ( neighborCount == 0 || firstNeighbor < 0 ) { return 1; }




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

    app.getViewer()->addCustomAction(
        "KDTREE_LEVEL_UP",
        Ra::Gui::KeyMappingManager::createEventBindingFromStrings( "", "", "Key_Up" ),
        [&kdtree, &aabb, &currentLevel, maxDisplayedLevel]( QEvent* event ) {
            if ( event->type() == QEvent::KeyPress ) {
                if ( currentLevel < maxDisplayedLevel ) {
                    ++currentLevel;
                    displayKdTreeBoxes( kdtree, 0, aabb, currentLevel );
                }
            }
        } );

    app.getViewer()->addCustomAction(
        "KDTREE_LEVEL_DOWN",
        Ra::Gui::KeyMappingManager::createEventBindingFromStrings( "", "", "Key_Down" ),
        [&currentLevel]( QEvent* event ) {
            if ( event->type() == QEvent::KeyPress ) {
                if ( currentLevel > 0 ) {
                    --currentLevel;
                }
            }
        } );

    return app.exec();
}
