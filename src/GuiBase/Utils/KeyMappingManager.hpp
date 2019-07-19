#ifndef RADIUMENGINE_KEYMAPPINGMANAGER_HPP
#define RADIUMENGINE_KEYMAPPINGMANAGER_HPP

#include <QtGui/QKeyEvent>
#include <QtGui/QMouseEvent>
#include <QtXml/QtXml>

#include <GuiBase/RaGuiBase.hpp>

#include <Core/Utils/Index.hpp>
#include <Core/Utils/Log.hpp>
#include <Core/Utils/Singleton.hpp>

namespace Ra {
namespace Gui {
/// An utility class used to map a (combination) of key / modifier to a specific action.
/// It can load configuration from a file or if no config is found it will load an
/// internal version of the default configuration.
class RA_GUIBASE_API KeyMappingManager
{
    RA_SINGLETON_INTERFACE( KeyMappingManager );

    // Needed by Q_ENUM
    Q_GADGET

  public:
    using Listener         = void ( * )();
    using KeyMappingAction = Ra::Core::Utils::Index;
    using Context          = Ra::Core::Utils::Index;

    /// load configuration from filename, or default configration filename. It
    /// calls the listener callback then.
    void loadConfiguration( const char* filename = nullptr );

    void reloadConfiguration();

    /// Return the action associated to the binding buttons + modifiers + key
    /// \param buttons are the mouse buttons pressed, could be NoButton
    /// \param modifiers are the keyboard modifiers, could be NoModifiers
    /// \param key is the key pressed, could be -1
    KeyMappingManager::KeyMappingAction getAction( const KeyMappingManager::Context& context,
                                                   const Qt::MouseButtons& buttons,
                                                   const Qt::KeyboardModifiers& modifiers,
                                                   int key,
                                                   bool wheel = false );

    /// Return the context index corresponding to contextName
    /// \param contextName the name of the context
    /// \return an invalid context if contextName has not been created (i.e. context,isInvalid())
    Context getContext( const std::string& contextName );

    /// Return the action index corresponding to a context index and actionName
    /// \param context the index of the context
    /// \param actionName the name of the action
    /// \return an invalid action if context is not valid, or if actionName has  not been created.
    /// (i,e action.isInvalid())
    KeyMappingAction getActionIndex( const Context& context, const std::string& actionName );

    /// \return Action name if context index and action index are valid, "Invalid" otherwise
    std::string getActionName( const Context& context, const KeyMappingAction& action );

    /// \return Context name if context index is valid, "Invalid" otherwise
    std::string getContextName( const Context& context );

    /// Add a callback, triggered when configuration is load or reloaded.
    void addListener( Listener callback );

    /// return a string of enum names from mouse buttons, comma separated,
    /// without space
    static std::string enumNamesFromMouseButtons( const Qt::MouseButtons& buttons );

    /// return a string of enum names from keyboard modifiers, comma separated,
    /// without space
    static std::string enumNamesFromKeyboardModifiers( const Qt::KeyboardModifiers& modifiers );

  private:
    KeyMappingManager();
    ~KeyMappingManager();

    // Private for now, but may need to be public if we want to customize keymapping configuration
    // otherwise than by editing the XML configuration file.
    class MouseBinding
    {
      public:
        explicit MouseBinding( Qt::MouseButtons buttons,
                               Qt::KeyboardModifiers modifiers = Qt::NoModifier,
                               int key                         = -1,
                               bool wheel                      = false ) :

            m_buttons{buttons},
            m_modifiers{modifiers},
            m_key{key},
            m_wheel{wheel} {}
        bool operator<( const MouseBinding& b ) const {
            return ( m_buttons < b.m_buttons ) ||
                   ( ( m_buttons == b.m_buttons ) && ( m_modifiers < b.m_modifiers ) ) ||
                   ( ( m_buttons == b.m_buttons ) && ( m_modifiers == b.m_modifiers ) &&
                     ( m_key < b.m_key ) ) ||
                   ( ( m_buttons == b.m_buttons ) && ( m_modifiers == b.m_modifiers ) &&
                     ( m_key == b.m_key ) && ( m_wheel < b.m_wheel ) );
        }

