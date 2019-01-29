#include <Engine/Component/GeometryComponent.hpp>

#include <iostream>

#include <Core/Asset/FileData.hpp>
#include <Core/Asset/GeometryData.hpp>
#include <Core/Containers/MakeShared.hpp>
#include <Core/Geometry/Normal.hpp>
#include <Core/Utils/Color.hpp>

#include <Engine/Managers/ComponentMessenger/ComponentMessenger.hpp>
#include <Engine/Renderer/RenderObject/RenderObjectManager.hpp>

#include <Engine/Renderer/Mesh/Mesh.hpp>

#include <Engine/Renderer/Material/BlinnPhongMaterial.hpp>
#include <Engine/Renderer/Material/Material.hpp>
#include <Engine/Renderer/Material/MaterialConverters.hpp>

#include <Engine/Renderer/RenderObject/Primitives/DrawPrimitives.hpp>
#include <Engine/Renderer/RenderObject/RenderObject.hpp>
#include <Engine/Renderer/RenderObject/RenderObjectTypes.hpp>

#include <Engine/Renderer/RenderTechnique/RenderTechnique.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderConfigFactory.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgram.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgramManager.hpp>

using TriangleArray = Ra::Core::VectorArray<Ra::Core::Vector3ui>;

namespace Ra {
namespace Engine {
GeometryComponent::GeometryComponent( const std::string& name, bool deformable, Entity* entity ) :
    Component( name, entity ),
    m_deformable{deformable} {}

GeometryComponent::~GeometryComponent() = default;

void GeometryComponent::initialize() {}

void GeometryComponent::addMeshRenderObject( const Ra::Core::Geometry::TriangleMesh& mesh,
                                             const std::string& name ) {
    setupIO( name );

    std::shared_ptr<Mesh> displayMesh( new Mesh( name ) );
    displayMesh->loadGeometry( mesh );

    auto renderObject =
        RenderObject::createRenderObject( name, this, RenderObjectType::Geometry, displayMesh );
    m_meshIndex = addRenderObject( renderObject );
}

void GeometryComponent::handleMeshLoading( const Ra::Core::Asset::GeometryData* data ) {
    std::string name( m_name );
    name.append( "_" + data->getName() );

    std::string roName = name;

    roName.append( "_RO" );
    std::string meshName = name;
    meshName.append( "_Mesh" );

    std::string matName = name;
    matName.append( "_Mat" );

    m_contentName = data->getName();

    auto displayMesh = Ra::Core::make_shared<Mesh>( meshName /*, Mesh::RM_POINTS*/ );

    Ra::Core::Geometry::TriangleMesh mesh;
    const auto T = data->getFrame();
    const Ra::Core::Transform N( ( T.matrix() ).inverse().transpose() );

    mesh.vertices().resize( data->getVerticesSize(), Ra::Core::Vector3::Zero() );

#pragma omp parallel for
    for ( uint i = 0; i < data->getVerticesSize(); ++i )
    {
        mesh.vertices()[i] = T * data->getVertices()[i];
    }

    if ( data->hasNormals() )
    {
        mesh.normals().resize( data->getVerticesSize(), Ra::Core::Vector3::Zero() );
#pragma omp parallel for
        for ( uint i = 0; i < data->getVerticesSize(); ++i )
        {
            mesh.normals()[i] = ( N * data->getNormals()[i] ).normalized();
        }
    }

    const auto& faces = data->getFaces();
    mesh.m_triangles.resize( faces.size(), Ra::Core::Vector3ui::Zero() );
#pragma omp parallel for
    for ( uint i = 0; i < faces.size(); ++i )
    {
        mesh.m_triangles[i] = faces[i].head<3>();
    }

    displayMesh->loadGeometry( mesh );

    if ( data->hasTangents() )
    {
        displayMesh->addData( Mesh::VERTEX_TANGENT, data->getTangents() );
    }

    if ( data->hasBiTangents() )
    {
        displayMesh->addData( Mesh::VERTEX_BITANGENT, data->getBiTangents() );
    }

    if ( data->hasTextureCoordinates() )
    {
        displayMesh->addData( Mesh::VERTEX_TEXCOORD, data->getTexCoords() );
    }

    if ( data->hasColors() )
    {
        displayMesh->addData( Mesh::VERTEX_COLOR, data->getColors() );
    }

    // To be discussed: Should not weights be part of the geometry ?
    //        mesh->addData( Mesh::VERTEX_WEIGHTS, meshData.weights );

    // The technique for rendering this component
    RenderTechnique rt;

    bool isTransparent{false};
    if ( data->hasMaterial() )
    {
        const Ra::Core::Asset::MaterialData& loadedMaterial = data->getMaterial();

        // First extract the material from asset
        auto converter = EngineMaterialConverters::getMaterialConverter( loadedMaterial.getType() );
        auto convertedMaterial = converter.second( &loadedMaterial );

        // Second, associate the material to the render technique
        std::shared_ptr<Material> radiumMaterial( convertedMaterial );
        if ( radiumMaterial != nullptr )
        {
            isTransparent = radiumMaterial->isTransparent();
        }
        rt.setMaterial( radiumMaterial );

        // Third, define the technique for rendering this material (here, using the default)
        auto builder = EngineRenderTechniques::getDefaultTechnique( loadedMaterial.getType() );
        builder.second( rt, isTransparent );
    } else
    {
        auto mat =
            Ra::Core::make_shared<BlinnPhongMaterial>( data->getName() + "_DefaultBPMaterial" );
        mat->m_kd = Ra::Core::Utils::Color::Grey();
        mat->m_ks = Ra::Core::Utils::Color::White();
        rt.setMaterial( mat );
        auto builder = EngineRenderTechniques::getDefaultTechnique( "BlinnPhong" );
        builder.second( rt, isTransparent );
    }

    auto ro = RenderObject::createRenderObject( roName, this, RenderObjectType::Geometry,
                                                displayMesh, rt );
    ro->setTransparent( isTransparent );

    setupIO( m_contentName );
    m_meshIndex = addRenderObject( ro );
}

Ra::Core::Utils::Index GeometryComponent::getRenderObjectIndex() const {
    return m_meshIndex;
}

const Ra::Core::Geometry::TriangleMesh& GeometryComponent::getMesh() const {
    return getDisplayMesh().getGeometry();
}

void GeometryComponent::setDeformable( bool b ) {
    this->m_deformable = b;
}

void GeometryComponent::setContentName( const std::string& name ) {
    this->m_contentName = name;
}

void GeometryComponent::setupIO( const std::string& id ) {
    ComponentMessenger::CallbackTypes<Ra::Core::Geometry::TriangleMesh>::Getter cbOut =
        std::bind( &GeometryComponent::getMeshOutput, this );
    ComponentMessenger::getInstance()->registerOutput<Ra::Core::Geometry::TriangleMesh>(
        getEntity(), this, id, cbOut );

    ComponentMessenger::CallbackTypes<Ra::Core::Geometry::TriangleMesh>::ReadWrite cbRw =
        std::bind( &GeometryComponent::getMeshRw, this );
    ComponentMessenger::getInstance()->registerReadWrite<Ra::Core::Geometry::TriangleMesh>(
        getEntity(), this, id, cbRw );

    ComponentMessenger::CallbackTypes<Ra::Core::Utils::Index>::Getter roOut =
        std::bind( &GeometryComponent::roIndexRead, this );
    ComponentMessenger::getInstance()->registerOutput<Ra::Core::Utils::Index>( getEntity(), this,
                                                                               id, roOut );

    ComponentMessenger::CallbackTypes<Ra::Core::Vector3Array>::ReadWrite vRW =
        std::bind( &GeometryComponent::getVerticesRw, this );
    ComponentMessenger::getInstance()->registerReadWrite<Ra::Core::Vector3Array>( getEntity(), this,
                                                                                  id + "v", vRW );

    ComponentMessenger::CallbackTypes<Ra::Core::Vector3Array>::ReadWrite nRW =
        std::bind( &GeometryComponent::getNormalsRw, this );
    ComponentMessenger::getInstance()->registerReadWrite<Ra::Core::Vector3Array>( getEntity(), this,
                                                                                  id + "n", nRW );

    ComponentMessenger::CallbackTypes<TriangleArray>::ReadWrite tRW =
        std::bind( &GeometryComponent::getTrianglesRw, this );
    ComponentMessenger::getInstance()->registerReadWrite<TriangleArray>( getEntity(), this,
                                                                         id + "t", tRW );

    if ( m_deformable )
    {
        ComponentMessenger::CallbackTypes<Ra::Core::Geometry::TriangleMesh>::Setter cbIn =
            std::bind( &GeometryComponent::setMeshInput, this, std::placeholders::_1 );
        ComponentMessenger::getInstance()->registerInput<Ra::Core::Geometry::TriangleMesh>(
            getEntity(), this, id, cbIn );
    }
}

const Mesh& GeometryComponent::getDisplayMesh() const {
    return *( getRoMgr()->getRenderObject( getRenderObjectIndex() )->getMesh() );
}

Mesh& GeometryComponent::getDisplayMesh() {
    return *( getRoMgr()->getRenderObject( getRenderObjectIndex() )->getMesh() );
}

const Ra::Core::Geometry::TriangleMesh* GeometryComponent::getMeshOutput() const {
    return &( getMesh() );
}

Ra::Core::Geometry::TriangleMesh* GeometryComponent::getMeshRw() {
    getDisplayMesh().setDirty( Mesh::VERTEX_POSITION );
    getDisplayMesh().setDirty( Mesh::VERTEX_NORMAL );
    getDisplayMesh().setDirty( Mesh::INDEX );
    return &( getDisplayMesh().getGeometry() );
}

void GeometryComponent::setMeshInput( const Core::Geometry::TriangleMesh* meshptr ) {
    CORE_ASSERT( meshptr, " Input is null" );
    CORE_ASSERT( m_deformable, "Mesh is not deformable" );

    Mesh& displayMesh = getDisplayMesh();
    displayMesh.loadGeometry( *meshptr );
}

Ra::Core::Geometry::TriangleMesh::PointAttribHandle::Container* GeometryComponent::getVerticesRw() {
    getDisplayMesh().setDirty( Mesh::VERTEX_POSITION );
    return &( getDisplayMesh().getGeometry().vertices() );
}

Ra::Core::Geometry::TriangleMesh::NormalAttribHandle::Container* GeometryComponent::getNormalsRw() {
    getDisplayMesh().setDirty( Mesh::VERTEX_NORMAL );
    return &( getDisplayMesh().getGeometry().normals() );
}

Ra::Core::VectorArray<Ra::Core::Vector3ui>* GeometryComponent::getTrianglesRw() {
    getDisplayMesh().setDirty( Mesh::INDEX );
    return &( getDisplayMesh().getGeometry().m_triangles );
}

const Ra::Core::Utils::Index* GeometryComponent::roIndexRead() const {
    return &m_meshIndex;
}

} // namespace Engine
} // namespace Ra