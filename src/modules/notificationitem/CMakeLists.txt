if (ENABLE_DBUS)
    add_fcitx5_addon(notificationitem notificationitem.cpp dbusmenu.cpp)
    target_link_libraries(notificationitem Fcitx5::Core Fcitx5::Module::DBus Fcitx5::Module::ClassicUI )
    install(TARGETS notificationitem DESTINATION "${FCITX_INSTALL_ADDONDIR}")
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/notificationitem.conf" DESTINATION "${FCITX_INSTALL_PKGDATADIR}/addon"
            COMPONENT config)
endif()

configure_file(notificationitem.conf.in.in notificationitem.conf.in @ONLY)
fcitx5_translate_desktop_file(${CMAKE_CURRENT_BINARY_DIR}/notificationitem.conf.in notificationitem.conf)
fcitx5_export_module(NotificationItem TARGET notificationitem BUILD_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}" HEADERS notificationitem_public.h INSTALL)
