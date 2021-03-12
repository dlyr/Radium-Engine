
#include <minimalradium.hpp>

#include <Core/Asset/FileData.hpp>
#include <Core/Containers/MakeShared.hpp>
#include <Core/Geometry/MeshPrimitives.hpp>
#include <Core/Geometry/TopologicalMesh.hpp>
#include <Core/Resources/Resources.hpp>
#include <Core/Tasks/Task.hpp>
#include <Core/Tasks/TaskQueue.hpp>
#include <Core/Utils/Timer.hpp>

#include <Engine/Data/BlinnPhongMaterial.hpp>
#include <Engine/Data/LambertianMaterial.hpp>
#include <Engine/Data/Mesh.hpp>
#include <Engine/Data/PlainMaterial.hpp>
#include <Engine/FrameInfo.hpp>
#include <Engine/Rendering/RenderObject.hpp>
#include <Engine/Rendering/RenderObjectManager.hpp>
#include <Engine/Scene/GeometryComponent.hpp>

#ifdef IO_USE_ASSIMP
#    include <IO/AssimpLoader/AssimpFileLoader.hpp>
#endif

#include <random>

const bool ENABLE_GRID      = true;
const bool ENABLE_CUBES     = true;
const bool ENABLE_POINTS    = true;
const bool ENABLE_LINES     = true;
const bool ENABLE_VECTORS   = true;
const bool ENABLE_RAYS      = true;
const bool ENABLE_TRIANGLES = true;
const bool ENABLE_CIRCLES   = true;
const bool ENABLE_ARCS      = true;
const bool ENABLE_SPHERES   = true;
const bool ENABLE_CAPSULES  = true;
const bool ENABLE_DISKS     = true;
const bool ENABLE_NORMALS   = true;
const bool ENABLE_POLYS     = true;
const bool ENABLE_LOGO      = true;
const bool ENABLE_COLLAPSE  = true;

using namespace Ra;
using namespace Ra::Core;
using namespace Ra::Engine;
using namespace Ra::Engine::Rendering;
using namespace Ra::Engine::Data;
using namespace Ra::Engine::Scene;

/**
 * This file contains a minimal radium/qt application which shows the geometrical primitives
 * supported by Radium
 */

namespace internal {
Ra::Core::Vector3Array getPolyMeshVertices() {
    return Ra::Core::Vector3Array( {
        // quad
        /*0*/ {-1.1_ra, -0_ra, 0_ra},
        /*1*/ {1.1_ra, -0_ra, 0_ra},
        /*2*/ {1_ra, 1_ra, 0_ra},
        /*3*/ {-1_ra, 1_ra, 0_ra},
        // hepta
        /*4*/ {2_ra, 2_ra, 0_ra},
        /*5*/ {2_ra, 3_ra, 0_ra},
        /*6*/ {0_ra, 4_ra, 0_ra},
        /*7*/ {-2_ra, 3_ra, 0_ra},
        /*8*/ {-2_ra, 2_ra, 0_ra},
        // degen
        /*9*/ {-1.1_ra, -2_ra, 0_ra},
        /*10*/ {-0.5_ra, -2_ra, 0_ra},
        /*11*/ {-0.3_ra, -2_ra, 0_ra},
        /*12*/ {0.0_ra, -2_ra, 0_ra},
        /*13*/ {0.0_ra, -2_ra, 0_ra},
        /*14*/ {0.3_ra, -2_ra, 0_ra},
        /*15*/ {0.5_ra, -2_ra, 0_ra},
        /*16*/ {1.1_ra, -2_ra, 0_ra},
        // degen2
        /*17*/ {-1_ra, -3_ra, 0_ra},
        /*18*/ {1_ra, -3_ra, 0_ra},
    } );
}

Ra::Core::AlignedStdVector<VectorNui>
// Ra::Core::VectorNuArray
getPolyMeshFaces() {
    //    using VectorType = Eigen::Matrix<uint, Eigen::Dynamic, 1>;
    using VectorType = VectorNui;
    auto quad        = VectorType( 4 );
    quad << 0, 1, 2, 3;
    auto hepta = VectorType( 7 );
    hepta << 3, 2, 4, 5, 6, 7, 8;
    auto degen = VectorType( 10 );
    degen << 1, 0, 9, 10, 11, 12, 13, 14, 15, 16;
    auto degen2 = VectorType( 10 );
    degen2 << 14, 13, 12, 11, 10, 9, 17, 18, 16, 15;

    return Ra::Core::AlignedStdVector<VectorType> {quad, hepta, degen, degen2};
    //    return Ra::Core::VectorNuArray {quad, hepta, degen, degen2};
    //    return Ra::Core::VectorNuArray( {quad, hepta} );
}
} // namespace internal

MinimalComponent::MinimalComponent( Ra::Engine::Scene::Entity* entity ) :
    Ra::Engine::Scene::Component( "Minimal Component", entity ) {}

void updateCellCorner( Vector3& cellCorner, const Scalar cellSize, const int nCellX, const int ) {

    cellCorner[0] += cellSize;
    if ( cellCorner[0] > cellSize * ( ( 2 * nCellX - 1 ) / 4_ra ) )
    {
        cellCorner[0] = -nCellX * cellSize / 2_ra;
        cellCorner[2] += cellSize;
    }
}

