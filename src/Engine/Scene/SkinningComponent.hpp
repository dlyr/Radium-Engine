#pragma once

#include <Core/Animation/HandleWeight.hpp>
#include <Core/Animation/Pose.hpp>
#include <Core/Animation/SkinningData.hpp>
#include <Core/Asset/HandleData.hpp>
#include <Core/Geometry/TriangleMesh.hpp>
#include <Core/Math/DualQuaternion.hpp>
#include <Core/Utils/Index.hpp>

#include <Engine/Data/Material.hpp>
#include <Engine/Scene/Component.hpp>
#include <Engine/Scene/ComponentMessenger.hpp>

namespace Ra {
namespace Engine {
namespace Scene {

/// The SkinningComponent class is responsible for applying Geometric Skinning Methods
/// on an animated object's mesh.
class RA_ENGINE_API SkinningComponent : public Component
{
  public:
    /// The Geometric Skinning Method.
    enum SkinningType {
        LBS = 0,  ///< Linear Blend Skinning
        DQS,      ///< Dual Quaternion Skinning
        COR,      ///< Center of Rotation skinning
        STBS_LBS, ///< Stretchable Twistable Bone Skinning with LBS
        STBS_DQS  ///< Stretchable Twistable Bone Skinning with DQS
    };

    SkinningComponent( const std::string& name, SkinningType type, Entity* entity ) :
        Component( name, entity ),
        m_skinningType( type ),
        m_isReady( false ),
        m_forceUpdate( false ),
        m_weightBone( 0 ),
        m_weightType( 0 ),
        m_showingWeights( false ) {}

    ~SkinningComponent() override {}

    virtual void initialize() override;

    /// Apply the Skinning Method.
    void skin();

    /// Update internal data and apply postprocesses to the mesh, e.g. normal computation.
    void endSkinning();

    /// Sets the Skinning method to use.
    void setSkinningType( SkinningType type );

    /// \returns the current skinning method.
    inline SkinningType getSkinningType() const { return m_skinningType; }

    /// Loads the skinning data from the given Handledata.
    /// \note Call initialize() afterwards to finalize data registration.
    void handleSkinDataLoading( const Ra::Core::Asset::HandleData* data,
                                const std::string& meshName,
                                const Ra::Core::Transform& meshFrame );

    /// @returns the reference skinning data.
    const Ra::Core::Skinning::RefData* getRefData() const { return &m_refData; }

    /// @returns the current Pose data.
    const Ra::Core::Skinning::FrameData* getFrameData() const { return &m_frameData; }

    /// @returns the name of the skinned mesh.
    const std::string getMeshName() const;

    /// @returns the name of the skeleton skinning the mesh.
    const std::string getSkeletonName() const;

    /// Toggles use of smart stretch.
    void setSmartStretch( bool on );

    /// @returns whether smart stretch is active or not.
    bool getSmartStretch() const { return m_smartStretch; }

    /// Toggles display of skinning weights.
    void showWeights( bool on );

    /// Set the type of skinning weight to display:
    ///  - 0 for standard skinning weights
    ///  - 1 for stbs weights
    void showWeightsType( int type );

    /// Set the bone to show the weights of.
    void setWeightBone( uint bone );

  public:
    /// The Entity name for Component communication.
    std::string m_skelName;

  private:
    /// Registers the Entity name for Component communication (out).
    void setupIO( const std::string& id );

    /// Computes internal data related to the Skinning method.
    void setupSkinningType( SkinningType type );

    /// Internal function to create the skinning weights.
    void createWeightMatrix();

    /// Skinning Weight Matrix getter for CC.
    const Ra::Core::Animation::WeightMatrix* getWeightsOutput() const;

    /// Applies smart stretch to the current pose.
    void applySmartStretch();

    /// Applies the bones bindMatrices to the given skeleton pose.
    void applyBindMatrices( Ra::Core::Animation::Pose& pose ) const;

  private:
    template<typename T>
    using Getter = typename ComponentMessenger::CallbackTypes<T>::Getter;

    template<typename T>
    using ReadWrite = typename ComponentMessenger::CallbackTypes<T>::ReadWrite;

    /// The mesh name for Component communication.
    std::string m_meshName;

    /// Inverse of the initial mesh transform.
    Ra::Core::Transform m_meshFrameInv;

    /// The refrence Skinning data.
    Ra::Core::Skinning::RefData m_refData;

    /// The current Pose data.
    Ra::Core::Skinning::FrameData m_frameData;

    /// Getter for the animation skeletton.
    Getter<Ra::Core::Animation::Skeleton> m_skeletonGetter;

    /// Read FMC's RO idx.
    Getter<Ra::Core::Utils::Index> m_renderObjectReader;

    /// Getter/Setter to the mesh
    bool m_meshIsPoly {false};
    ReadWrite<Ra::Core::Geometry::TriangleMesh> m_triMeshWriter;
    ReadWrite<Ra::Core::Geometry::PolyMesh> m_polyMeshWriter;

    /// The Skinning Method.
    SkinningType m_skinningType;

    /// Are all the required data available.
    bool m_isReady;

    /// Whether skinning is mandatory for the current frame.
    bool m_forceUpdate;

    /// The duplicate vertices map, used to recompute smooth normals.
    // FIXME: implement proper normal skinning such as http://vcg.isti.cnr.it/deformFactors/
    std::vector<Ra::Core::Utils::Index> m_duplicatesMap;

    /// The skinning weights, stored per bone.
    /// \note These are stored this way because we cannot build the weight matrix
    ///       without data from other components (skeleton).
    std::map<std::string, std::vector<std::pair<uint, Scalar>>> m_loadedWeights;
    /// The bind matrices, stored per bone.
    /// \note These are stored this way because we cannot fill m_refData
    ///       without data from other components (skeleton).
    std::map<std::string, Ra::Core::Transform> m_loadedBindMatrices;

    /// The STBS weights.
    Ra::Core::Animation::WeightMatrix m_weightSTBS;

    /// stretch mode: false = standard, true = smart.
    bool m_smartStretch { true };

    /// Initial RO Material when not showing skinning weights.
    std::shared_ptr<Data::Material> m_baseMaterial;
    /// Material to be used for the skinning weights
    std::shared_ptr<Data::Material> m_weightMaterial;
    Ra::Core::Vector3Array m_baseUV;
    Ra::Core::Vector3Array m_weightsUV;
    uint m_weightBone;
    uint m_weightType;
    bool m_showingWeights;
};

} // namespace Scene
} // namespace Engine
} // namespace Ra

