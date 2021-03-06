cmake_minimum_required(VERSION 3.1)

project(kio-fuse VERSION 0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(Qt5_MIN_VERSION 5.9)
set(KF5_MIN_VERSION 5.56)

find_package(ECM ${KF5_MIN_VERSION} CONFIG REQUIRED)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

include(ECMSetupVersion)
include(ECMGenerateHeaders)
include(CMakePackageConfigHelpers)
include(FeatureSummary)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDEFrameworkCompilerSettings NO_POLICY_SCOPE)
include(ECMQtDeclareLoggingCategory)

find_package(PkgConfig REQUIRED)
find_package(Qt5 ${Qt5_MIN_VERSION} COMPONENTS Core REQUIRED)
find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS 
    DBusAddons
    KIO
)
pkg_check_modules(FUSE3 REQUIRED fuse3)

add_subdirectory(src)

if(BUILD_TESTING)
	add_subdirectory(tests)
endif()

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
