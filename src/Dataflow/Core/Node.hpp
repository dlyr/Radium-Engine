#pragma once
#include "Core/Containers/VariableSet.hpp"
#include "Core/Utils/Index.hpp"
#include <Dataflow/RaDataflow.hpp>

#include <Dataflow/Core/EditableParameter.hpp>
#include <Dataflow/Core/PortIn.hpp>
#include <Dataflow/Core/PortOut.hpp>
#include <Dataflow/Core/TypeDemangler.hpp>

#include <nlohmann/json.hpp>

#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <type_traits> // conditional, is_same
#include <typeinfo>
#include <vector>

namespace Ra {
namespace Dataflow {
namespace Core {

/** \brief Base abstract class for all the nodes added and used by the node system.
 * A node represent a function acting on some input data and generating some outputs.
 * To build a computation graph, nodes should be added to the graph, which is itself a node
 * (\see Ra::Dataflow::Core::DataflowGraph) and linked together through their input/output port.
 *
 * Nodes computes their function using the input data collecting from the input ports,
 * in an evaluation context (possibly empty) defined by their internal data to generate results
 * sent to their output ports.
 *
 */
class RA_DATAFLOW_API Node
{
  public:
    using PortIndex = Ra::Core::Utils::Index;
    template <typename Port>
    using PortPtr = std::shared_ptr<Port>;
    template <typename Port>
    using PortCollection = std::vector<Port>;
    template <typename Port>
    using PortRawPtr = typename PortPtr<Port>::element_type*;
    template <typename Type>
    using PortInPtr = PortPtr<PortIn<Type>>;
    template <typename Type>
    using PortInRawPtr = typename PortInPtr<Type>::element_type*;
    template <typename Type>
    using PortOutPtr = PortPtr<PortOut<Type>>;
    template <typename Type>
    using PortOutRawPtr = typename PortOutPtr<Type>::element_type*;

    using PortBasePtr       = PortPtr<PortBase>;
    using PortBaseInPtr     = PortPtr<PortBaseIn>;
    using PortBaseOutPtr    = PortPtr<PortBaseOut>;
    using PortBaseRawPtr    = PortRawPtr<PortBase>;
    using PortBaseInRawPtr  = PortRawPtr<PortBaseIn>;
    using PortBaseOutRawPtr = PortRawPtr<PortBaseOut>;

    using PortBaseCollection    = std::vector<PortBasePtr>;
    using PortBaseInCollection  = std::vector<PortBaseInPtr>;
    using PortBaseOutCollection = std::vector<PortBaseOutPtr>;

    template <typename Port>
    using IndexAndPort = std::pair<PortIndex, Port>;

    /// \name Constructors
    /// @{
    /// \brief delete default constructors.
    /// \see https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rc-copy-virtual
    Node()                         = delete;
    Node( const Node& )            = delete;
    Node& operator=( const Node& ) = delete;
    /// @}

    /// \brief make Node a base abstract class
    virtual ~Node() = default;

    /// \brief Two nodes are considered equal if there type and instance names are the same.
    bool operator==( const Node& o_node );

    /// \name Function execution control
    /// @{
    /// \brief Initializes the node content
    /// The init() function should be called once at the beginning of the lifetime of the node by
    /// the owner of the node (the graph which contains the node).
    /// Its goal is to initialize the node's internal data if any.
    /// The base version do nothing.
    virtual void init();

    /// \brief Compile the node to check its validity
    /// Only nodes defining a full computation graph will need to override this method.
    /// The base version do nothing.
    /// \return the compilation status
    virtual bool compile();

    /// \brief Executes the node.
    /// Execute the node function on the input ports (to be fetched) and write the results to the
    /// output ports.
    /// \return the execution status.
    virtual bool execute() = 0;

    /// \brief delete the node content
    /// The destroy() function is called once at the end of the lifetime of the node.
    /// Its goal is to free the internal data that have been allocated.
    virtual void destroy();
    /// @}

    /// \name Control the interfaces of the nodes (inputs, outputs, internal data, ...)
    /// @{

