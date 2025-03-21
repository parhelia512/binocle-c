//
// Binocle
// Copyright (c) 2015-2019 Valerio Santinelli
// All rights reserved.
//

#ifndef BINOCLE_SDL_H
#define BINOCLE_SDL_H

#include <stdbool.h>
#include <sys/stat.h>

#if defined(__IPHONEOS__) || defined(__ANDROID__) || defined(__EMSCRIPTEN__)

#include <SDL3/SDL_opengles.h>
#include <SDL3/SDL_opengles2.h>
#include <SDL3/SDL_system.h>

#elif defined(__APPLE__)

#if defined(BINOCLE_GL)
//#define GL_GLEXT_PROTOTYPES 1
//#include <SDL3/SDL_opengl.h>
#include <OpenGL/gl.h>
#elif defined(BINOCLE_METAL)
#include <OpenGL/gl.h>
#endif
#endif

#if defined(__WINDOWS__)
#include <glew/include/GL/glew.h>
#include <SDL3/SDL_opengl.h>
#include <SDL3/SDL_system.h>
#endif

// SDL
#include <SDL3/SDL.h>

/**
 * \brief Starts SDL2 and must be called before everything else
 * @return true if everything went ok
 */
bool binocle_sdl_init();

/**
 * \brief Shuts down SDL2 and must be called when the program exits
 */
void binocle_sdl_exit();

/**
 * \brief Gets the last modification time of a given file
 * @param filename The filename
 * @return The last modification time of the file
 */
time_t binocle_sdl_get_last_modification_time(char *filename);

/**
 * \brief Loads a text file
 * Loads a text file and stores it in buffer. This function allocates memory so it's up to the consumer to free
 * it when done with the content.
 * @param filename The filename
 * @param buffer The buffer to write the output to
 * @param buffer_length The size of the buffer
 * @return true if everything went ok
 */
bool binocle_sdl_load_text_file(char *filename, char **buffer, size_t *buffer_length);

/**
 * Writes a text to a file
 * @param filename The filename
 * @param buffer The buffer with the text
 * @param size The size of the buffer
 * @return true if everything went ok
 */
bool binocle_sdl_write_text_file(char *filename, char *buffer, size_t size);

/**
 * \brief Loads a binary file
 * Loads a binary file and stores it in buffer. This function allocates memory so it's up to the consumer to free
 * it when done with the content.
 * @param filename The filename
 * @param buffer The buffer to write the output to
 * @param buffer_length The size of the buffer
 * @return true if everything went ok
 */
bool binocle_sdl_load_binary_file(char *filename, char **buffer, size_t *buffer_length);

/**
 * \brief Returns the path of the assets folder
 * The actual path depends on the current platform.
 * This function allocates the memory for the string and it's up to the caller to free it after use.
 * @return the pointer to the string with the folder of the assets
 */
const char *binocle_sdl_assets_dir();

/**
 * \brief Returns whether a filename ends with a certain suffix
 * @param str the filename
 * @param suffix the suffix
 * @return true if the filename ends with the provided suffix
 */
bool binocle_sdl_filename_ends_with(const char *str, const char *suffix);

/**
 * \brief Returns the path where files can be written.
 * This is specific per platform.
 * @param org the name of the organization
 * @param app the name of the application
 * @return the path where files can be written. An empty string if the path can't be found.
 */
char *binocle_sdl_preferences_dir(const char *org, const char *app);

char *binocle_sdl_str_replace(char *orig, char *rep, char *with);
bool binocle_sdl_file_exists(const char *filename);
bool binocle_sdl_directory_exists(const char *path);

#endif //BINOCLE_SDL_H