/// This function is called when the component is properly
/// setup, i.e. it has an entity.
void MinimalComponent::initialize() {
    ///
    // basic render technique associated with all object here, they use per vertex kd.
    RenderTechnique shadedRt;
    {
        auto mat              = make_shared<BlinnPhongMaterial>( "Shaded Material" );
        mat->m_perVertexColor = true;
        mat->m_ks             = Utils::Color::White();
        mat->m_ns             = 100_ra;

        auto builder = EngineRenderTechniques::getDefaultTechnique( "BlinnPhong" );
        builder.second( shadedRt, false );
        shadedRt.setParametersProvider( mat );
    }
    RenderTechnique plainRt;
    {
        auto mat              = make_shared<PlainMaterial>( "Plain Material" );
        mat->m_perVertexColor = true;

        auto builder = EngineRenderTechniques::getDefaultTechnique( "Plain" );
        builder.second( plainRt, false );
        plainRt.setParametersProvider( mat );
    }
    RenderTechnique lambertianRt;
    {
        auto mat              = make_shared<LambertianMaterial>( "Lambertian Material" );
        mat->m_perVertexColor = true;

        auto builder = EngineRenderTechniques::getDefaultTechnique( "Lambertian" );
        builder.second( lambertianRt, false );
        lambertianRt.setParametersProvider( mat );
    }

    //// setup ////
    Scalar colorBoost = 1_ra; /// since simple primitive are ambient only, boost their color
    Scalar cellSize   = 0.35_ra;
    int nCellX        = 7;
    int nCellY        = 7;
    Vector3 cellCorner {-nCellX * cellSize / 2_ra, 0_ra, -nCellY * cellSize / 2_ra};
    Vector3 toCellCenter {cellSize / 2_ra, cellSize / 2_ra, cellSize / 2_ra};
    Scalar offset {0.05_ra};
    Vector3 offsetVec {offset, offset, offset};
    std::random_device rd;    // Will be used to obtain a seed for the random number engine
    std::mt19937 gen( rd() ); // Standard mersenne_twister_engine seeded with rd()
    std::uniform_real_distribution<Scalar> dis015( 0_ra, cellSize - 2_ra * offset );
    std::uniform_real_distribution<Scalar> dis01( 0_ra, 1_ra );
    std::uniform_real_distribution<Scalar> dis11( -1_ra, 1_ra );
    std::uniform_int_distribution<uint> disInt( 0, 128 );
    uint end = 8;

    //// GRID ////
    if ( ENABLE_GRID )
    {
        RenderTechnique gridRt;
        // Plain shader
        auto builder = EngineRenderTechniques::getDefaultTechnique( "Plain" );
        builder.second( gridRt, false );
        auto mat              = Ra::Core::make_shared<PlainMaterial>( "Grid material" );
        mat->m_perVertexColor = true;
        gridRt.setParametersProvider( mat );

        auto gridPrimitive = DrawPrimitives::Grid( Vector3::Zero(),
                                                   Vector3::UnitX(),
                                                   Vector3::UnitZ(),
                                                   Utils::Color::Grey( 0.6f ),
                                                   cellSize,
                                                   nCellX );

        auto gridRo = RenderObject::createRenderObject(
            "test_grid", this, RenderObjectType::Geometry, gridPrimitive, gridRt );
        gridRo->setPickable( false );
        addRenderObject( gridRo );
    }

    //// CUBES ////
    if ( ENABLE_CUBES )
    {
        std::shared_ptr<Mesh> cube1( new Mesh( "Cube" ) );
        auto coord = cellSize / 8_ra;
        cube1->loadGeometry( Geometry::makeSharpBox( Vector3 {coord, coord, coord} ) );
        cube1->getCoreGeometry().addAttrib(
            "in_color", Vector4Array {cube1->getNumVertices(), Utils::Color::Green()} );

        auto renderObject1 = RenderObject::createRenderObject(
            "CubeRO_1", this, RenderObjectType::Geometry, cube1, shadedRt );
        renderObject1->setLocalTransform( Transform {Translation( cellCorner )} );

        addRenderObject( renderObject1 );

        // another cube
        std::shared_ptr<Mesh> cube2( new Mesh( "Cube" ) );
        coord = cellSize / 4_ra;
        cube2->loadGeometry( Geometry::makeSharpBox( Vector3 {coord, coord, coord} ) );
        cube2->getCoreGeometry().addAttrib(
            "colour", Vector4Array {cube2->getNumVertices(), Utils::Color::Red()} );

        cube2->setAttribNameCorrespondance( "colour", "in_color" );
        auto renderObject2 = RenderObject::createRenderObject(
            "CubeRO_2", this, RenderObjectType::Geometry, cube2, lambertianRt );
        coord = cellSize / 2_ra;
        renderObject2->setLocalTransform(
            Transform {Translation( cellCorner + Vector3( coord, coord, coord ) )} );

        addRenderObject( renderObject2 );
    }
    //// POINTS ////
    if ( ENABLE_POINTS )
    {
        updateCellCorner( cellCorner, cellSize, nCellX, nCellY );
        updateCellCorner( cellCorner, cellSize, nCellX, nCellY );
        addRenderObject( RenderObject::createRenderObject(
            "test_point",
            this,
            RenderObjectType::Geometry,
            DrawPrimitives::Point( cellCorner, colorBoost * Utils::Color {0_ra, 1_ra, 0.3_ra} ),
            plainRt ) );
        for ( int i = 0; i < 10; ++i )
        {
            Vector3 randomVec {cellCorner + offsetVec +
                               Vector3 {dis015( gen ), dis015( gen ), dis015( gen )}};
            Color randomCol {dis01( gen ), dis01( gen ), dis01( gen )};
            addRenderObject( RenderObject::createRenderObject(
                "test_point",
                this,
                RenderObjectType::Geometry,
                DrawPrimitives::Point( randomVec, colorBoost * randomCol, 0.03_ra ),
                plainRt ) );
        }
    }
    //// LINES ////
    if ( ENABLE_LINES )
    {
        updateCellCorner( cellCorner, cellSize, nCellX, nCellY );
        updateCellCorner( cellCorner, cellSize, nCellX, nCellY );

        addRenderObject( RenderObject::createRenderObject(
            "test_line",
            this,
            RenderObjectType::Geometry,
            DrawPrimitives::Line( cellCorner,
                                  cellCorner + Vector3 {0_ra, 0.4_ra, 0_ra},
                                  colorBoost * Utils::Color::Red() ),
            plainRt ) );
        for ( int i = 0; i < 20; ++i )
        {
            Vector3 randomVec1 {cellCorner + offsetVec +
                                Vector3 {dis015( gen ), dis015( gen ), dis015( gen )}};
            Vector3 randomVec2 {cellCorner + offsetVec +
                                Vector3 {dis015( gen ), dis015( gen ), dis015( gen )}};
            Color randomCol {dis01( gen ), dis01( gen ), dis01( gen )};

            addRenderObject( RenderObject::createRenderObject(
                "test_line",
                this,
                RenderObjectType::Geometry,
                DrawPrimitives::Line( randomVec1, randomVec2, colorBoost * randomCol ),
                plainRt ) );
        }
    }
    //// VECTOR ////
    if ( ENABLE_VECTORS )
    {
        updateCellCorner( cellCorner, cellSize, nCellX, nCellY );
        updateCellCorner( cellCorner, cellSize, nCellX, nCellY );
        addRenderObject( RenderObject::createRenderObject(
            "test_vector",
            this,
            RenderObjectType::Geometry,
            DrawPrimitives::Vector(
                cellCorner, Vector3 {0_ra, 0.5_ra, 0_ra}, colorBoost * Utils::Color::Blue() ),
            plainRt ) );

        for ( int i = 0; i < 10; ++i )
        {
            Vector3 randomVec1 {cellCorner + offsetVec +
                                Vector3 {dis015( gen ), dis015( gen ), dis015( gen )}};
            Vector3 randomVec2 {cellCorner + offsetVec +
                                Vector3 {dis015( gen ), dis015( gen ), dis015( gen )}};
            Color randomCol {dis01( gen ), dis01( gen ), dis01( gen )};

            addRenderObject( RenderObject::createRenderObject(
                "test_vector",
                this,
                RenderObjectType::Geometry,
                DrawPrimitives::Vector(
                    randomVec1, randomVec2 - randomVec1, colorBoost * randomCol ),
                plainRt ) );
        }
    }
    if ( ENABLE_RAYS )
    {
        /// RAY ////
        updateCellCorner( cellCorner, cellSize, nCellX, nCellY );
        updateCellCorner( cellCorner, cellSize, nCellX, nCellY );
        addRenderObject( RenderObject::createRenderObject(
            "test_ray",
            this,
            RenderObjectType::Geometry,
            DrawPrimitives::Ray(
                {cellCorner, {0_ra, 1_ra, 0_ra}}, colorBoost * Utils::Color::Yellow(), cellSize ),
            plainRt ) );
    }
    //// TRIANGLES ////
    if ( ENABLE_TRIANGLES )
    {
        updateCellCorner( cellCorner, cellSize, nCellX, nCellY );
        updateCellCorner( cellCorner, cellSize, nCellX, nCellY );
        addRenderObject( RenderObject::createRenderObject(
            "test_triangle",
            this,
            RenderObjectType::Geometry,
            DrawPrimitives::Triangle( cellCorner + 4_ra * Vector3 {-0.01_ra, 0.0_ra, 0.0_ra},
                                      cellCorner + 4_ra * Vector3 {+0.01_ra, 0.0_ra, 0.0_ra},
                                      cellCorner + 4_ra * Vector3 {+0.0_ra, 0.02_ra, 0.0_ra},
                                      colorBoost * Utils::Color::White(),
                                      true ),
            shadedRt ) );

        addRenderObject( RenderObject::createRenderObject(
            "test_triangle",
            this,
            RenderObjectType::Geometry,
            DrawPrimitives::Triangle( cellCorner + Vector3 {cellSize / 2_ra, 0_ra, 0_ra} +
                                          Vector3 {-0.071_ra, 0.0_ra, 0.0_ra},
                                      cellCorner + Vector3 {cellSize / 2_ra, 0_ra, 0_ra} +
                                          Vector3 {+0.071_ra, 0.0_ra, 0.0_ra},
                                      cellCorner + Vector3 {cellSize / 2_ra, 0_ra, 0_ra} +
                                          Vector3 {+0.0_ra, 0.2_ra, 0.0_ra},
                                      colorBoost * Utils::Color::Green(),
                                      true ),
            shadedRt ) );

        for ( int i = 0; i < 10; ++i )
        {

            addRenderObject( RenderObject::createRenderObject(
                "test_triangle_wire",
                this,
                RenderObjectType::Geometry,
                DrawPrimitives::Triangle(
                    cellCorner + Vector3 {cellSize / 2_ra, 0_ra, 0_ra} +
                        Vector3 {-0.071_ra, 0.0_ra, Scalar( i ) / 20_ra * cellSize},
                    cellCorner + Vector3 {cellSize / 2_ra, 0_ra, 0_ra} +
                        Vector3 {+0.071_ra, 0.0_ra, Scalar( i ) / 20_ra * cellSize},
                    cellCorner + Vector3 {cellSize / 2_ra, 0_ra, 0_ra} +
                        Vector3 {+0.0_ra, 0.2_ra, Scalar( i ) / 20_ra * cellSize},
                    colorBoost * Utils::Color::White() * Scalar( i ) / 10_ra,
                    false ),
                plainRt ) );
        }
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
    if ( ENABLE_CIRCLES )
    {
        updateCellCorner( cellCorner, cellSize, nCellX, nCellY );
        updateCellCorner( cellCorner, cellSize, nCellX, nCellY );
        addRenderObject( RenderObject::createRenderObject(
            "test_circle",
            this,
            RenderObjectType::Geometry,
            DrawPrimitives::Circle( cellCorner,
                                    {0_ra, 0_ra, 1_ra},
                                    cellSize / 8_ra,
                                    64,
                                    colorBoost * Utils::Color::White() ),
            plainRt ) );

        for ( uint j = 0; j < end; ++j )
            for ( uint i = 0; i < end; ++i )
            {
                Vector3 circleCenter {cellCorner + offsetVec +
                                      Vector3 {Scalar( j ) / end * ( cellSize - 2 * offset ),
                                               offset,
                                               Scalar( i ) / end * ( cellSize - 2 * offset )}};
                Vector3 circleNormal {Vector3 {Scalar( i ), Scalar( j ), 10_ra}};
                circleNormal.normalize();
                Color randomCol {dis01( gen ), dis01( gen ), dis01( gen )};
                Scalar circleRadius {Scalar( end / 2 + i ) / Scalar( 2 * end ) * cellSize / 8_ra};
                uint circleSubdiv {3 + j * end + i};

                addRenderObject( RenderObject::createRenderObject(
                    "test_circle",
                    this,
                    RenderObjectType::Geometry,
                    DrawPrimitives::Circle( circleCenter,
                                            circleNormal,
                                            circleRadius,
                                            circleSubdiv,
                                            colorBoost * randomCol ),
                    plainRt ) );
            }
    }
    //// CIRCLE ARC ////
    if ( ENABLE_ARCS )
    {
        addRenderObject( RenderObject::createRenderObject(
            "test_circle",
            this,
            RenderObjectType::Geometry,
            DrawPrimitives::CircleArc( cellCorner + Vector3 {0_ra, 2_ra * offset, 0_ra},
                                       {0_ra, 0_ra, 1_ra},
                                       cellSize / 8_ra,
                                       1_ra,
                                       64,
                                       colorBoost * Utils::Color::White() ),
            plainRt ) );

        for ( uint j = 0; j < end; ++j )
            for ( uint i = 0; i < end; ++i )
            {
                Vector3 circleCenter {cellCorner + offsetVec +
                                      Vector3 {Scalar( j ) / end * ( cellSize - 2 * offset ),
                                               2 * offset,
                                               Scalar( i ) / end * ( cellSize - 2 * offset )}};
                Vector3 circleNormal {0_ra, 0_ra, 1_ra};
                circleNormal.normalize();
                Color randomCol {dis01( gen ), dis01( gen ), dis01( gen )};
                Scalar circleRadius {( cellSize - 2_ra * offset ) / 20_ra};
                Scalar circleArc {Scalar( i ) / Scalar( end ) * 2_ra};
                uint circleSubdiv {2 + j};

                addRenderObject( RenderObject::createRenderObject(
                    "test_circle",
                    this,
                    RenderObjectType::Geometry,
                    DrawPrimitives::CircleArc( circleCenter,
                                               circleNormal,
                                               circleRadius,
                                               circleArc,
                                               circleSubdiv,
                                               colorBoost * randomCol ),
                    plainRt ) );
            }
    }
    //// SPHERE /////

    if ( ENABLE_SPHERES )
    {
        updateCellCorner( cellCorner, cellSize, nCellX, nCellY );
        updateCellCorner( cellCorner, cellSize, nCellX, nCellY );
        addRenderObject( RenderObject::createRenderObject(
            "test_sphere",
            this,
            RenderObjectType::Geometry,
            DrawPrimitives::Sphere( cellCorner, 0.02_ra, Utils::Color::White() ),
            shadedRt ) );

        end = 32;
        for ( uint i = 0; i < end; ++i )
        {
            Scalar angle {Scalar( i ) / Scalar( end ) * 7_ra};
            Scalar ratio {Scalar( i ) / Scalar( end - 1 )};
            Vector3 center {cellCorner + Vector3 {cellSize / 2_ra, ratio * .1_ra, cellSize / 2_ra}};
            Vector3 center1 {center + Vector3 {ratio * cellSize * .4_ra * std::cos( angle ),
                                               0_ra,
                                               ratio * cellSize * .4_ra * std::sin( angle )}};
            Vector3 center2 {
                center +
                Vector3 {ratio * cellSize * .4_ra * std::cos( angle + Math::PiDiv3 * 2_ra ),
                         0_ra,
                         ratio * cellSize * .4_ra * std::sin( angle + Math::PiDiv3 * 2_ra )}};

            Vector3 center3 {
                center +
                Vector3 {ratio * cellSize * .4_ra * std::cos( angle + Math::PiDiv3 * 4_ra ),
                         0_ra,
                         ratio * cellSize * .4_ra * std::sin( angle + Math::PiDiv3 * 4_ra )}};

            Color color1 {Utils::Color::Green() * ratio};
            Color color2 {Utils::Color::Red() * ratio};
            Color color3 {Utils::Color::Blue() * ratio};

            addRenderObject( RenderObject::createRenderObject(
                "test_sphere",
                this,
                RenderObjectType::Geometry,
                DrawPrimitives::Sphere( center1, 0.005_ra + ratio * 0.01_ra, color1 ),
                shadedRt ) );
            addRenderObject( RenderObject::createRenderObject(
                "test_sphere",
                this,
                RenderObjectType::Geometry,
                DrawPrimitives::Sphere( center2, 0.005_ra + ratio * 0.01_ra, color2 ),
                shadedRt ) );

            addRenderObject( RenderObject::createRenderObject(
                "test_sphere",
                this,
                RenderObjectType::Geometry,
                DrawPrimitives::Sphere( center3, 0.01_ra + ratio * 0.01_ra, color3 ),
                shadedRt ) );
        }
    }
    //// CAPSULE ////
    if ( ENABLE_CAPSULES )
    {
        updateCellCorner( cellCorner, cellSize, nCellX, nCellY );
        updateCellCorner( cellCorner, cellSize, nCellX, nCellY );

        addRenderObject( RenderObject::createRenderObject(
            "test_capsule",
            this,
            RenderObjectType::Geometry,
            DrawPrimitives::Capsule( cellCorner,
                                     cellCorner + Vector3 {0_ra, 0.1_ra, 0_ra},
                                     0.02_ra,
                                     Utils::Color::White() ),
            shadedRt ) );
    }
    //// DISK ////
    if ( ENABLE_DISKS )
    {
        updateCellCorner( cellCorner, cellSize, nCellX, nCellY );
        updateCellCorner( cellCorner, cellSize, nCellX, nCellY );

        addRenderObject( RenderObject::createRenderObject(
            "test_disk",
            this,
            RenderObjectType::Geometry,
            DrawPrimitives::Disk( cellCorner,
                                  Vector3 {0_ra, 0_ra, 1_ra},
                                  0.05_ra,
                                  32,
                                  colorBoost * Utils::Color::White() ),
            shadedRt ) );
    }

    /// NORMAL
    if ( ENABLE_NORMALS )
    {
        updateCellCorner( cellCorner, cellSize, nCellX, nCellY );
        updateCellCorner( cellCorner, cellSize, nCellX, nCellY );
        addRenderObject( RenderObject::createRenderObject(
            "test_normal",
            this,
            RenderObjectType::Geometry,
            DrawPrimitives::Normal( cellCorner + Vector3 {0_ra, 0.1_ra, 0_ra},
                                    Vector3 {0_ra, 0_ra, 1_ra},
                                    colorBoost * Utils::Color::White(),
                                    0.01_ra ),
            plainRt ) );
    }
    /*
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
                       spline, uint pointCount, const Core::Utils::Color& color, Scalar scale
       = 1.0f
                   );*/

    //// PolyMesh ////
    if ( ENABLE_POLYS )
    {
        updateCellCorner( cellCorner, cellSize, nCellX, nCellY );
        updateCellCorner( cellCorner, cellSize, nCellX, nCellY );

        /// \todo may have an alignement issue
        /*
                Ra::Core::Geometry::PolyMesh polyMesh;
                polyMesh.setVertices( internal::getPolyMeshVertices() );

                Vector3Array normals;
                normals.resize( polyMesh.vertices().size() );
                std::transform(
                    polyMesh.vertices().cbegin(),
                    polyMesh.vertices().cend(),
                    normals.begin(),
                    []( const Vector3& v ) { return ( v + Vector3( 0_ra, 0_ra, 1_ra )
           ).normalized(); } ); polyMesh.setNormals( normals );

                polyMesh.setIndices( internal::getPolyMeshFaces() );
        */

        Ra::Core::Geometry::PolyMesh polyMesh;
        polyMesh.setVertices( {
            // quad
            {-1.1_ra, -0_ra, 0_ra},
            {1.1_ra, -0_ra, 0_ra},
            {1_ra, 1_ra, 0_ra},
            {-1_ra, 1_ra, 0_ra},
            // hepta
            {2_ra, 2_ra, 0_ra},
            {2_ra, 3_ra, 0_ra},
            {0_ra, 4_ra, 0_ra},
            {-2_ra, 3_ra, 0_ra},
            {-2_ra, 2_ra, 0_ra},
        } );

        Vector3Array normals;
        normals.resize( polyMesh.vertices().size() );
        std::transform(
            polyMesh.vertices().cbegin(),
            polyMesh.vertices().cend(),
            normals.begin(),
            []( const Vector3& v ) { return ( v + Vector3( 0_ra, 0_ra, 1_ra ) ).normalized(); } );
        polyMesh.setNormals( normals );

        auto quad = VectorNui( 4 );
        quad << 0, 1, 2, 3;
        auto hepta = VectorNui( 7 );
        hepta << 3, 2, 4, 5, 6, 7, 8;
        polyMesh.setIndices( {quad, hepta} );
        // polyMesh.setIndices( {quad} );

        std::shared_ptr<PolyMesh> poly1( new PolyMesh( "Poly", std::move( polyMesh ) ) );
        poly1->getCoreGeometry().addAttrib(
            "in_color",
            Vector4Array {poly1->getNumVertices(),
                          colorBoost * Utils::Color {1_ra, 0.6_ra, 0.1_ra}} );

        auto renderObject1 = RenderObject::createRenderObject(
            "polymesh", this, RenderObjectType::Geometry, poly1, shadedRt );
        renderObject1->setLocalTransform( Transform {Translation( Vector3( cellCorner ) ) *
                                                     Eigen::UniformScaling<Scalar>( 0.06_ra )} );

        addRenderObject( renderObject1 );

        Ra::Core::Geometry::TopologicalMesh topo {poly1->getCoreGeometry()};
        topo.triangulate();
        topo.checkIntegrity();
        auto triangulated = topo.toTriangleMesh();
        std::shared_ptr<Mesh> poly2( new Mesh( "Poly", std::move( triangulated ) ) );
        poly2->getCoreGeometry().addAttrib(
            "in_color",
            Vector4Array {poly2->getNumVertices(),
                          colorBoost * Utils::Color {0_ra, 0.6_ra, 0.1_ra}} );

        auto renderObject2 = RenderObject::createRenderObject(
            "triangulated", this, RenderObjectType::Geometry, poly2, shadedRt );
        renderObject2->setLocalTransform(
            Transform {Translation( Vector3( cellCorner ) + toCellCenter ) *
                       Eigen::UniformScaling<Scalar>( 0.03_ra )} );

        addRenderObject( renderObject2 );
    }

    if ( ENABLE_LOGO )
    {
        updateCellCorner( cellCorner, cellSize, nCellX, nCellY );
        updateCellCorner( cellCorner, cellSize, nCellX, nCellY );

        Asset::FileData* data;
        auto l               = IO::AssimpFileLoader();
        auto rp              = Resources::getResourcesPath();
        std::string filename = *rp + "/Assets/radium-logo.dae";
        data                 = l.loadFile( filename );
        if ( data != nullptr )
        {
            auto geomData = data->getGeometryData();

            for ( const auto& gd : geomData )
            {
                std::shared_ptr<AttribArrayDisplayable> mesh {nullptr};
                switch ( gd->getType() )
                {
                case Ra::Core::Asset::GeometryData::TRI_MESH:
                    mesh =
                        std::shared_ptr<Mesh> {meshFactory<Geometry::TriangleMesh>( "logo", gd )};
                    break;
                case Ra::Core::Asset::GeometryData::QUAD_MESH:
                case Ra::Core::Asset::GeometryData::POLY_MESH:
                    mesh =
                        std::shared_ptr<PolyMesh> {meshFactory<Geometry::PolyMesh>( "logo", gd )};
                    break;
                default:
                    break;
                }

                std::shared_ptr<Data::Material> roMaterial;
                const Core::Asset::MaterialData* md =
                    gd->hasMaterial() ? &( gd->getMaterial() ) : nullptr;
                // First extract the material from asset or create a default one
                if ( md != nullptr )
                {
                    auto converter =
                        Data::EngineMaterialConverters::getMaterialConverter( md->getType() );
                    auto mat = converter.second( md );
                    roMaterial.reset( mat );
                }
                else
                {
                    auto mat             = new Data::BlinnPhongMaterial( "_DefaultBPMaterial" );
                    mat->m_renderAsSplat = mesh->getNumFaces() == 0;
                    roMaterial.reset( mat );
                }

                // Create the RenderObject
                auto renderObject = RenderObject::createRenderObject(
                    "logo", this, RenderObjectType::Geometry, mesh, Rendering::RenderTechnique {} );
                renderObject->setLocalTransform(
                    Transform {Translation( Vector3( cellCorner ) + toCellCenter ) *
                               Eigen::UniformScaling<Scalar>( cellSize * 0.02_ra )} );
                renderObject->setMaterial( roMaterial );

                // Build the renderTechnique from the loaded material
                auto builder =
                    EngineRenderTechniques::getDefaultTechnique( roMaterial->getMaterialName() );
                auto logoRt = Core::make_shared<RenderTechnique>();
                builder.second( *logoRt, false );
                logoRt->setParametersProvider( roMaterial );
                // associate the renderTechnique to the RenderObject
                renderObject->setRenderTechnique( logoRt );

                addRenderObject( renderObject );
            }
        }
    }

    if ( ENABLE_COLLAPSE )
    {

        using namespace Ra::Core;
        using namespace Ra::Core::Utils;
        using namespace Ra::Core::Geometry;
        auto findHalfedge = []( TopologicalMesh& topo,
                                const Vector3& from,
                                const Vector3& to ) -> optional<TopologicalMesh::HalfedgeHandle> {
            bool found;
            TopologicalMesh::HalfedgeHandle he;
            for ( auto he_iter = topo.halfedges_begin(); he_iter != topo.halfedges_end();
                  ++he_iter )
            {

                if ( topo.point( topo.to_vertex_handle( he_iter ) ) == to &&
                     topo.point( topo.from_vertex_handle( he_iter ) ) == from )
                {
                    found = true;
                    he    = *he_iter;
                }
            }
            if ( found ) return he;
            return {};
        };

        auto addMesh = [this, colorBoost, shadedRt, plainRt]( Vector3 pos, TopologicalMesh topo1 ) {
            topo1.checkIntegrity();
            auto mesh1 = topo1.toTriangleMesh();
            std::shared_ptr<Mesh> poly( new Mesh( "TEST", std::move( mesh1 ) ) );

            auto renderObject2 = RenderObject::createRenderObject(
                "TEST", this, RenderObjectType::Geometry, poly, plainRt );
            renderObject2->setLocalTransform( Transform {
                Translation( Vector3( pos ) ) * Eigen::UniformScaling<Scalar>( 0.02_ra )} );

            addRenderObject( renderObject2 );
        };

        Vector3Array points {
            {00._ra, 00._ra, 00._ra},
            {10._ra, 00._ra, 00._ra},
            {05._ra, 05._ra, 00._ra},
            {05._ra, 10._ra, 00._ra},
            {15._ra, 05._ra, 00._ra},
            {10._ra, 08._ra, 00._ra},
            {10._ra, 12._ra, 00._ra},
            {15._ra, 10._ra, 00._ra},
        };
        Vector3Array points2 = {points[0], points[0], points[1], points[1], points[1], points[2],
                                points[2], points[2], points[2], points[3], points[3], points[3],
                                points[4], points[4], points[5], points[5], points[5], points[5],
                                points[5], points[5], points[6], points[6], points[7], points[7]};

        Vector4Array colors = {
            {0_ra, 0_ra, 0_ra, 1_ra},    {1_ra, 1_ra, 1_ra, 1_ra},    {2_ra, 2_ra, 2_ra, 1_ra},
            {3_ra, 3_ra, 3_ra, 1_ra},    {4_ra, 4_ra, 4_ra, 1_ra},    {5_ra, 5_ra, 5_ra, 1_ra},
            {6_ra, 6_ra, 6_ra, 1_ra},    {7_ra, 7_ra, 7_ra, 1_ra},    {8_ra, 8_ra, 8_ra, 1_ra},
            {9_ra, 9_ra, 9_ra, 1_ra},    {10_ra, 10_ra, 10_ra, 1_ra}, {11_ra, 11_ra, 11_ra, 1_ra},
            {12_ra, 12_ra, 12_ra, 1_ra}, {13_ra, 13_ra, 13_ra, 1_ra}, {14_ra, 14_ra, 14_ra, 1_ra},
            {15_ra, 15_ra, 15_ra, 1_ra}, {16_ra, 16_ra, 16_ra, 1_ra}, {17_ra, 17_ra, 17_ra, 1_ra},
            {18_ra, 18_ra, 18_ra, 1_ra}, {19_ra, 19_ra, 19_ra, 1_ra}, {20_ra, 20_ra, 20_ra, 1_ra},
            {21_ra, 21_ra, 21_ra, 1_ra}, {22_ra, 22_ra, 22_ra, 1_ra}, {23_ra, 23_ra, 23_ra, 1_ra},
        };

        for ( auto& c : colors )
        {
            c = colorBoost * Vector4 {dis01( gen ), dis01( gen ), dis01( gen ), 1_ra};
        }

        VectorArray<Vector3ui> indices1 {
            {0, 2, 1}, {0, 3, 2}, {1, 2, 5}, {2, 3, 5}, {1, 5, 4}, {3, 6, 5}, {5, 6, 7}, {4, 5, 7}};
        Vector3uArray indices3 = {{0, 2, 1}, {1, 2, 5}, {1, 5, 4}, {3, 6, 5}, {5, 6, 7}, {4, 5, 7}};

        Vector3uArray indices4 = {
            {0, 2, 5}, {3, 14, 6}, {4, 12, 15}, {11, 18, 20}, {17, 22, 21}, {16, 13, 23}};

        VectorArray<Vector3ui> indices2 {{0, 5, 2},
                                         {1, 9, 8},
                                         {3, 6, 14},
                                         {7, 10, 19},
                                         {4, 15, 12},
                                         {11, 20, 18},
                                         {17, 21, 22},
                                         {16, 23, 13}};
        Vector4Array colors2 {24, Color::White()};
        for ( const auto& face : indices2 )
        {
            colors2[face[0]] = colors[face[0]];
            colors2[face[1]] = colors[face[0]];
            colors2[face[2]] = colors[face[0]];
        }
        TriangleMesh mesh1;

        TopologicalMesh topo1;
        optional<TopologicalMesh::HalfedgeHandle> optHe;
        Vector3 pos;

        auto addMergeScene =
            [findHalfedge, addMesh, &cellCorner, toCellCenter, cellSize, nCellX, nCellY](
                const Vector3Array& points,
                const Vector4Array& colors,
                const Vector3uArray& indices1,
                const Vector3& from,
                const Vector3& to ) {
                TriangleMesh mesh1;

                TopologicalMesh topo1;
                optional<TopologicalMesh::HalfedgeHandle> optHe;
                Vector3 pos;

                mesh1.setVertices( points );
                mesh1.addAttrib( Mesh::getAttribName( Mesh::VERTEX_COLOR ),
                                 Vector4Array {colors.begin(), colors.begin() + points.size()} );
                mesh1.setIndices( indices1 );
                topo1 = TopologicalMesh {mesh1};
                optHe = findHalfedge( topo1, from, to );

                pos = cellCorner + toCellCenter;
                addMesh( pos, topo1 );

                updateCellCorner( cellCorner, cellSize, nCellX, nCellY );
                pos = cellCorner + toCellCenter;
                topo1.collapseWedge( *optHe );
                addMesh( pos, topo1 );

                topo1 = TopologicalMesh {mesh1};
                optHe = findHalfedge( topo1, from, to );
                pos   = cellCorner + toCellCenter + Vector3 {0_ra, .25_ra, 0_ra};
                topo1.collapseWedge( *optHe, true );
                addMesh( pos, topo1 );
            };

        // With "continuous" wedges.
        addMergeScene( points, colors, indices1, points[5], points[2] );

        updateCellCorner( cellCorner, cellSize, nCellX, nCellY );

        // with "flat face" wedges
        addMergeScene( points2, colors2, indices2, points[5], points[2] );

        updateCellCorner( cellCorner, cellSize, nCellX, nCellY );

        // boundary
        // With "continuous" wedges.
        addMergeScene( points, colors, indices3, points[5], points[2] );

        updateCellCorner( cellCorner, cellSize, nCellX, nCellY );
        // with "flat face" wedges
        addMergeScene( points2, colors2, indices4, points[5], points[2] );

        // other way round
        updateCellCorner( cellCorner, cellSize, nCellX, nCellY );

        // With "continuous" wedges.
        addMergeScene( points, colors, indices1, points[2], points[5] );

        updateCellCorner( cellCorner, cellSize, nCellX, nCellY );

        // with "flat face" wedges
        addMergeScene( points2, colors2, indices2, points[2], points[5] );

        updateCellCorner( cellCorner, cellSize, nCellX, nCellY );
        //
        //        // boundary
        //        // With "continuous" wedges.
        addMergeScene( points, colors, indices3, points[2], points[5] );
        //
        updateCellCorner( cellCorner, cellSize, nCellX, nCellY );
        //        // with "flat face" wedges
        addMergeScene( points2, colors2, indices4, points[2], points[5] );
    }
}

/// This system will be added to the engine. Every frame it will
/// add a task to be executed, calling the spin function of the component.
void MinimalSystem::generateTasks( Ra::Core::TaskQueue* q, const Ra::Engine::FrameInfo& info ) {
    CORE_UNUSED( info );
    CORE_UNUSED( q );

    // We check that our component is here.
    //    CORE_ASSERT( m_components.size() == 2, "System incorrectly initialized" );
    //    MinimalComponent* c = static_cast<MinimalComponent*>( m_components[0].second );
}

void MinimalSystem::addComponent( Ra::Engine::Scene::Entity* ent, MinimalComponent* comp ) {
    registerComponent( ent, comp );
    /*
        //// POLYMESH FROM FILEDATA ////
        {
            using Ra::Core::Asset::GeometryData;

            GeometryData geometry( "Geometry", GeometryData::POLY_MESH );
            Ra::Core::Transform tr = {Ra::Core::Translation( Ra::Core::Vector3( 2, 0_ra, 2 ) ) *
                                      Eigen::UniformScaling<Scalar>( 0.06_ra )};
            geometry.setFrame( tr );
            geometry.setVertices( internal::getPolyMeshVertices() );

            Ra::Core::Vector3Array normals;
            normals.resize( geometry.getVertices().size() );
            std::transform(
                geometry.getVertices().cbegin(),
                geometry.getVertices().cend(),
                normals.begin(),
                []( const Ra::Core::Vector3& v ) { return ( v + Ra::Core::Vector3( 0_ra, 0_ra, 1_ra
       ) ).normalized(); } ); geometry.setNormals( normals );

            geometry.setFaces( internal::getPolyMeshFaces() );

            auto comp2 =
                new Ra::Engine::Scene::PolyMeshComponent( "GeometryComponent", ent, &geometry );
            registerComponent( ent, comp2 );
            comp2->initialize();
        }*/
}
