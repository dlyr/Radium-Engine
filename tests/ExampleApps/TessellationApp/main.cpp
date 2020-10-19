// Include Radium base application and its simple Gui
#include <GuiBase/BaseApplication.hpp>
#include <GuiBase/RadiumWindow/SimpleWindowFactory.hpp>

// include the Engine/entity/component interface
#include <Core/Geometry/MeshPrimitives.hpp>
#include <Core/Resources/Resources.hpp>
#include <Engine/Component/GeometryComponent.hpp>
#include <Engine/Managers/EntityManager/EntityManager.hpp>
#include <Engine/Renderer/RenderObject/RenderObjectManager.hpp>
#include <Engine/System/GeometrySystem.hpp>

#include <QTimer>

using namespace Ra::Engine;
using namespace Ra;
int main( int argc, char* argv[] ) {
    //! [Creating the application]
    Ra::GuiBase::BaseApplication app( argc, argv, Ra::GuiBase::SimpleWindowFactory {} );
    //! [Creating the application]

    //! [Creating the cube]
    auto cube = Ra::Core::Geometry::makeSharpBox( {0.1f, 0.1f, 0.1f} );
    //! [Creating the cube]

    //! [Colorize the Cube]
    cube.addAttrib(
        "in_color",
        Ra::Core::Vector4Array {cube.vertices().size(), Ra::Core::Utils::Color::Green()} );
    //! [Colorize the Cube]

    //! [Create the engine entity for the cube]
    auto e = app.m_engine->getEntityManager()->createEntity( "Green cube" );
    //! [Create the engine entity for the cube]

    //! [Create a geometry component with the cube]
    auto c = new Ra::Engine::TriangleMeshComponent( "Cube Mesh", e, std::move( cube ), nullptr );
    //! [Create a geometry component with the cube]

    //! [Register the entity/component association to the geometry system ]
    auto geometrySystem = app.m_engine->getSystem( "GeometrySystem" );
    geometrySystem->addComponent( e, c );
    //! [Register the entity/component association to the geometry system ]

    //! [Tell the window that something is to be displayed]
    app.m_mainWindow->postLoadFile( "Cube" );
    //! [Tell the window that something is to be displayed]

    c->getDisplayable()->setRenderMode( Ra::Engine::AttribArrayDisplayable::RM_PATCHES );
    auto renderObject =
        Ra::Engine::RadiumEngine::getInstance()->getRenderObjectManager()->getRenderObject(
            c->getRenderObjectIndex() );

    auto renderTechnique = renderObject->getRenderTechnique();
    auto configuration1 =
        renderTechnique->getConfiguration( DefaultRenderingPasses::LIGHTING_OPAQUE );

    auto resDir =
        std::string( Core::Resources::getRadiumResourcesDir() ) + "Shaders/tessellationExample";
    configuration1.addShader( Ra::Engine::ShaderType_TESS_CONTROL, resDir + ".tcs.glsl" );
    configuration1.addShader( Ra::Engine::ShaderType_TESS_EVALUATION, resDir + ".tes.glsl" );
    configuration1.addShader( Ra::Engine::ShaderType_VERTEX, resDir + ".vert.glsl" );
    configuration1.addShader( Ra::Engine::ShaderType_GEOMETRY, resDir + ".geom.glsl" );
    configuration1.addShader( Ra::Engine::ShaderType_FRAGMENT, resDir + ".frag.glsl" );
    renderTechnique->setConfiguration( configuration1, DefaultRenderingPasses::LIGHTING_OPAQUE );

    auto configuration0 = renderTechnique->getConfiguration( DefaultRenderingPasses::Z_PREPASS );
    configuration0.addShader( Ra::Engine::ShaderType_TESS_CONTROL, resDir + ".tcs.glsl" );
    configuration0.addShader( Ra::Engine::ShaderType_TESS_EVALUATION, resDir + ".tes.glsl" );
    configuration0.addShader( Ra::Engine::ShaderType_VERTEX, resDir + ".vert.glsl" );
    configuration0.addShader( Ra::Engine::ShaderType_GEOMETRY, resDir + ".geom.glsl" );
    configuration0.addShader( Ra::Engine::ShaderType_FRAGMENT, resDir + "ZPrePass.frag.glsl" );
    renderTechnique->setConfiguration( configuration0, DefaultRenderingPasses::Z_PREPASS );
    // terminate the app after 4 second (approximatively). Camera can be moved using mouse moves.
    //  auto close_timer = new QTimer( &app );
    // close_timer->setInterval( 4000 );
    // QObject::connect( close_timer, &QTimer::timeout, [&app]() { app.appNeedsToQuit(); } );
    // close_timer->start();

    return app.exec();
}