    /// \brief Get a port by its name
    /// \param type either "in" or "out", the directional type of the port
    /// \param name
    /// \return the index to access the port and a raw ptr to the port.
    IndexAndPort<PortBaseRawPtr> getPortByName( const std::string& type,
                                                const std::string& name ) const;
    IndexAndPort<PortBaseInRawPtr> getInputByName( const std::string& name ) const;
    IndexAndPort<PortBaseOutRawPtr> getOutputByName( const std::string& name ) const;

    /// \brief Get a port by its index
    /// \param type either "in" or "out", the directional type of the port
    /// \param idx
    /// \return an alias pointer on the requested port if it exists, nullptr else
    PortBaseRawPtr getPortByIndex( const std::string& type, PortIndex idx ) const;
    auto getInputByIndex( PortIndex idx ) const { return getPortBase( m_inputs, idx ); }
    auto getOutputByIndex( PortIndex idx ) const { return getPortBase( m_outputs, idx ); }

    template <typename T>
    auto getInputByIndex( PortIndex idx ) const {
        return getPort<T>( m_inputs, idx );
    }

    /// \brief Gets a output typed port by its index
    /// \param idx
    /// \return
    template <typename T>
    auto getOutputByIndex( PortIndex idx ) const {
        return getPort<T>( m_outputs, idx );
    }

    /// \brief Gets the in ports of the node.
    /// Input ports are own to the node.
    const PortBaseInCollection& getInputs() const;

    /// \brief Gets the out ports of the node.
    /// Output ports are own to the node.
    const PortBaseOutCollection& getOutputs() const;

    /// \brief Build the interface ports of the node
    /// Derived node can override the default implementation that build an interface port for each
    /// input or output port (e.g. if several inputs have the same type T, make an interface that is
    /// a vector of T*)
    virtual const std::vector<PortBase*>& buildInterfaces( Node* parent );

    /// \brief Get the interface ports of the node
    const std::vector<PortBase*>& getInterfaces() const;

    /// \brief Gets the editable parameters of the node.
    /// used only by the node editor gui to build the editon widget
    const std::vector<std::unique_ptr<EditableParameterBase>>& getEditableParameters();
    /// @}

    /// \name Identification methods
    /// @{
    /// \brief Gets the type name of the node.
    const std::string& getTypeName() const;

    /// \brief Gets the instance name of the node.
    const std::string& getInstanceName() const;

    /// \brief Sets the instance name (rename) the node
    void setInstanceName( const std::string& newName );

    /// @}

    /// \name Serialization of a node
    /// @{
    /// \todo : specify the json format for nodes and what is expected from the following methods

    /// \brief serialize the content of the node.
    /// Fill the given json object with the json representation of the concrete node.
    void toJson( nlohmann::json& data ) const;

    /// \brief unserialized the content of the node.
    /// Fill the node from its json representation
    bool fromJson( const nlohmann::json& data );

    /// \brief Add a metadata to the node to store application specific information.
    /// used, e.g. by the node editor gui to save node position in the graphical canvas.
    void addJsonMetaData( const nlohmann::json& data );

    /// \brief Give access to extra json data stored on the node.
    const nlohmann::json& getJsonMetaData();
    /// @}

    /// \brief Returns the demangled type name of the node or any human readable representation of
    /// the type name.
    /// This is a public static member each node must define to be serializable
    static const std::string& getTypename();

    inline bool isInitialized() const { return m_initialized; }
    Ra::Core::VariableSet& getParameters() { return m_parameters; }

  protected:
    /// Construct the base node given its name and type
    /// \param instanceName The name of the node
    /// \param typeName The type name of the node
    Node( const std::string& instanceName, const std::string& typeName );

    ///\brief Gets the Port By Name
    ///
    ///\tparam PortType PortBaseIn or PortBaseOut
    ///\param ports
    ///\param name The named used to add the port.
    ///\return IndexAndPort<PortRawPtr<PortType>> the index to access the port and a raw ptr to
    /// the port.
    template <typename PortType>
    IndexAndPort<PortRawPtr<PortType>>
    getPortByName( const PortCollection<PortPtr<PortType>>& ports, const std::string& name ) const {
        auto itp = std::find_if(
            ports.begin(), ports.end(), [n = name]( const auto& p ) { return p->getName() == n; } );
        PortRawPtr<PortType> fprt { nullptr };
        PortIndex portIndex;
        if ( itp != ports.cend() ) {
            fprt      = itp->get();
            portIndex = std::distance( ports.begin(), itp );
        }
        return { portIndex, fprt };
    }

