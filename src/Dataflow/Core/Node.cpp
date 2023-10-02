#include <Dataflow/Core/Node.hpp>

#include <Core/Utils/Log.hpp>

namespace Ra {
namespace Dataflow {
namespace Core {

using namespace Ra::Core::Utils;

Node::Node( const std::string& instanceName, const std::string& typeName ) :
    m_typeName { typeName }, m_instanceName { instanceName } {}

bool Node::fromJson( const nlohmann::json& data ) {
    if ( data.empty() ) {
        // This is to avoid wrong error message when creating node from the editor
        return true;
    }

    if ( data.contains( "instance" ) ) { m_instanceName = data["instance"]; }
    else {
        LOG( logERROR ) << "Missing required instance name when loading node " << m_instanceName;
        return false;
    }
    // get the common content of the Node from the json data
    bool loaded = false;
    if ( data.contains( "model" ) ) {
        // get the specific concrete node information
        const auto& datamodel = data["model"];
        loaded                = fromJsonInternal( datamodel );
    }
    else {
        LOG( logERROR ) << "Missing required model when loading a Dataflow::Node";
        loaded = false;
    }
    // get the supplemental information related to application/gui/...
    for ( auto& [key, value] : data.items() ) {
        if ( key != "instance" && key != "model" ) { m_extraJsonData.emplace( key, value ); }
    }
    return loaded;
}

void Node::toJson( nlohmann::json& data ) const {

    // write the common content of the Node to the json data
    data["instance"] = m_instanceName;

    nlohmann::json model;
    model["name"] = m_typeName;

    // Fill the specific concrete node information (model instance)
    toJsonInternal( model );
    data.emplace( "model", model );

    // store the supplemental information related to application/gui/...
    for ( auto& [key, value] : m_extraJsonData.items() ) {
        if ( key != "instance" && key != "model" ) { data.emplace( key, value ); }
    }
}

void Node::addJsonMetaData( const nlohmann::json& data ) {
    for ( auto& [key, value] : data.items() ) {
        m_extraJsonData[key] = value;
    }
}

Node::IndexAndPort<Node::PortBaseRawPtr> Node::getPortByName( const std::string& type,
                                                              const std::string& name ) const {
    if ( type == "in" ) { return getPortByName( m_inputs, name ); }
    return getPortByName( m_outputs, name );
}

Node::IndexAndPort<Node::PortBaseInRawPtr> Node::getInputByName( const std::string& name ) const {
    return getPortByName( m_inputs, name );
}

Node::IndexAndPort<Node::PortBaseOutRawPtr> Node::getOutputByName( const std::string& name ) const {
    return getPortByName( m_outputs, name );
}

PortBase* Node::getPortByIndex( const std::string& type, PortIndex idx ) const {
    if ( type == "in" ) return getPortBase( m_inputs, idx );
    return getPortBase( m_outputs, idx );
}

} // namespace Core
} // namespace Dataflow
} // namespace Ra
