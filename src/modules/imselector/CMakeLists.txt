add_fcitx5_addon(imselector imselector.cpp)
target_link_libraries(imselector Fcitx5::Core)
install(TARGETS imselector DESTINATION "${FCITX_INSTALL_ADDONDIR}")
configure_file(imselector.conf.in.in imselector.conf.in @ONLY)
fcitx5_translate_desktop_file(${CMAKE_CURRENT_BINARY_DIR}/imselector.conf.in imselector.conf)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/imselector.conf" DESTINATION "${FCITX_INSTALL_PKGDATADIR}/addon"
        COMPONENT config)