    ///\brief Gets the PortBase In or Out by its index
    ///
    ///\tparam PortType PortBaseIn or PortBaseOut
    ///\param ports
    ///\param idx
    ///\return PortRawPtr<PortType>
    template <typename PortType>
    PortRawPtr<PortType> getPortBase( const PortCollection<PortPtr<PortType>>& ports,
                                      PortIndex idx ) const {
        if ( 0 <= idx && size_t( idx ) < ports.size() ) { return ports[idx].get(); }
        return nullptr;
    }

    ///\brief Gets the PortBase In or Out by its index
    ///
    ///\tparam PortType PortBaseIn or PortBaseOut
    ///\param ports
    ///\param idx
    ///\return PortRawPtr<PortType>
    template <typename PortType>
    PortRawPtr<PortType> getPortBaseNoCheck( const PortCollection<PortPtr<PortType>>& ports,
                                             PortIndex idx ) const {
        return ports[idx].get();
    }

    ///\brief Gets a port in a collection by its index.
    ///
    ///\tparam T The contained type.
    ///\tparam PortType PortBaseIn or PortBaseOut
    ///\param ports The port collection
    ///\param idx
    ///\return auto A raw ptr to the port typed in or out.
    template <typename T, typename PortType>
    auto getPort( const PortCollection<PortPtr<PortType>>& ports, PortIndex idx ) const {
        return static_cast<std::conditional<
            /*if*/ std::is_same<PortBaseIn, PortType>::value,
            /*then*/ PortInRawPtr<T>,
            /*else*/ PortOutRawPtr<T>>>( getPortBase( ports, idx ) );
    }

    /// internal json representation of the Node.
    /// Default implementation warn about unsupported deserialization.
    /// Effective deserialzation must be implemented by inheriting classes.
    /// Be careful with template specialization and function member overriding in derived classes.
    virtual bool fromJsonInternal( const nlohmann::json& ) {
        LOG( Ra::Core::Utils::logDEBUG ) << "Unable deserializing " << getTypeName() << ".";
        return true;
    }

    /// internal json representation of the Node.
    /// Default implementation warn about unsupported serialization.
    /// Effective serialzation must be implemented by inheriting classes.
    /// Be careful with template specialization and function member overriding in derived classes.
    virtual void toJsonInternal( nlohmann::json& data ) const {
        data["comment"] = std::string { "Could not serialized " } + getTypeName();
        LOG( Ra::Core::Utils::logDEBUG )
            << "Unable to save data when serializing a " << getTypeName() << ".";
    }

    /// Adds an in port to the node.
    /// This function checks if the port is an input port.
    /// \param in The in port to add.
    bool addInput( PortBaseIn* in );
    PortIndex addInput( PortBaseInPtr in );
    PortIndex addOutput( PortBaseOutPtr out );
    template <typename PortType>
    PortIndex addPort( PortCollection<PortPtr<PortType>>&, PortPtr<PortType> port );

    template <typename T, typename... U>
    PortInPtr<T> addInputPort( U&&... u ) {
        auto idx = addInput( std::make_shared<PortIn<T>>( this, std::forward<U>( u )... ) );
        return getInputPort<T>( idx );
    }
    template <typename T, typename... U>
    PortOutPtr<T> addOutputPort( U&&... u ) {
        auto idx = addOutput( std::make_shared<PortOut<T>>( this, std::forward<U>( u )... ) );
        return getOutputPort<T>( idx );
    }

    template <typename T>
    PortInPtr<T> getInputPort( PortIndex idx ) {
        return std::static_pointer_cast<PortIn<T>>( m_inputs[idx] );
    }

    template <typename T>
    PortOutPtr<T> getOutputPort( PortIndex idx ) {
        return std::static_pointer_cast<PortOut<T>>( m_outputs[idx] );
    }

    PortBaseInPtr getInputPort( PortIndex idx ) { return m_inputs[idx]; }
    PortBaseOutPtr getOutputPort( PortIndex idx ) { return m_outputs[idx]; }

