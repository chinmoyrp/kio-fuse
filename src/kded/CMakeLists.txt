set(KDED_KIOFUSE_SRCS kded.cpp)

qt5_add_dbus_adaptor(KDED_KIOFUSE_SRCS org.kde.KIOFuse.xml kded.h KIOFuse kdedadaptor KIOFuseAdaptor)

add_library(kiofuse MODULE ${KDED_KIOFUSE_SRCS})
kcoreaddons_desktop_to_json(kiofuse kiofuse.desktop)

target_link_libraries(kiofuse
    KF5::CoreAddons
    KF5::DBusAddons
    KF5::KIOCore
)

install(TARGETS kiofuse DESTINATION ${PLUGIN_INSTALL_DIR}/kf5/kded)
