SET ( FRX_MOBILE_SERVER
	server/MobileServer.cpp
	server/main.cpp
)

add_executable(VSTForx.mobile-server ${FRX_MOBILE_SERVER})
target_link_libraries (VSTForx.mobile-server frx_core ${FRX_CLIBS})