        //  private:
        Qt::MouseButtons m_buttons;
        Qt::KeyboardModifiers m_modifiers;
        // only one key
        int m_key;
        bool m_wheel;
    };

    /// bind binding to actionIndex, in contextIndex. If replace previously
    /// binded action, with a warning if binding was alreasly present.,
    void bindKeyToAction( Ra::Core::Utils::Index contextIndex,
                          const MouseBinding& binding,
                          Ra::Core::Utils::Index actionIndex );

    void loadConfigurationInternal();
    void loadConfigurationTagsInternal( QDomElement& node );
    void loadConfigurationMappingInternal( const std::string& context,
                                           const std::string& keyString,
                                           const std::string& modifiersString,
                                           const std::string& buttonsString,
                                           const std::string& wheelString,
                                           const std::string& actionString );

    /// Return KeyboardModifiers described in modifierString, multiple modifiers
    /// are comma separated in the modifiers string, as in
    /// "ShiftModifier,AltModifier". This function do note trim any white space.
    static Qt::KeyboardModifiers getQtModifiersValue( const std::string& modifierString );

    /// Return MouseButtons desribed in buttonString, multiple modifiers
    /// are comma separated in the modifiers string, \note only one button is
    /// supported for the moment.
    static Qt::MouseButtons getQtMouseButtonsValue( const std::string& buttonsString );

  private:
    std::string defaultConfigFile;
    // For XML parsing using Qt
    QDomDocument m_domDocument;
    QMetaEnum m_metaEnumAction;
    QMetaEnum m_metaEnumKey;
    QFile* m_file;

    using MouseBindingMapping = std::map<MouseBinding, KeyMappingAction>;
    using ContextNameMap      = std::map<std::string, Context>;
    using ActionNameMap       = std::map<std::string, Ra::Core::Utils::Index>;

    std::vector<Listener> m_listeners;

    ContextNameMap m_contextNameToIndex;              ///< context string give index
    std::vector<ActionNameMap> m_actionNameToIndex;   ///< one element per context
    std::vector<MouseBindingMapping> m_mappingAction; ///< one element per context
};

/// KeyMappingManageable decorate, typical use as a CRTP :
/// class MyClass : public KeyMappingManageable<MyClass> { [...]
/// it defines a static class member m_keyMappingContext, readable with
/// getContext()
/// This context index must be registered by the class to the KeyMappingManager
/// with KeyMappingManager::getContext("MyClassContextName"); after a
/// configuration file is loaded.
template <typename T>
class KeyMappingManageable
{
  public:
    //    static virtual void registerKeyMapping() = 0;

    /*
        /// @return true if the event has been taken into account, false otherwise
        virtual bool handleMouseReleaseEvent( QMouseEvent* event ) { return false; }

        /// @return true if the event has been taken into account, false otherwise
        virtual bool handleMouseMoveEvent( QMouseEvent* event,
                                           const Qt::MouseButtons& buttons,
                                           const Qt::KeyboardModifiers& modifiers,
                                           int key ) {
            return false;
        }

        /// @return true if the event has been taken into account, false otherwise
        virtual bool handleWheelEvent( QWheelEvent* event ) { return false; }

        /// @return true if the event has been taken into account, false otherwise
        virtual bool handleKeyPressEvent( QKeyEvent* event,
                                          const KeyMappingManager::KeyMappingAction& action ) {
            return false;
        }

        /// @return true if the event has been taken into account, false otherwise
        virtual bool handleKeyReleaseEvent( QKeyEvent* event ) { return false; }
    */
    static KeyMappingManager::Context getContext() { return m_keyMappingContext; }

  protected:
    static KeyMappingManager::Context m_keyMappingContext;
};

/// create one m_keyMappingContext by template type.
template <typename T>
KeyMappingManager::Context KeyMappingManageable<T>::m_keyMappingContext;

}; // namespace Gui
} // namespace Ra

#endif // RADIUMENGINE_KEYMAPPINGMANAGER_HPP
