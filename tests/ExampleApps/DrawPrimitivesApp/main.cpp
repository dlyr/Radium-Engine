#include <Engine/RaEngine.hpp>
#include <Engine/Scene/EntityManager.hpp>
#include <Gui/BaseApplication.hpp>
#include <Gui/RadiumWindow/SimpleWindowFactory.hpp>

#include <QApplication>

#include <QOpenGLContext>

#include <minimalapp.hpp>
#include <minimalradium.hpp>

int main( int argc, char* argv[] ) {

    // Create default format for Qt.
    QSurfaceFormat format;
    format.setVersion( 4, 4 );
    format.setProfile( QSurfaceFormat::CoreProfile );
    format.setDepthBufferSize( 24 );
    format.setStencilBufferSize( 8 );
    // format.setSamples( 16 );
    format.setSwapBehavior( QSurfaceFormat::DoubleBuffer );
    format.setSwapInterval( 0 );
    QSurfaceFormat::setDefaultFormat( format );

    // Create app and show viewer window
    MinimalApp app( argc, argv );
    app.m_viewer->show();
    app.m_viewer->resize( {500, 500} );
    CORE_ASSERT( app.m_viewer->getContext()->isValid(), "OpenGL was not initialized" );
    // process all events so that everithing is initialized
    QApplication::processEvents();

    // Create one system
    MinimalSystem* sys = new MinimalSystem;
    app.m_engine->registerSystem( "Minimal system", sys );

    // Create and initialize entity and component
    Ra::Engine::Scene::Entity* e = app.m_engine->getEntityManager()->createEntity( "Cube" );
    MinimalComponent* c          = new MinimalComponent( e );
    sys->addComponent( e, c );
    c->initialize();

    auto aabb = Ra::Engine::RadiumEngine::getInstance()->computeSceneAabb();
    if ( !aabb.isEmpty() ) { app.m_viewer->fitCameraToScene( aabb ); }

    // Start the app.
    app.m_frame_timer->start();
    return app.exec();
    /*
        // Create app and show viewer window
        Ra::Gui::BaseApplication app( argc, argv );
        app.initialize( Ra::Gui::SimpleWindowFactory {} );

        // Create one system
        MinimalSystem* sys = new MinimalSystem;
        app.m_engine->registerSystem( "Minimal system", sys );

        // Create and initialize entity and component
        auto e = app.m_engine->getEntityManager()->createEntity( "Cube" );
        auto c = new MinimalComponent( e );
        sys->addComponent( e, c );
        c->initialize();

        app.m_mainWindow->postLoadFile( "Primitives" );
        return app.exec();*/
}
