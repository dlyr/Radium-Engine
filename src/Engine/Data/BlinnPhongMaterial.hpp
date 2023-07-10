#pragma once

#include <Engine/RaEngine.hpp>

#include <Core/Utils/Color.hpp>
#include <Engine/Data/Material.hpp>
#include <Engine/Data/Texture.hpp>
#include <Engine/Data/TextureManager.hpp>
#include <Engine/RadiumEngine.hpp>

#include <map>
#include <string>

namespace Ra {
namespace Core {
namespace Asset {
class MaterialData;
}
} // namespace Core

namespace Engine {

namespace Data {
class ShaderProgram;

namespace TextureSemantics {
namespace BlinnPhongMaterial {
enum class TextureSemantic { TEX_DIFFUSE, TEX_SPECULAR, TEX_NORMAL, TEX_SHININESS, TEX_ALPHA };
}
} // namespace TextureSemantics

/**
 * Implementation of the Blinn-Phong Material BSDF.
 * @todo due to "Material.glsl" interface modification, must test this version with all plugins,
 * apps, ... that uses Radium Renderer
 */

class RA_ENGINE_API BlinnPhongMaterial final
    : public Material,
      public ParameterSetEditingInterface,
      public MaterialTextureSet<TextureSemantics::BlinnPhongMaterial::TextureSemantic>
{
    friend class BlinnPhongMaterialConverter;

  public:
    using TextureSemantic = TextureSemantics::BlinnPhongMaterial::TextureSemantic;

  public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    /**
     * Construct a named Blinn-Phongmaterial
     * @param instanceName The name of this instance of the material
     */
    explicit BlinnPhongMaterial( const std::string& instanceName );

    /**
     * Destructor.
     * @note The material does not have ownership on its texture. This destructor do not delete the
     * associated textures.
     */
    ~BlinnPhongMaterial() override;
    using MaterialTextureSet<TextureSemantics::BlinnPhongMaterial::TextureSemantic>::addTexture;

    /// \todo Fix this specialisation. Maybe assume that named texture have to be added to manager
    /// before hand, and that texture on the fly addition is a fix, hence no special care for normal
    /// maps.
    void addTexture( const TextureSemantic& semantic, const std::string& texture ) {
        CORE_ASSERT( !texture.empty(), "Invalid texture name" );
        auto texManager = RadiumEngine::getInstance()->getTextureManager();
        auto texHandle  = texManager->getTextureHandle( texture );
        if ( texHandle.isValid() ) {
            MaterialTextureSet<TextureSemantics::BlinnPhongMaterial::TextureSemantic>::addTexture(
                semantic, texHandle );
        }
        else {
            TextureParameters data;
            data.name          = texture;
            data.sampler.wrapS = GL_REPEAT;
            data.sampler.wrapT = GL_REPEAT;
            if ( semantic != TextureSemantic::TEX_NORMAL )
                data.sampler.minFilter = GL_LINEAR_MIPMAP_LINEAR;
            MaterialTextureSet<TextureSemantics::BlinnPhongMaterial::TextureSemantic>::addTexture(
                semantic, data );
        }
    }

    void updateGL() override;
    void updateFromParameters() override;
    bool isTransparent() const override;

    /**
     * Register the material in the material library.
     * After registration, the material could be instantiated by any Radium system, renderer,
     * plugin, ...
     */
    static void registerMaterial();

    /**
     * Remove the material from the material library.
     * After removal, the material is no more available, ...
     */
    static void unregisterMaterial();

    /**
     * Get a json containing metadata about the parameters of the material.
     * @return the metadata in json format
     */
    inline nlohmann::json getParametersMetadata() const override;

    inline void setColoredByVertexAttrib( bool state ) override;

    inline bool isColoredByVertexAttrib() const override;

  public:
    Core::Utils::Color m_kd { 0.7, 0.7, 0.7, 1.0 };
    Core::Utils::Color m_ks { 0.3, 0.3, 0.3, 1.0 };
    Scalar m_ns { 64.0 };
    Scalar m_alpha { 1.0 };
    bool m_perVertexColor { false };
    bool m_renderAsSplat { false };

  private:
    std::map<TextureSemantic, TextureManager::TextureHandle> m_textures;
    static nlohmann::json s_parametersMetadata;

    /**
     * Update the rendering parameters for the Material
     */
    void updateRenderingParameters();
};

/**
 * Converter from an external representation comming from FileData to internal representation.
 */
class RA_ENGINE_API BlinnPhongMaterialConverter final
{
  public:
    BlinnPhongMaterialConverter()  = default;
    ~BlinnPhongMaterialConverter() = default;

    Material* operator()( const Ra::Core::Asset::MaterialData* toconvert );
};

inline nlohmann::json BlinnPhongMaterial::getParametersMetadata() const {
    return s_parametersMetadata;
}

inline void BlinnPhongMaterial::setColoredByVertexAttrib( bool state ) {
    bool oldState    = m_perVertexColor;
    m_perVertexColor = state;
    if ( oldState != m_perVertexColor ) { needUpdate(); }
}

inline bool BlinnPhongMaterial::isColoredByVertexAttrib() const {
    return m_perVertexColor;
}

} // namespace Data
} // namespace Engine
} // namespace Ra
