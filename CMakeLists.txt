PROJECT(fawesome)

ADD_EXECUTABLE(fawesome

	src/main.cpp

	src/file.cpp

	src/tclbind.cpp

	src/configfile.cpp

	src/framelimiter.cpp

	src/screen.cpp

	src/audioplayer.cpp

	src/filenotfoundexception.cpp

	src/surfacecache.cpp

        src/gfx/gfxobject.cpp
	src/gfx/sprite.cpp
	src/gfx/controllablesprite.cpp
	src/gfx/draggablesprite.cpp
	src/gfx/mousecursor.cpp
	src/gfx/player.cpp
        src/gfx/layer.cpp
	src/gfx/map.cpp
	src/gfx/splash.cpp

	src/eventhandler.cpp
	src/eventqueue.cpp

        src/events/event.cpp

	src/events/controllerevent.cpp
	src/events/controllerpressevent.cpp
	src/events/controllerreleaseevent.cpp

	src/events/pointerevent.cpp
	src/events/pointerpressevent.cpp
	src/events/pointerreleaseevent.cpp
	src/events/pointermoveevent.cpp

	src/events/activateevent.cpp
)

TARGET_LINK_LIBRARIES(fawesome tcl8.5 z SDL SDL_image SDL_ttf SDL_gfx)

SET(CMAKE_CXX_FLAGS_DEBUG "-g -W -Wall -Wextra -Wnon-virtual-dtor -pedantic")
SET(CMAKE_CXX_FLAGS_RELEASE "-O3 -funroll-loops -finline-functions -ffast-math -DNDEBUG")

IF(MINGW)
	TARGET_LINK_LIBRARIES(fawesome zlib tcl8.5 fawesound mingw32 SDLmain SDL SDL_image SDL_ttf SDL_gfx SDL_mixer)
	SET(CMAKE_EXE_LINKER_FLAGS "resource.obj")

	ADD_CUSTOM_COMMAND(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/resource.obj
		COMMAND windres.exe
			-I${CMAKE_CURRENT_SOURCE_DIR}
			-o ${CMAKE_CURRENT_BINARY_DIR}/resource.obj
			< ${CMAKE_CURRENT_SOURCE_DIR}/resource.rc
		)

ENDIF(MINGW)
