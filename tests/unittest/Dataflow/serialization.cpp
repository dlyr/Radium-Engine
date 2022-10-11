#include <catch2/catch.hpp>

#include <string>
#include <vector>

#include <iostream>

#include <Dataflow/Core/DataflowGraph.hpp>
#include <Dataflow/Core/Nodes/CoreBuiltInsNodes.hpp>

TEST_CASE( "Dataflow/Core/DataflowGraph", "[Dataflow][Core][DataflowGraph]" ) {
    SECTION( "Serialization of a graph" ) {

        using namespace Ra::Dataflow::Core;
        using DataType = Scalar;
        DataflowGraph g { "original graph" };

        auto source_a = new Sources::SingleDataSourceNode<DataType>( "a" );
        g.addNode( source_a );
        auto a        = g.getDataSetter( "a_to" );
        auto source_b = new Sources::SingleDataSourceNode<DataType>( "b" );
        g.addNode( source_b );
        auto b    = g.getDataSetter( "b_to" );
        auto sink = new Sinks::SinkNode<DataType>( "r" );
        g.addNode( sink );
        auto r                       = g.getDataGetter( "r_from" );
        using TestNode               = Functionals::BinaryOpNode<DataType, DataType, DataType>;
        TestNode::BinaryOperator add = []( TestNode::Arg1_type a,
                                           TestNode::Arg2_type b ) -> TestNode::Res_type {
            return a + b;
        };
        auto op = new TestNode( "addition" );
        op->setOperator( add );
        g.addNode( op );
        g.addLink( source_a, "to", op, "a" );
        g.addLink( op, "r", sink, "from" );
        g.addLink( source_b, "to", op, "b" );

        // execution of the original graph
        DataType x { 1_ra };
        a->setData( &x );
        DataType y { 2_ra };
        b->setData( &y );
        g.execute();
        auto z = r->getData<DataType>();
        REQUIRE( z == x + y );

        // Save the graph
        std::string tmpdir { "tmpDir4Tests" };

        std::cout << "Graph tmp dir : " << tmpdir << "\n";
        std::filesystem::create_directories( tmpdir );
        g.saveToJson( tmpdir + "/GraphSerializationTest.json" );
        g.destroy();
        // this does nothing as g was destroyed
        g.execute();

        // Create a new graph and load from the saved graph
        DataflowGraph g1 { "loaded graph" };
        g1.loadFromJson( tmpdir + "/GraphSerializationTest.json" );

        // Setting the unserializable data on nodes (functions)
        auto addition = g1.getNode( "addition" );
        REQUIRE( addition != nullptr );
        REQUIRE( addition->getTypeName() == Functionals::BinaryOpScalar::getTypename() );
        auto typedAddition = dynamic_cast<Functionals::BinaryOpScalar*>( addition );
        REQUIRE( typedAddition != nullptr );
        typedAddition->setOperator( add );
        // execute loaded graph
        g1.execute();
        auto r_loaded  = g1.getDataGetter( "r_from" );
        auto& z_loaded = r_loaded->getData<DataType>();
        // Data loaded for source nodes are the one saved by the original graph
        REQUIRE( z_loaded == z );
        auto a_loaded = g1.getDataSetter( "a_to" );
        auto b_loaded = g1.getDataSetter( "b_to" );
        DataType xp { 2_ra };
        a_loaded->setData( &xp );
        DataType yp { 3_ra };
        b_loaded->setData( &yp );
        g1.execute();
        REQUIRE( z_loaded == 5 );
        std::filesystem::remove_all( tmpdir );
    }
}
