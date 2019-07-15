#include "Mesh.hpp"
#include <globjects/Buffer.h>

namespace Ra {
namespace Engine {

void Mesh::setRenderMode( MeshRenderMode mode ) {
    m_renderMode = mode;
    updatePickingRenderMode();
}

const Core::Geometry::AbstractGeometry& Mesh::getGeometry() const {
    return m_mesh;
}

Core::Geometry::AbstractGeometry& Mesh::getGeometry() {
    return m_mesh;
}

const Core::Geometry::TriangleMesh& Mesh::getTriangleMesh() const {
    return m_mesh;
}

Core::Geometry::TriangleMesh& Mesh::getTriangleMesh() {
    return m_mesh;
}

const Core::Vector3Array& Mesh::getData( const Mesh::Vec3Data& type ) const {
    using Handle  = Core::Geometry::TriangleMesh::Vec3AttribHandle;
    auto name     = getAttribName( type );
    Handle handle = m_mesh.getAttribHandle<Core::Vector3>( name );
    if ( !m_mesh.isValid( handle ) ) return m_dummy3;
    return m_mesh.getAttrib( handle ).data();
}

const Core::Vector4Array& Mesh::getData( const Mesh::Vec4Data& type ) const {
    using Handle  = Core::Geometry::TriangleMesh::Vec4AttribHandle;
    auto name     = getAttribName( type );
    Handle handle = m_mesh.getAttribHandle<Core::Vector4>( name );
    if ( !m_mesh.isValid( handle ) ) return m_dummy4;
    return m_mesh.getAttrib( handle ).data();
}

void Mesh::setDirty( const Mesh::MeshData& type ) {
    auto name = getAttribName( type );
    auto itr  = m_handleToBuffer.find( name );
    if ( itr == m_handleToBuffer.end() )
    {
        m_handleToBuffer[name] = m_dataDirty.size();
        m_dataDirty.push_back( true );
        m_vbos.emplace_back( nullptr );
    }
    else
        m_dataDirty[itr->second] = true;

    m_isDirty = true;
}

void Mesh::setDirty( const Vec3Data& type ) {
    auto name = getAttribName( type );
    auto itr  = m_handleToBuffer.find( name );
    if ( itr == m_handleToBuffer.end() )
    {
        m_handleToBuffer[name] = m_dataDirty.size();
        m_dataDirty.push_back( true );
        m_vbos.push_back( nullptr );
    }
    else
        m_dataDirty[itr->second] = true;

    m_isDirty = true;
}

void Mesh::setDirty( const Vec4Data& type ) {
    auto name = getAttribName( type );
    auto itr  = m_handleToBuffer.find( name );
    if ( itr == m_handleToBuffer.end() )
    {
        m_handleToBuffer[name] = m_dataDirty.size();
        m_dataDirty.push_back( true );
        m_vbos.push_back( nullptr );
    }
    else
        m_dataDirty[itr->second] = true;

    m_isDirty = true;
}

std::string Mesh::getAttribName( MeshData type ) {
    if ( type == VERTEX_POSITION ) return {"in_position"};
    if ( type == VERTEX_NORMAL ) return {"in_normal"};
    return {"indices but should not happend"};
}

std::string Mesh::getAttribName( Vec3Data type ) {
    return std::string( "Vec3_attr_" ) + std::to_string( uint( type ) );
}

std::string Mesh::getAttribName( Vec4Data type ) {
    return std::string( "Vec4_attr_" ) + std::to_string( uint( type ) );
}

// void Mesh::colorize( const Core::Utils::Color& color ) {
//    Core::Vector4Array colors( getTriangleMesh().vertices().size(), color );
//    addData( Engine::Mesh::VERTEX_COLOR, colors );
//}

} // namespace Engine
} // namespace Ra