    ///\todo remove these it not needed by dataflow graph
    /// \brief remove the given input port from the managed input ports
    /// \param in the port to remove
    /// \return true if the port was removed (the in pointer is the set to nullptr), false else
    bool removeInput( PortBaseIn*& in );
    void removeInput( PortIndex idx ) { m_inputs[idx].reset(); }

    /// Adds an out port to the node and the data associated with it.
    /// This function checks if there is no out port with the same name already associated with
    /// this node. \param out The in port to add. \param data The data associated with the port.
    template <typename T>
    void addOutput( PortOut<T>* out, T* data );

    /// \brief remove the given output port from the managed input ports
    /// \param out the port to remove
    /// \return true if the port was removed (the out pointer is the set to nullptr), false else
    bool removeOutput( PortBaseOut*& out );
    void removeOutput( PortIndex idx ) { m_outputs[idx].reset(); }

    /// \brief Adds an editable parameter to the node if it does not already exist.
    /// \note the node will take ownership of the editable object.
    /// \param editableParameter The editable parameter to add.
    bool addEditableParameter( EditableParameterBase* editableParameter );

    /// Remove an editable parameter to the node if it does exist.
    /// \param name The name of the editable parameter to remove.
    /// \return true if the editable parameter is found and removed.
    bool removeEditableParameter( const std::string& name );

    /// \brief get a typed reference to the editable parameter.
    /// \tparam E The type of the expected editable parameter.
    /// \param name The name of the editable parameter to get.
    /// \return the pointer to the editable parameter if any, nullptr if not.
    template <typename E>
    EditableParameter<E>* getEditableParameter( const std::string& name );

    /// \brief Flag that checks if the node is already initialized
    bool m_initialized { false };
    /// The type name of the node. Initialized once at construction
    std::string m_typeName;
    /// The instance name of the node
    std::string m_instanceName;
    /// The in ports of the node (own by the node)
    PortCollection<PortPtr<PortBaseIn>> m_inputs;
    /// The out ports of the node  (own by the node)
    PortCollection<PortPtr<PortBaseOut>> m_outputs;
    /// The reflected ports of the node if it is only a source or sink node.
    /// This stores only aliases as interface ports will belong to the parent
    /// node (i.e. the graph this node belongs to)
    PortCollection<PortPtr<PortBase>> m_interface;

    /// The editable parameters of the node
    /// \todo replace this by a Ra::Core::VariableSet
    std::vector<std::unique_ptr<EditableParameterBase>> m_editableParameters;
    // start transition
    Ra::Core::VariableSet m_parameters;

