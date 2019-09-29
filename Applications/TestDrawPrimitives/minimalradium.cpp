
#include <minimalradium.hpp>

#include <Core/Geometry/MeshPrimitives.hpp>
#include <Core/Tasks/Task.hpp>
#include <Core/Tasks/TaskQueue.hpp>
#include <Core/Utils/Timer.hpp>

#include <Engine/Renderer/Mesh/Mesh.hpp>

#include <Core/Containers/MakeShared.hpp>
#include <Engine/Renderer/Material/BlinnPhongMaterial.hpp>
#include <Engine/Renderer/RenderObject/RenderObject.hpp>
#include <Engine/Renderer/RenderObject/RenderObjectManager.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderConfigFactory.hpp>

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

    auto config = ShaderConfigurationFactory::getConfiguration( "Plain" );
    auto mat    = Ra::Core::make_shared<BlinnPhongMaterial>( "Default material" );
    RenderTechnique rt;
    rt.setMaterial( mat );
    rt.setConfiguration( config );

    std::shared_ptr<Ra::Engine::Mesh> display( new Ra::Engine::Mesh( "Cube" ) );
    display->loadGeometry( Geometry::makeSharpBox( {0.1f, 0.1f, 0.1f} ) );
    auto renderObject =
        RenderObject::createRenderObject( "CubeRO", this, RenderObjectType::Geometry, display );
    addRenderObject( renderObject );

    addRenderObject( RenderObject::createRenderObject(
        "test_point",
        this,
        RenderObjectType::Geometry,
        DrawPrimitives::Point( {1_ra, 0_ra, 1_ra}, {0_ra, 1_ra, 0.3_ra} ),
        rt ) );

    addRenderObject( RenderObject::createRenderObject(
        "test_grid",
        this,
        RenderObjectType::Geometry,
        DrawPrimitives::Grid(
            Vector3::Zero(), Vector3::UnitX(), Vector3::UnitZ(), Utils::Color::Grey( 0.6f ) ),
        rt ) );

    addRenderObject( RenderObject::createRenderObject(
        "test_grid",
        this,
        RenderObjectType::Geometry,
        DrawPrimitives::Line( {0_ra, 0_ra, 0_ra}, {1_ra, 1_ra, 1_ra}, Utils::Color::Red() ),
        rt ) );

    addRenderObject( RenderObject::createRenderObject(
        "test_vector",
        this,
        RenderObjectType::Geometry,
        DrawPrimitives::Vector( {0_ra, 0_ra, 0_ra}, {-1_ra, 1_ra, 1_ra}, Utils::Color::Blue() ),
        rt ) );

    addRenderObject( RenderObject::createRenderObject(
        "test_ray",
        this,
        RenderObjectType::Geometry,
        DrawPrimitives::Ray( {{-1_ra, 1_ra, 1_ra}, {1_ra, 0_ra, 0_ra}}, Utils::Color::Yellow() ),
        rt ) );

    addRenderObject(
        RenderObject::createRenderObject( "test_triangle",
                                          this,
                                          RenderObjectType::Geometry,
                                          DrawPrimitives::Triangle( {-1.1_ra, 0.0_ra, 0.0_ra},
                                                                    {-0.9_ra, 0.0_ra, 0.0_ra},
                                                                    {-1.0_ra, 0.2_ra, 0.0_ra},
                                                                    Utils::Color::Green(),
                                                                    true ),
                                          rt ) );
    addRenderObject(
        RenderObject::createRenderObject( "test_triangle_wire",
                                          this,
                                          RenderObjectType::Geometry,
                                          DrawPrimitives::Triangle( {-1.1_ra, 0.0_ra, 0.1_ra},
                                                                    {-0.9_ra, 0.0_ra, 0.1_ra},
                                                                    {-1.0_ra, 0.2_ra, 0.1_ra},
                                                                    Utils::Color::White(),
                                                                    false ),
                                          rt ) );

    /*            addRenderObject( RenderObject::createRenderObject(
    "test_ray",
    this,
    RenderObjectType::Geometry,
    DrawPrimitives::QuadStrip( const Core::Vector3& a,
                                         const Core::Vector3& x,
                                         const Core::Vector3& y,
                                         uint quads,
                                         const Core::Utils::Color& color );
            addRenderObject( RenderObject::createRenderObject(
    "test_ray",
    this,
    RenderObjectType::Geometry,
    DrawPrimitives::Circle( const Core::Vector3& center,
                                      const Core::Vector3& normal,
                                      Scalar radius,
                                      uint segments,
                                      const Core::Utils::Color& color );
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

/// This function will spin our cube
void MinimalComponent::spin() {
    Ra::Core::AngleAxis aa( 0.01f, Ra::Core::Vector3::UnitY() );
    Ra::Core::Transform rot( aa );

    auto ro = Ra::Engine::RadiumEngine::getInstance()->getRenderObjectManager()->getRenderObject(
        m_renderObjects[0] );
    Ra::Core::Transform t = ro->getLocalTransform();
    ro->setLocalTransform( rot * t );
}

/// This system will be added to the engine. Every frame it will
/// add a task to be executed, calling the spin function of the component.
void MinimalSystem::generateTasks( Ra::Core::TaskQueue* q, const Ra::Engine::FrameInfo& info ) {
    // We check that our component is here.
    CORE_ASSERT( m_components.size() == 1, "System incorrectly initialized" );
    MinimalComponent* c = static_cast<MinimalComponent*>( m_components[0].second );

    // Create a new task which wil call c->spin() when executed.
    q->registerTask(
        new Ra::Core::FunctionTask( std::bind( &MinimalComponent::spin, c ), "spin" ) );
}

void MinimalSystem::addComponent( Ra::Engine::Entity* ent, MinimalComponent* comp ) {
    registerComponent( ent, comp );
}
