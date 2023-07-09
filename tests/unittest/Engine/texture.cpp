#include "Engine/Data/Texture.hpp"
#include "Engine/OpenGL.hpp"

#include <catch2/catch.hpp>

#include <Engine/Data/BlinnPhongMaterial.hpp>
#include <Engine/Data/LambertianMaterial.hpp>
#include <Engine/Data/PlainMaterial.hpp>
#include <Engine/Data/VolumetricMaterial.hpp>
#include <Engine/RadiumEngine.hpp>

#include <Headless/CLIViewer.hpp>
#include <globjects/Texture.h>
#ifdef HEADLESS_HAS_EGL
#    include <Headless/OpenGLContext/EglOpenGLContext.hpp>
#else
#    include <Headless/OpenGLContext/GlfwOpenGLContext.hpp>
#endif

using namespace Ra::Headless;
using namespace Ra::Engine::Data;
using namespace Ra::Core::Utils;

void printGlError() {
    gl::GLenum err = gl::glGetError();
    if ( err != gl::GL_NO_ERROR ) {
        const char* errBuf = glErrorString( err );
        LOG( Ra::Core::Utils::logERROR )
            << "OpenGL error (" << __FILE__ << ":" << __LINE__ << ", glCheckError()) : " << errBuf
            << "(" << err << " : 0x" << std::hex << err << std::dec << ")." << '\n'
            << Ra::Core::Utils::StackTrace();
    }
    else { LOG( Ra::Core::Utils::logINFO ) << "no gl error"; }
}

TEST_CASE( "Engine/Data/Textures", "[Engine][Engine/Data][Textures]" ) {

    // Get the Engine and materials initialized
    glbinding::Version glVersion { 4, 4 };
#ifdef HEADLESS_HAS_EGL
    CLIViewer viewer { std::make_unique<EglOpenGLContext>( glVersion ) };
#else
    CLIViewer viewer { std::make_unique<GlfwOpenGLContext>( glVersion ) };
#endif
    auto dummy_name = "unittest_textures";
    auto code       = viewer.init( 1, &dummy_name );
    viewer.setCamera();

    SECTION( "Texture Init Now" ) {
        REQUIRE( code == 0 );
        viewer.bindOpenGLContext( true );
        gl45core::GLuint id1, id2;
        TextureParameters params = { {}, {} };
        {
            Texture texture1( params );
            Texture texture2( params );

            texture1.initializeNow();
            texture2.initializeNow();

            auto gpuTexture1 = texture1.getGpuTexture();
            auto gpuTexture2 = texture2.getGpuTexture();

            REQUIRE( gpuTexture1 != nullptr );
            REQUIRE( gpuTexture2 != nullptr );

            id1 = gpuTexture1->id();
            id2 = gpuTexture2->id();

            REQUIRE( id1 != id2 );

            REQUIRE( gl45core::glIsTexture( id1 ) );
            REQUIRE( gl45core::glIsTexture( id2 ) );

            texture1.destroyNow();

            REQUIRE( !gl45core::glIsTexture( id1 ) );
            REQUIRE( texture1.getGpuTexture() == nullptr );
            REQUIRE( gl45core::glIsTexture( id2 ) );
            REQUIRE( texture2.getGpuTexture() == gpuTexture2 );
        }

        viewer.bindOpenGLContext( true );
        viewer.oneFrame();

        REQUIRE( !gl45core::glIsTexture( id1 ) );
        REQUIRE( !gl45core::glIsTexture( id2 ) );
    }

    SECTION( "Texture Init Delayed" ) {
        REQUIRE( code == 0 );
        viewer.bindOpenGLContext( true );
        gl45core::GLuint id1, id2;
        TextureParameters params = { {}, {} };
        {
            Texture texture1( params );
            Texture texture2( params );

            texture1.initialize();
            texture2.initialize();

            auto gpuTexture1 = texture1.getGpuTexture();
            auto gpuTexture2 = texture2.getGpuTexture();

            REQUIRE( gpuTexture1 == nullptr );
            REQUIRE( gpuTexture2 == nullptr );

            viewer.oneFrame();

            gpuTexture1 = texture1.getGpuTexture();
            gpuTexture2 = texture2.getGpuTexture();

            REQUIRE( gpuTexture1 != nullptr );
            REQUIRE( gpuTexture2 != nullptr );

            id1 = gpuTexture1->id();
            id2 = gpuTexture2->id();

            REQUIRE( id1 != id2 );

            REQUIRE( gl45core::glIsTexture( id1 ) );
            REQUIRE( gl45core::glIsTexture( id2 ) );

            texture1.destroy();

            // id1 is still texture
            REQUIRE( gl45core::glIsTexture( id1 ) );
            // while gpuTexture ptr directly reset
            REQUIRE( texture1.getGpuTexture() == nullptr );

            viewer.oneFrame();

            REQUIRE( !gl45core::glIsTexture( id1 ) );
            REQUIRE( texture1.getGpuTexture() == nullptr );

            REQUIRE( gl45core::glIsTexture( id2 ) );
            REQUIRE( texture2.getGpuTexture() == gpuTexture2 );
        }

        viewer.bindOpenGLContext( true );
        viewer.oneFrame();

        REQUIRE( !gl45core::glIsTexture( id1 ) );
        REQUIRE( !gl45core::glIsTexture( id2 ) );
    }
}
