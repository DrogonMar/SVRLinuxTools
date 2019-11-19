
set(TP_HEADERS ThirdParty/OpenVRQT/openvroverlaycontroller.h)
set(TP_SOURCES ThirdParty/OpenVRQT/openvroverlaycontroller.cpp)

include_directories(ThirdParty/OpenVR/headers)
link_directories(ThirdParty/OpenVR/lib/linux64)

find_package(PkgConfig REQUIRED)
pkg_check_modules(PULSE REQUIRED libpulse)