#include <Core/Geometry/MeshPrimitives.hpp>
#include <Core/Geometry/TriangleMesh.hpp>
#include <Tests.hpp>

namespace Ra {
namespace Testing {

void run() {
    using Ra::Core::Vector3;
    using Ra::Core::Geometry::TriangleMesh;
    using Vec3AttribHandle = Ra::Core::Utils::AttribHandle<Vector3>;

    TriangleMesh mesh = Ra::Core::Geometry::makeBox();

    // "in_position" or "in_normal" are added by default
    auto h_pos = mesh.getAttribHandle<Vector3>( "in_position" );
    RA_VERIFY( mesh.isValid( h_pos ), "Should be a valid handle." );
    auto h_nor = mesh.getAttribHandle<Vector3>( "in_normal" );
    RA_VERIFY( mesh.isValid( h_nor ), "Should be a valid handle." );

    // Add/Remove attributes without filling it
    auto handleEmpty = mesh.addAttrib<Vec3AttribHandle::value_type>( "empty" );
    mesh.removeAttrib( handleEmpty );
    RA_VERIFY( !mesh.isValid( handleEmpty ), "Should be an invalid handle." );
    handleEmpty = mesh.addAttrib<Vec3AttribHandle::value_type>( "empty" );
    RA_VERIFY( mesh.isValid( handleEmpty ), "Should get a valid handle here !" );
    mesh.removeAttrib( handleEmpty );
    handleEmpty = mesh.getAttribHandle<Vec3AttribHandle::value_type>( "empty" );
    RA_VERIFY( !mesh.isValid( handleEmpty ), "Should be an invalid handle." );

    // Test access to the attribute container
    auto handleFilled     = mesh.addAttrib<Vec3AttribHandle::value_type>( "filled" );
    auto& attribFilled    = mesh.getAttrib( handleFilled );
    auto& containerFilled = attribFilled.getDataWithLock();
    RA_VERIFY( attribFilled.isLocked(), "data lock failed" );

    // Test filling and removing vec3 attributes
    for ( int i = 0; i != mesh.vertices().size(); ++i )
        containerFilled.push_back( Vec3AttribHandle::value_type::Random() );
    attribFilled.unlock();

    auto handleFilled2     = mesh.getAttribHandle<Vec3AttribHandle::value_type>( "filled" );
    auto& containerFilled2 = mesh.getAttrib( handleFilled2 ).data();
    RA_VERIFY( containerFilled == containerFilled2, "getAttrib variants are not consistents" );

    mesh.removeAttrib( handleFilled );

    // Test attribute creation by type, filling and removal
    auto handle      = mesh.addAttrib<Eigen::Matrix<unsigned int, 1, 1>>( "filled2" );
    auto& container3 = mesh.getAttrib( handle ).getDataWithLock();
    using HandleType = decltype( handle );

    for ( int i = 0; i != mesh.vertices().size(); ++i )
        container3.push_back( typename HandleType::value_type( i ) );
    mesh.getAttrib( handle ).unlock();
    mesh.removeAttrib( handle );

    // Test dummy handle
    auto invalid = mesh.getAttribHandle<float>( "toto" );
    RA_VERIFY( !mesh.isValid( invalid ), "Invalid Attrib Handle cannot be recognized" );

    // Test attribute copy
    const auto v0         = mesh.vertices()[0];
    TriangleMesh meshCopy = mesh;
    meshCopy.copyAllAttributes( mesh );
    RA_VERIFY( mesh.vertices()[0].isApprox( v0 ), "Cannot copy TriangleMesh" );
    meshCopy.verticesWithLock()[0] += Ra::Core::Vector3( 0.5, 0.5, 0.5 );
    meshCopy.verticesUnlock();
    RA_VERIFY( !meshCopy.vertices()[0].isApprox( v0 ), "Cannot copy TriangleMesh attributes" );
}
} // namespace Testing
} // namespace Ra

int main( int argc, const char** argv ) {
    using namespace Ra;

    if ( !Testing::init_testing( 1, argv ) ) { return EXIT_FAILURE; }

#pragma omp parallel for
    for ( int i = 0; i < Testing::g_repeat; ++i )
    {
        CALL_SUBTEST( ( Testing::run() ) );
    }

    return EXIT_SUCCESS;
}
