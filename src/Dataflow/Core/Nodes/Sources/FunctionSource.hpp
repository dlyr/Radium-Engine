#pragma once
#include "Dataflow/Core/NodeFactory.hpp"
#pragma once
#include <Dataflow/Core/Node.hpp>

#include <iostream>
#include <utility>

namespace Ra {
namespace Dataflow {
namespace Core {
namespace Sources {

/**
 * \brief Node that deliver a std::function<R( Args... )>
 * \tparam R return type of the function
 * \tparam Type of the function arguments
 */
template <class R, class... Args>
class FunctionSourceNode : public Node
{

  public:
    using function_type = std::function<R( Args... )>;

    explicit FunctionSourceNode( const std::string& name ) :
        FunctionSourceNode( name, FunctionSourceNode<R, Args...>::getTypename() ) {}

    bool execute() override;

    /** \brief Set the function to be delivered by the node.
     * @param data
     */
    void setData( function_type data );

    /**
     * \brief Get the delivered data
     * @return The non owning pointer (alias) to the delivered data.
     */
    function_type* getData() const;

    Node::PortOutPtr<function_type> getFunctionPort() { return m_portOut; }

  protected:
    FunctionSourceNode( const std::string& instanceName, const std::string& typeName );

    bool fromJsonInternal( const nlohmann::json& data ) override {
        return Node::fromJsonInternal( data );
    }
    void toJsonInternal( nlohmann::json& data ) const override { Node::toJsonInternal( data ); }

    /// @{
    /// The data provided by the node
    function_type m_localData { []( Args... ) { return R {}; } };

    Ra::Core::VariableSet::VariableHandle<function_type> m_dataHandle;

    /// @}

    /// Alias to the output port
    Node::PortOutPtr<function_type> m_portOut;

  public:
    static const std::string& getTypename();
};

// -----------------------------------------------------------------
// ---------------------- inline methods ---------------------------

template <class R, class... Args>
FunctionSourceNode<R, Args...>::FunctionSourceNode( const std::string& instanceName,
                                                    const std::string& typeName ) :
    Node( instanceName, typeName ),
    m_dataHandle {
        m_parameters.insertVariable<function_type>( "data", []( Args... ) { return R {}; } )
            .first },
    m_portOut { addOutputPort<function_type>( &m_dataHandle->second, "f" ) } {}

template <class R, class... Args>
bool FunctionSourceNode<R, Args...>::execute() {
    return true;
}

template <class R, class... Args>
void FunctionSourceNode<R, Args...>::setData( function_type data ) {
    m_dataHandle->second = std::move( data );
    m_portOut->setData( &m_dataHandle->second );
}

template <class R, class... Args>
typename FunctionSourceNode<R, Args...>::function_type*
FunctionSourceNode<R, Args...>::getData() const {
    return m_dataHandle->second;
}

template <class R, class... Args>
const std::string& FunctionSourceNode<R, Args...>::getTypename() {
    static std::string demangledTypeName =
        std::string { "Source<" } + Ra::Dataflow::Core::simplifiedDemangledType<function_type>() +
        ">";
    return demangledTypeName;
}

} // namespace Sources
} // namespace Core
} // namespace Dataflow
} // namespace Ra
