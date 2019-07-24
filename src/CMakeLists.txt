set(KIOFUSE_SOURCES
	main.cpp
	kiofusevfs.cpp
	kiofusevfs.h
	kiofusenode.h)

ecm_qt_declare_logging_category(KIOFUSE_SOURCES
   HEADER debug.h
   IDENTIFIER KIOFUSE_LOG
   CATEGORY_NAME org.kde.kio.fuse
   DEFAULT_SEVERITY Warning)
   
add_executable(kio-fuse ${KIOFUSE_SOURCES})
target_include_directories(kio-fuse PRIVATE ${FUSE3_INCLUDE_DIRS})
target_compile_definitions(kio-fuse PRIVATE FUSE_USE_VERSION=31 ${FUSE3_CFLAGS_OTHER})
target_link_libraries(kio-fuse PRIVATE Qt5::Core KF5::KIOCore ${FUSE3_LIBRARIES})
install(TARGETS kio-fuse DESTINATION ${KF5_INSTALL_TARGETS_DEFAULT_ARGS})
add_subdirectory(kded)
