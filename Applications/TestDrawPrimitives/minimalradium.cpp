
#include <minimalradium.hpp>

#include <Core/Containers/MakeShared.hpp>
#include <Core/Geometry/MeshPrimitives.hpp>
#include <Core/Tasks/Task.hpp>
#include <Core/Tasks/TaskQueue.hpp>
#include <Core/Utils/Timer.hpp>
#include <Engine/Renderer/Material/BlinnPhongMaterial.hpp>
#include <Engine/Renderer/Mesh/Mesh.hpp>
#include <Engine/Renderer/RenderObject/RenderObject.hpp>
#include <Engine/Renderer/RenderObject/RenderObjectManager.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderConfigFactory.hpp>

#include <random>

/* This file contains a minimal radium/qt application which shows the
classic "Spinning Cube" demo. */

/// This is a very basic component which holds a spinning cube.

MinimalComponent::MinimalComponent( Ra::Engine::Entity* entity ) :
    Ra::Engine::Component( "Minimal Component", entity ) {}

/// This function is called when the component is properly
/// setup, i.e. it has an entity.
void MinimalComponent::initialize() {

    using namespace Ra::Core;
    using namespace Ra::Engine;

    ///
    // basic render technique associated with all object here, they use per vertex kd.
    RenderTechnique rt;
    auto mat              = make_shared<BlinnPhongMaterial>( "Default Material" );
    mat->m_hasPerVertexKd = true;
    rt.setMaterial( mat );
    auto builder = Ra::Engine::EngineRenderTechniques::getDefaultTechnique( "BlinnPhong" );
    builder.second( rt, false );

    //// setup ////
    Scalar colorBoost = 3_ra; /// since simple primitive are ambient only, boost
                              /// their color
    Scalar cellSize = 0.25_ra;
    Vector3 cellCorner{-1_ra, 0_ra, 0.25_ra};
    Scalar offset{0.05_ra};
    Vector3 offsetVec{offset, offset, offset};
    std::random_device rd;    // Will be used to obtain a seed for the random number engine
    std::mt19937 gen( rd() ); // Standard mersenne_twister_engine seeded with rd()
    std::uniform_real_distribution<Scalar> dis015( 0_ra, cellSize - 2_ra * offset );
    std::uniform_real_distribution<Scalar> dis01( 0_ra, 1_ra );
    std::uniform_real_distribution<Scalar> dis11( -1_ra, 1_ra );
    std::uniform_int_distribution<uint> disInt( 0, 128 );

    //// GRID ////
    addRenderObject(
        RenderObject::createRenderObject( "test_grid",
                                          this,
                                          RenderObjectType::Geometry,
                                          DrawPrimitives::Grid( Vector3::Zero(),
                                                                Vector3::UnitX(),
                                                                Vector3::UnitZ(),
                                                                Utils::Color::Grey( 0.6f ),
                                                                cellSize,
                                                                8 ),
                                          rt ) );

    //// CUBES ////
    std::shared_ptr<Ra::Engine::Mesh> cube1( new Ra::Engine::Mesh( "Cube" ) );
    cube1->loadGeometry( Geometry::makeSharpBox( {0.1f, 0.1f, 0.1f} ) );
    cube1->getCoreGeometry().addAttrib(
        "in_color", Vector4Array{cube1->getNumVertices(), Utils::Color::Green()} );

    auto renderObject1 =
        RenderObject::createRenderObject( "CubeRO", this, RenderObjectType::Geometry, cube1, rt );
    renderObject1->setLocalTransform(
        Transform{Translation( Vector3( 3 * cellSize, 0_ra, 0_ra ) )} );

    addRenderObject( renderObject1 );

    // another cube
    std::shared_ptr<Ra::Engine::Mesh> cube2( new Ra::Engine::Mesh( "Cube" ) );
    cube2->loadGeometry( Geometry::makeSharpBox( {0.1f, 0.1f, 0.1f} ) );
    cube2->getCoreGeometry().addAttrib(
        "colour", Vector4Array{cube2->getNumVertices(), Utils::Color::Red()} );

    cube2->setTranslation( "colour", "in_color" );
    auto renderObject2 =
        RenderObject::createRenderObject( "CubeRO", this, RenderObjectType::Geometry, cube2, rt );
    renderObject2->setLocalTransform(
        Transform{Translation( Vector3( 4 * cellSize, 0_ra, 0_ra ) )} );

    addRenderObject( renderObject2 );

    //// POINTS ////
    cellCorner = {-1_ra, 0_ra, 0.0_ra};
    addRenderObject( RenderObject::createRenderObject(
        "test_point",
        this,
        RenderObjectType::Geometry,
        DrawPrimitives::Point( cellCorner, colorBoost * Utils::Color{0_ra, 1_ra, 0.3_ra} ),
        rt ) );

    for ( int i = 0; i < 10; ++i )
    {
        Vector3 randomVec{cellCorner + offsetVec +
                          Vector3{dis015( gen ), dis015( gen ), dis015( gen )}};
        Color randomCol{dis01( gen ), dis01( gen ), dis01( gen )};
        addRenderObject( RenderObject::createRenderObject(
            "test_point",
            this,
            RenderObjectType::Geometry,
            DrawPrimitives::Point( randomVec, colorBoost * randomCol, 0.03_ra ),
            rt ) );
    }

    //// LINES ////
    cellCorner = {-0.75_ra, 0_ra, 0.0_ra};
    addRenderObject( RenderObject::createRenderObject(
        "test_line",
        this,
        RenderObjectType::Geometry,
        DrawPrimitives::Line( cellCorner,
                              cellCorner + Vector3{0_ra, 0.4_ra, 0_ra},
                              colorBoost * Utils::Color::Red() ),
        rt ) );

    for ( int i = 0; i < 20; ++i )
    {
        Vector3 randomVec1{cellCorner + offsetVec +
                           Vector3{dis015( gen ), dis015( gen ), dis015( gen )}};
        Vector3 randomVec2{cellCorner + offsetVec +
                           Vector3{dis015( gen ), dis015( gen ), dis015( gen )}};
        Color randomCol{dis01( gen ), dis01( gen ), dis01( gen )};

        addRenderObject( RenderObject::createRenderObject(
            "test_line",
            this,
            RenderObjectType::Geometry,
            DrawPrimitives::Line( randomVec1, randomVec2, colorBoost * randomCol ),
            rt ) );
    }

    //// VECTOR ////
    cellCorner = {-0.5_ra, 0_ra, 0.0_ra};
    addRenderObject( RenderObject::createRenderObject(
        "test_vector",
        this,
        RenderObjectType::Geometry,
        DrawPrimitives::Vector(
            cellCorner, Vector3{0_ra, 0.5_ra, 0_ra}, colorBoost * Utils::Color::Blue() ),
        rt ) );

    for ( int i = 0; i < 10; ++i )
    {
        Vector3 randomVec1{cellCorner + offsetVec +
                           Vector3{dis015( gen ), dis015( gen ), dis015( gen )}};
        Vector3 randomVec2{cellCorner + offsetVec +
                           Vector3{dis015( gen ), dis015( gen ), dis015( gen )}};
        Color randomCol{dis01( gen ), dis01( gen ), dis01( gen )};

        addRenderObject( RenderObject::createRenderObject(
            "test_vector",
            this,
            RenderObjectType::Geometry,
            DrawPrimitives::Vector( randomVec1, randomVec2 - randomVec1, colorBoost * randomCol ),
            rt ) );
    }

    /// RAY ////
    cellCorner = {-0.25_ra, 0_ra, 0.0_ra};
    addRenderObject( RenderObject::createRenderObject(
        "test_ray",
        this,
        RenderObjectType::Geometry,
        DrawPrimitives::Ray(
            {cellCorner, {0_ra, 1_ra, 0_ra}}, colorBoost * Utils::Color::Yellow(), cellSize ),
        rt ) );

    //// TRIANGLES ////
    cellCorner = {0_ra, 0_ra, 0.0_ra};
    addRenderObject( RenderObject::createRenderObject(
        "test_triangle",
        this,
        RenderObjectType::Geometry,
        DrawPrimitives::Triangle( cellCorner + Vector3{-0.01_ra, 0.0_ra, 0.0_ra},
                                  cellCorner + Vector3{+0.01_ra, 0.0_ra, 0.0_ra},
                                  cellCorner + Vector3{+0.0_ra, 0.02_ra, 0.0_ra},
                                  colorBoost * Utils::Color::White(),
                                  true ),
        rt ) );

    cellCorner = {0_ra + 0.125_ra, 0_ra, 0.0_ra};
    addRenderObject( RenderObject::createRenderObject(
        "test_triangle",
        this,
        RenderObjectType::Geometry,
        DrawPrimitives::Triangle( cellCorner + Vector3{-0.071_ra, 0.0_ra, 0.0_ra},
                                  cellCorner + Vector3{+0.071_ra, 0.0_ra, 0.0_ra},
                                  cellCorner + Vector3{+0.0_ra, 0.2_ra, 0.0_ra},
                                  colorBoost * Utils::Color::Green(),
                                  true ),
        rt ) );

    for ( int i = 0; i < 10; ++i )
    {

        addRenderObject( RenderObject::createRenderObject(
            "test_triangle_wire",
            this,
            RenderObjectType::Geometry,
            DrawPrimitives::Triangle(
                cellCorner + Vector3{-0.071_ra, 0.0_ra, Scalar( i ) / 10_ra * cellSize},
                cellCorner + Vector3{+0.071_ra, 0.0_ra, Scalar( i ) / 10_ra * cellSize},
                cellCorner + Vector3{+0.0_ra, 0.2_ra, Scalar( i ) / 10_ra * cellSize},
                colorBoost * Utils::Color::White() * Scalar( i ) / 10_ra,
                false ),
            rt ) );
    }

    /*
    addRenderObject(
        RenderObject::createRenderObject( "test_quad_strip",
                                          this,
                                          RenderObjectType::Geometry,
                                          DrawPrimitives::QuadStrip( {0.1_ra, 0.0_ra, -1.0_ra},
                                                                     {0.3_ra, 0.0_ra, 0.1_ra},
                                                                     {-0.1_ra, 0.3_ra, 0.1_ra},
                                                                     6,
                                                                     {0.7_ra, 0.2_ra, 0.9_ra} ) ) );
    */

    //// CIRCLE ////
    cellCorner = {0.25_ra, 0_ra, 0.0_ra};
    addRenderObject( RenderObject::createRenderObject(
        "test_cirlce",
        this,
        RenderObjectType::Geometry,
        DrawPrimitives::Circle( cellCorner,
                                {0_ra, 0_ra, 1_ra},
                                cellSize / 8_ra,
                                64,
                                colorBoost * Utils::Color::White() ),
        rt ) );

    uint end = 8;
    for ( uint j = 0; j < end; ++j )
        for ( uint i = 0; i < end; ++i )
        {
            Vector3 randomVec1{cellCorner + offsetVec +
                               Vector3{Scalar( j ) / end * ( cellSize - 2 * offset ),
                                       offset,
                                       Scalar( i ) / end * ( cellSize - 2 * offset )}};
            Vector3 randomVec2{Vector3{Scalar( i ), Scalar( j ), 10_ra}};
            randomVec2.normalize();
            Color randomCol{dis01( gen ), dis01( gen ), dis01( gen )};
            Scalar randomRadius{Scalar( end / 2 + i ) / Scalar( 2 * end ) * cellSize / 8_ra};
            uint randomSubdiv{3 + j * end + i};

            addRenderObject( RenderObject::createRenderObject(
                "test_circle",
                this,
                RenderObjectType::Geometry,
                DrawPrimitives::Circle(
                    randomVec1, randomVec2, randomRadius, randomSubdiv, colorBoost * randomCol ),
                rt ) );
        }

    /*
            addRenderObject( RenderObject::createRenderObject(
    "test_ray",
    this,
    RenderObjectType::Geometry,
    DrawPrimitives::CircleArc( const Core::Vector3& center,
                                         const Core::Vector3& normal,
                                         Scalar radius,
                                         Scalar angle,
                                         uint segments,
                                         const Core::Utils::Color& color );
            addRenderObject( RenderObject::createRenderObject(
    "test_ray",
    this,
    RenderObjectType::Geometry,
    DrawPrimitives::Sphere( const Core::Vector3& center,
                                      Scalar radius,
                                      const Core::Utils::Color& color );
            addRenderObject( RenderObject::createRenderObject(
    "test_ray",
    this,
    RenderObjectType::Geometry,
    DrawPrimitives::Capsule( const Core::Vector3& p1,
                                       const Core::Vector3& p2,
                                       Scalar radius,
                                       const Core::Utils::Color& color );
            addRenderObject( RenderObject::createRenderObject(
    "test_ray",
    this,
    RenderObjectType::Geometry,
    DrawPrimitives::Disk( const Core::Vector3& center,
                                    const Core::Vector3& normal,
                                    Scalar radius,
                                    uint segments,
                                    const Core::Utils::Color& color );
            addRenderObject( RenderObject::createRenderObject(
    "test_ray",
    this,
    RenderObjectType::Geometry,
    DrawPrimitives::Normal( const Core::Vector3& point,
                                      const Core::Vector3& normal,
                                      const Core::Utils::Color& color,
                                      Scalar scale = 0.1f );
            addRenderObject( RenderObject::createRenderObject(
    "test_ray",
    this,
    RenderObjectType::Geometry,
    DrawPrimitives::Frame( const Core::Transform& frameFromEntity,
        Scalar scale = 0.1f );

            addRenderObject( RenderObject::createRenderObject(
    "test_ray",
    this,
    RenderObjectType::Geometry,
    DrawPrimitives::Grid( const Core::Vector3& center,
                                    const Core::Vector3& x,
                                    const Core::Vector3& y,
                                    const Core::Utils::Color& color,
                                    Scalar cellSize = 1.f,
                                    uint res        = 10 );

            addRenderObject( RenderObject::createRenderObject(
    "test_ray",
    this,
    RenderObjectType::Geometry,
    DrawPrimitives::AABB( const Core::Aabb& aabb, const
        Core::Utils::Color& color );

            addRenderObject( RenderObject::createRenderObject(
    "test_ray",
    this,
    RenderObjectType::Geometry,
    DrawPrimitives::OBB( const Core::Geometry::Obb& obb, const
        Core::Utils::Color& color );

            addRenderObject( RenderObject::createRenderObject(
    "test_ray",
    this,
    RenderObjectType::Geometry,
    DrawPrimitives::Spline( const Core::Geometry::Spline<3, 3>&
        spline, uint pointCount, const Core::Utils::Color& color, Scalar scale = 1.0f
    );*/
}

/// This system will be added to the engine. Every frame it will
/// add a task to be executed, calling the spin function of the component.
void MinimalSystem::generateTasks( Ra::Core::TaskQueue* q, const Ra::Engine::FrameInfo& info ) {
    // We check that our component is here.
    CORE_ASSERT( m_components.size() == 1, "System incorrectly initialized" );
    //    MinimalComponent* c = static_cast<MinimalComponent*>( m_components[0].second );
}

void MinimalSystem::addComponent( Ra::Engine::Entity* ent, MinimalComponent* comp ) {
    registerComponent( ent, comp );
}
