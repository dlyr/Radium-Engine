#pragma once

#include <Engine/Data/TextureManager.hpp>
#include <Engine/RadiumEngine.hpp>

namespace Ra {
namespace Engine {
namespace Data {

/** @brief Base class to manage a set of textures indexed by semantic (enum).
 */
template <typename TextureSemantic>
class MaterialTextureSet
{
  public:
    virtual ~MaterialTextureSet() = default;

    void addTexture( const TextureSemantic& semantic,
                     const TextureManager::TextureHandle& texture ) {
        m_textures[semantic] = texture;
    }

    void addTexture( const TextureSemantic& semantic, const TextureParameters& texture ) {
        auto texManager      = RadiumEngine::getInstance()->getTextureManager();
        m_textures[semantic] = texManager->addTexture( texture );
    }

    Texture* getTexture( const TextureSemantic& semantic ) const {
        Texture* tex    = nullptr;
        auto texManager = RadiumEngine::getInstance()->getTextureManager();
        auto it         = m_textures.find( semantic );
        if ( it != m_textures.end() ) { tex = texManager->getTexture( it->second ); }
        return tex;
    }

    void addTexture( const TextureSemantic& semantic, const std::string& texture ) {
        CORE_ASSERT( !texture.empty(), "Invalid texture name" );
        auto texManager = RadiumEngine::getInstance()->getTextureManager();
        auto texHandle  = texManager->getTextureHandle( texture );
        if ( texHandle.isValid() ) { addTexture( semantic, texHandle ); }
        else {
            TextureParameters data;
            data.name              = texture;
            data.sampler.wrapS     = GL_REPEAT;
            data.sampler.wrapT     = GL_REPEAT;
            data.sampler.minFilter = GL_LINEAR_MIPMAP_LINEAR;
            addTexture( semantic, data );
        }
    }

  private:
    std::map<TextureSemantic, TextureManager::TextureHandle> m_textures;
};
} // namespace Data
} // namespace Engine
} // namespace Ra
