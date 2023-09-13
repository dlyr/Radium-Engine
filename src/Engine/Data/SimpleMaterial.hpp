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
namespace Engine {
namespace Data {

/** \brief Namespace to define materials' texture semantics.
 *
 * Convention: add you're material's MyMaterial Enum inside TextureSemantics namespace.
 */
namespace TextureSemantics {
/// Semantic of the texture : define which BSDF parameter is controled by the texture
enum class SimpleMaterial { TEX_COLOR, TEX_MASK };
} // namespace TextureSemantics

/**
 * Base implementation for simple, monocolored, materials.
 * This material could not be used as is. Only derived class could be used by a renderer.
 */
class RA_ENGINE_API SimpleMaterial : public Material,
                                     public ParameterSetEditingInterface,
                                     public MaterialTextureSet<TextureSemantics::SimpleMaterial>
{
  public:
    using TextureSemantic = TextureSemantics::SimpleMaterial;
    /**
     * Construct a named  material
     * \param name The name of the material
     */
    explicit SimpleMaterial( const std::string& instanceName,
                             const std::string& materialName,
                             MaterialAspect aspect = MaterialAspect::MAT_OPAQUE );

    /**
     * Update the openGL state of the material.
     * This state only consists on associated textures.
     */
    void updateGL() override final;

    inline void setColoredByVertexAttrib( bool state ) override;

    inline bool isColoredByVertexAttrib() const override;

    inline void setColor( Core::Utils::Color c ) {
        m_color = std::move( c );
        needUpdate();
    }

  protected:
    /// Load the material parameter description
    static void loadMetaData( nlohmann::json& destination );

  private:
    /**
     * Update the rendering parameters for the Material
     */
    void updateRenderingParameters();
    /// The base color of the material
    Core::Utils::Color m_color { 0.9, 0.9, 0.9, 1.0 };
    /// Indicates if the material will takes its base color from vertices' attributes.
    bool m_perVertexColor { false };
};

inline void SimpleMaterial::setColoredByVertexAttrib( bool state ) {
    if ( state != m_perVertexColor ) {
        m_perVertexColor = state;
        needUpdate();
    }
}

inline bool SimpleMaterial::isColoredByVertexAttrib() const {
    return m_perVertexColor;
}

} // namespace Data
} // namespace Engine
} // namespace Ra
