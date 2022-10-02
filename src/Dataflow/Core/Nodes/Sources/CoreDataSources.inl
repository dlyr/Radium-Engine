#pragma once
#include <Dataflow/Core/Nodes/Sources/CoreDataSources.hpp>

#include <Core/Utils/TypesUtils.hpp>

namespace Ra {
namespace Dataflow {
namespace Core {
namespace Sources {

/* Source bool */
inline BooleanValueSource::BooleanValueSource( const std::string& name ) :
    SingleDataSourceNode( name, BooleanValueSource::getTypename() ) {
    setEditable( "boolean" );
}

inline void BooleanValueSource::toJsonInternal( nlohmann::json& data ) const {
    data["boolean"] = *getData();
}

inline void BooleanValueSource::fromJsonInternal( const nlohmann::json& data ) {
    if ( data.contains( "boolean" ) ) {
        bool v = data["boolean"];
        setData( v );
    }
}

inline const std::string& BooleanValueSource::getTypename() {
    static std::string demangledTypeName { "Source<bool>" };
    return demangledTypeName;
}

/* Source int */
inline IntValueSource::IntValueSource( const std::string& name ) :
    SingleDataSourceNode( name, IntValueSource::getTypename() ) {
    setEditable( "value" );
}

inline void IntValueSource::toJsonInternal( nlohmann::json& data ) const {
    data["value"] = *getData();
}

inline void IntValueSource::fromJsonInternal( const nlohmann::json& data ) {
    if ( data.contains( "value" ) ) {
        int v = data["value"];
        setData( v );
    }
}

inline const std::string& IntValueSource::getTypename() {
    static std::string demangledTypeName { "Source<int>" };
    return demangledTypeName;
}

/* Source uint */
inline UIntValueSource::UIntValueSource( const std::string& name ) :
    SingleDataSourceNode( name, UIntValueSource::getTypename() ) {
    setEditable( "value" );
}

inline void UIntValueSource::toJsonInternal( nlohmann::json& data ) const {
    data["value"] = *getData();
}

inline void UIntValueSource::fromJsonInternal( const nlohmann::json& data ) {
    if ( data.contains( "value" ) ) {
        unsigned int v = data["value"];
        setData( v );
    }
}

inline const std::string& UIntValueSource::getTypename() {
    static std::string demangledTypeName { "Source<uint>" };
    return demangledTypeName;
}

/* Source Scalar */
inline const std::string& ScalarValueSource::getTypename() {
    static std::string demangledTypeName { "Source<Scalar>" };
    return demangledTypeName;
}

inline ScalarValueSource::ScalarValueSource( const std::string& name ) :
    SingleDataSourceNode( name, ScalarValueSource::getTypename() ) {
    setEditable( "number" );
}

inline void ScalarValueSource::toJsonInternal( nlohmann::json& data ) const {
    data["number"] = *getData();
}

inline void ScalarValueSource::fromJsonInternal( const nlohmann::json& data ) {
    if ( data.contains( "number" ) ) {
        Scalar v = data["number"];
        setData( v );
    }
}

/* Source Color */
inline const std::string& ColorSourceNode::getTypename() {
    static std::string demangledTypeName { "Source<RaColor>" };
    return demangledTypeName;
}

inline ColorSourceNode::ColorSourceNode( const std::string& name ) :
    SingleDataSourceNode( name, ColorSourceNode::getTypename() ) {
    setEditable( "color" );
}

inline void ColorSourceNode::toJsonInternal( nlohmann::json& data ) const {
    auto c = Ra::Core::Utils::Color::linearRGBTosRGB( *getData() );
    std::array<Scalar, 3> color { { c.x(), c.y(), c.z() } };
    data["color"] = color;
}

inline void ColorSourceNode::fromJsonInternal( const nlohmann::json& data ) {
    if ( data.contains( "color" ) ) {
        std::array<Scalar, 3> c = data["color"];
        auto v =
            Ra::Core::Utils::Color::sRGBToLinearRGB( Ra::Core::Utils::Color( c[0], c[1], c[2] ) );
        setData( v );
    }
}

} // namespace Sources
} // namespace Core
} // namespace Dataflow
} // namespace Ra
