#include <Core/Types.hpp>
#include <Core/Utils/Attribs.hpp>
#include <Core/Utils/Log.hpp>
#include <Eigen/src/Core/AssignEvaluator.h>
#include <Eigen/src/Core/GenericPacketMath.h>
#include <Eigen/src/Core/Matrix.h>
#include <algorithm>
#include <ostream>

#include "Core/CoreMacros.hpp"
#include "Core/Utils/Attribs.inl"
#include "Core/Utils/Index.hpp"

namespace Ra {
namespace Core {
namespace Utils {

AttribBase::~AttribBase() {
    detachAll();
}

AttribManager::~AttribManager() {
    clear();
}

void AttribManager::clear() {
    m_attribs.clear();
    m_attribsIndex.clear();
    m_numAttribs = 0;
}

void AttribManager::copyAllAttributes( const AttribManager& m ) {
    m_attribs.clear();
    m_numAttribs = 0;
    for ( const auto& attr : m.m_attribs ) {
        m_attribsIndex[attr->getName()] = m_attribs.size();
        m_attribs.push_back( attr->clone() );
        ++m_numAttribs;
    }
}

bool AttribManager::hasSameAttribs( const AttribManager& other ) {
    // one way
    for ( const auto& attr : m_attribsIndex ) {
        if ( other.m_attribsIndex.find( attr.first ) == other.m_attribsIndex.cend() ) {
            return false;
        }
    }
    // the other way
    for ( const auto& attr : other.m_attribsIndex ) {
        if ( m_attribsIndex.find( attr.first ) == m_attribsIndex.cend() ) { return false; }
    }
    return true;
}

} // namespace Utils
} // namespace Core
} // namespace Ra