    /// Additional data on the node, added by application or gui or ...
    nlohmann::json m_extraJsonData;
};

// -----------------------------------------------------------------
// ---------------------- inline methods ---------------------------

inline void Node::init() {
    m_initialized = true;
}

inline void Node::destroy() {
    m_interface.clear();
    m_inputs.clear();
    m_outputs.clear();
    m_editableParameters.clear();
}

inline const nlohmann::json& Node::getJsonMetaData() {
    return m_extraJsonData;
}

inline const std::string& Node::getTypeName() const {
    return m_typeName;
}

inline const std::string& Node::getInstanceName() const {
    return m_instanceName;
}

inline void Node::setInstanceName( const std::string& newName ) {
    m_instanceName = newName;
}

inline const Node::PortBaseInCollection& Node::getInputs() const {
    return m_inputs;
}

inline const Node::PortBaseOutCollection& Node::getOutputs() const {
    return m_outputs;
}

inline const std::vector<PortBase*>& Node::buildInterfaces( Node* parent ) {
    m_interface.clear();
    m_interface.shrink_to_fit();
    if ( !m_inputs.empty() ) {
        m_interface.reserve( m_inputs.size() );
        for ( const auto& p : m_inputs ) {
            m_interface.emplace_back(
                p->reflect( parent, getInstanceName() + '_' + p->getName() ) );
        }
    }
    else {
        m_interface.reserve( m_outputs.size() );
        for ( const auto& p : m_outputs ) {
            m_interface.emplace_back(
                p->reflect( parent, getInstanceName() + '_' + p->getName() ) );
        }
    }
    return m_interface;
}

inline const std::vector<PortBase*>& Node::getInterfaces() const {
    return m_interface;
}

inline const std::vector<std::unique_ptr<EditableParameterBase>>& Node::getEditableParameters() {
    return m_editableParameters;
}

inline bool Node::operator==( const Node& o_node ) {
    return ( m_typeName == o_node.getTypeName() ) && ( m_instanceName == o_node.getInstanceName() );
}

inline bool Node::addInput( PortBaseIn* in ) {
    if ( !in->is_input() ) { return false; }
    bool found = false;
    for ( auto& input : m_inputs ) {
        if ( input->getName() == in->getName() ) { found = true; }
    }
    if ( !found ) { m_inputs.emplace_back( in ); }
    return !found;
}

template <typename PortType>
inline Node::PortIndex Node::addPort( PortCollection<PortPtr<PortType>>& ports,
                                      PortPtr<PortType> port ) {
    PortIndex idx;
    // look for a free slot
    auto it = std::find_if( ports.begin(), ports.end(), []( const auto& port ) { return !port; } );
    if ( it != ports.end() ) {
        it->swap( port );
        idx = std::distance( ports.begin(), it );
    }
    else {
        ports.push_back( std::move( port ) );
        idx = ports.size() - 1;
    }
    return idx;
}

inline Node::PortIndex Node::addInput( PortBaseInPtr in ) {
    CORE_ASSERT( in->is_input(), "in port must be is_input" );
    return addPort( m_inputs, std::move( in ) );
}

inline Node::PortIndex Node::addOutput( PortBaseOutPtr out ) {
    CORE_ASSERT( !out->is_input(), "out port must be not is_input" );
    return addPort( m_outputs, std::move( out ) );
}

inline bool Node::removeInput( PortBaseIn*& in ) {
    auto itP = std::find_if(
        m_inputs.begin(), m_inputs.end(), [in]( const auto& p ) { return p.get() == in; } );
    if ( itP != m_inputs.end() ) {
        m_inputs.erase( itP );
        in = nullptr;
        return true;
    }
    return false;
}

template <typename T>
void Node::addOutput( PortOut<T>* out, T* data ) {
    bool found = false;
    for ( auto& output : m_outputs ) {
        if ( output->getName() == out->getName() ) { found = true; }
    }
    if ( !found ) {
        m_outputs.emplace_back( out );
        out->setData( data );
    }
}

inline bool Node::removeOutput( PortBaseOut*& out ) {
    auto outP = std::find_if(
        m_outputs.begin(), m_outputs.end(), [out]( const auto& p ) { return p.get() == out; } );
    if ( outP != m_outputs.end() ) {
        m_outputs.erase( outP );
        out = nullptr;
        return true;
    }
    return false;
}

inline bool Node::addEditableParameter( EditableParameterBase* editableParameter ) {
    auto edIt = std::find_if(
        m_editableParameters.begin(),
        m_editableParameters.end(),
        [name = editableParameter->getName()]( const auto& p ) { return p->getName() == name; } );
    if ( edIt == m_editableParameters.end() ) {
        m_editableParameters.emplace_back( editableParameter );
    }
    return edIt == m_editableParameters.end();
}

inline bool Node::removeEditableParameter( const std::string& name ) {
    bool found = false;
    auto it    = m_editableParameters.begin();
    while ( it != m_editableParameters.end() ) {
        if ( ( *it ).get()->getName() == name ) {
            m_editableParameters.erase( it );
            found = true;
            break;
        }
        ++it;
    }
    return found;
}

template <typename E>
inline EditableParameter<E>* Node::getEditableParameter( const std::string& name ) {
    auto it = m_editableParameters.begin();
    while ( it != m_editableParameters.end() ) {
        if ( ( *it ).get()->getName() == name ) {
            auto p = dynamic_cast<EditableParameter<E>*>( ( *it ).get() );
            if ( p != nullptr ) { return *p; }
        }
        ++it;
    }
    return nullptr;
}

inline const std::string& Node::getTypename() {
    static std::string demangledTypeName { "Abstract Node" };
    return demangledTypeName;
}

inline bool Node::compile() {
    return true;
}

} // namespace Core
} // namespace Dataflow
} // namespace Ra
