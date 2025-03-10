add_definitions(-Wno-warn-absolute-paths)
add_definitions (-DBINOCLE_GL)
set(BINOCLE_GL true)
add_definitions (-DBINOCLE_GLES2)

# -std=gnu++0x
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu99")
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99")

set (CMAKE_EXECUTABLE_SUFFIX ".html")
# Add -s ALLOW_MEMORY_GROWTH=1 to avoid memory issues at the cost of some optimizations
if (DEBUG)
    message("Compiling for EMSCRIPTEN in Debug mode")
    #it might be worth disabling WASM for debugging (-s WASM=0) but I'm not even sure it can be of any help today
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -s ASSERTIONS=2 -s USE_SDL=3 -s USE_ZLIB=1 -s USE_LIBPNG=1 -s USE_VORBIS=1 -s USE_OGG=1 -s STB_IMAGE=1 -sFETCH -s INITIAL_MEMORY=1073741824 -s DISABLE_EXCEPTION_CATCHING=0 -s NO_EXIT_RUNTIME=1 -s USE_WEBGL2=1 -sMIN_WEBGL_VERSION=2 -sMAX_WEBGL_VERSION=2 -sFORCE_FILESYSTEM=1 -sASYNCIFY -sSTACK_SIZE=128000 --preload-file ${BINOCLE_DATA_DIR}@/assets/ --shell-file ${BINOCLE_DATA_DIR}index.html --use-preload-plugins -Wno-invalid-offsetof -std=gnu++0x")
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -s ASSERTIONS=2 -s USE_SDL=3 -s USE_ZLIB=1 -s USE_LIBPNG=1 -s USE_VORBIS=1 -s USE_OGG=1 -s STB_IMAGE=1 -sFETCH -s INITIAL_MEMORY=1073741824 -s DISABLE_EXCEPTION_CATCHING=0 -s NO_EXIT_RUNTIME=1 -s USE_WEBGL2=1 -sMIN_WEBGL_VERSION=2 -sMAX_WEBGL_VERSION=2 -sFORCE_FILESYSTEM=1 -sASYNCIFY -sSTACK_SIZE=128000 --preload-file ${BINOCLE_DATA_DIR}@/assets/ --shell-file ${BINOCLE_DATA_DIR}index.html --use-preload-plugins")
else()
    message("Compiling for EMSCRIPTEN in Release mode")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O -s USE_SDL=3 -s USE_ZLIB=1 -s USE_LIBPNG=1 -s USE_VORBIS=1 -s USE_OGG=1 -s STB_IMAGE=1 -sFETCH -s INITIAL_MEMORY=1073741824 -s DISABLE_EXCEPTION_CATCHING=0 -s NO_EXIT_RUNTIME=1 -s USE_WEBGL2=1 -sMIN_WEBGL_VERSION=2 -sMAX_WEBGL_VERSION=2 -sFORCE_FILESYSTEM=1 -sASYNCIFY -sSTACK_SIZE=128000 --preload-file ${BINOCLE_DATA_DIR}@/assets/ --shell-file ${BINOCLE_DATA_DIR}index.html --use-preload-plugins -Wno-invalid-offsetof -std=gnu++0x")
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O -s USE_SDL=3 -s USE_ZLIB=1 -s USE_LIBPNG=1 -s USE_VORBIS=1 -s USE_OGG=1 -s STB_IMAGE=1 -sFETCH -s INITIAL_MEMORY=1073741824 -s DISABLE_EXCEPTION_CATCHING=0 -s NO_EXIT_RUNTIME=1 -s USE_WEBGL2=1 -sMIN_WEBGL_VERSION=2 -sMAX_WEBGL_VERSION=2 -sFORCE_FILESYSTEM=1 -sASYNCIFY -sSTACK_SIZE=128000 --preload-file ${BINOCLE_DATA_DIR}@/assets/ --shell-file ${BINOCLE_DATA_DIR}index.html --use-preload-plugins")
    message("EMCC settings: ${CMAKE_C_FLAGS}")
endif ()


set (BINOCLE_LINK_LIBRARIES ${BINOCLE_LINK_LIBRARIES})
