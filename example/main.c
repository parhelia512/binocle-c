//
//  Binocle
//  Copyright(C)2015-2025 Valerio Santinelli
//

#include <stdio.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include "binocle_sdl.h"
#include "backend/binocle_color.h"
#include "binocle_window.h"
#include "binocle_game.h"
#include "binocle_viewport_adapter.h"
#include "binocle_camera.h"
#include <binocle_input.h>
#include <binocle_image.h>
#include <binocle_sprite.h>
#include <backend/binocle_material.h>
#include <binocle_lua.h>
#include <binocle_app.h>
#include "binocle_gd.h"
#include "binocle_log.h"
#include "binocle_bitmapfont.h"
#include "binocle_audio.h"
#include "sokol_gfx.h"
#include "binocle_http.h"
#include "binocle_memory.h"

#if defined(__IPHONEOS__) || defined(__ANDROID__)
#include <SDL3/SDL_main.h>
#endif

#define BINOCLE_MATH_IMPL
#include "binocle_math.h"

// #define GAMELOOP 1
//#define DEMOLOOP
#define TWODLOOP

#include "constants.h"
#include "my_game.h"

#if defined(BINOCLE_MACOS) && defined(BINOCLE_METAL)
#define SHADER_PATH "dst/metal-macos"
#define DEFAULT_VS_FILENAME "default.glsl_default_metal_macos_vs.metal"
#define DEFAULT_FS_FILENAME "default.glsl_default_metal_macos_fs.metal"
#define SCREEN_VS_FILENAME "screen.glsl_default_metal_macos_vs.metal"
#define SCREEN_FS_FILENAME "screen.glsl_default_metal_macos_fs.metal"
#define FLAT_VS_FILENAME "flat_flat_metal_macos_vs.metal"
#define FLAT_FS_FILENAME "flat_flat_metal_macos_fs.metal"
#elif defined(__IPHONEOS__) || defined(__ANDROID__) || defined(__EMSCRIPTEN__)
#define SHADER_PATH "dst/gles"
#define DEFAULT_VS_FILENAME "default.glsl_default_glsl300es_vs.glsl"
#define DEFAULT_FS_FILENAME "default.glsl_default_glsl300es_fs.glsl"
#define SCREEN_VS_FILENAME "screen.glsl_default_glsl300es_vs.glsl"
#define SCREEN_FS_FILENAME "screen.glsl_default_glsl300es_fs.glsl"
#define FLAT_VS_FILENAME "flat_flat_glsl300es_vs.glsl"
#define FLAT_FS_FILENAME "flat_flat_glsl300es_fs.glsl"
#else
#define SHADER_PATH "dst/gl33"
#define DEFAULT_VS_FILENAME "default.glsl_default_glsl410_vs.glsl"
#define DEFAULT_FS_FILENAME "default.glsl_default_glsl410_fs.glsl"
#define SCREEN_VS_FILENAME "screen.glsl_default_glsl410_vs.glsl"
#define SCREEN_FS_FILENAME "screen.glsl_default_glsl410_fs.glsl"
#define FLAT_VS_FILENAME "flat_flat_glsl410_vs.glsl"
#define FLAT_FS_FILENAME "flat_flat_glsl410_fs.glsl"
#endif

typedef struct default_shader_params_t {
  float projectionMatrix[16];
  float modelMatrix[16];
  float viewMatrix[16];
} default_shader_params_t;

typedef struct screen_shader_fs_params_t {
  float resolution[2];
  float scale[2];
  float viewport[2];
  uint8_t _pad_24[8];
} screen_shader_fs_params_t;

typedef struct screen_shader_vs_params_t {
  float transform[16];
} screen_shader_vs_params_t;

binocle_app app;
game_state_t *game_state;

#if (defined(__APPLE__) && !defined(__IPHONEOS__)) || defined(__EMSCRIPTEN__)
#define WITH_PHYSICS
#endif

#ifdef WITH_PHYSICS
#include "chipmunk/chipmunk.h"
#include "physics.h"
physics_state_t ps;
#endif

#if defined(BINOCLE_HTTP)
void test_http_get() {
  binocle_http_body_t body;
  if (binocle_http_get("https://podium.altralogica.it/l/binocle-example/top/0",
                       &body)) {
    binocle_log_info(body.memory);
  }
  free(body.memory);
}

void test_http_post() {
  binocle_http_body_t body;
  char *req = "{score=1}";
  if (binocle_http_post(
        "https://podium.altralogica.it/l/binocle-example/members/tanis/score",
        req, &body)) {
    binocle_log_info(body.memory);
  }
  free(body.memory);
}

void test_http_put() {
  binocle_http_body_t body;
  char *req = "{\"score\":1}";
  if (binocle_http_put(
        "https://podium.altralogica.it/l/binocle-example/members/tanis/score",
        req, &body)) {
    binocle_log_info(body.memory);
  }
  free(body.memory);
}
#endif

#ifdef TWODLOOP
void main_loop() {
  binocle_window_begin_frame(game_state->engine.window);
  float dt = binocle_window_get_frame_time(game_state->engine.window) / 1000.0f;

  binocle_input_update(&game_state->engine.input);
  binocle_audio_update_music_stream(&game_state->engine.audio,
                                    &game_state->engine.music);

  if (game_state->engine.input.resized) {
    kmVec2 oldWindowSize;
    oldWindowSize.x = game_state->engine.window->width;
    oldWindowSize.y = game_state->engine.window->height;
    game_state->engine.window->width = game_state->engine.input.newWindowSize.x;
    game_state->engine.window->height =
      game_state->engine.input.newWindowSize.y;
    // Update the pixel-perfect rescaling viewport adapter
    binocle_viewport_adapter_reset(game_state->camera.viewport_adapter,
                                   oldWindowSize,
                                   game_state->engine.input.newWindowSize);
    game_state->engine.input.resized = false;
  }

  if (binocle_input_is_key_pressed(&game_state->engine.input, KEY_RIGHT)) {
    game_state->player_pos.x +=
      50.0f * (1.0f / game_state->engine.window->frame_time);
  } else if (binocle_input_is_key_pressed(&game_state->engine.input,
                                          KEY_LEFT)) {
    game_state->player_pos.x -=
      50.0f * (1.0f / game_state->engine.window->frame_time);
  }

  if (binocle_input_is_key_pressed(&game_state->engine.input, KEY_UP)) {
    game_state->player_pos.y +=
      50.0f * (1.0f / game_state->engine.window->frame_time);
  } else if (binocle_input_is_key_pressed(&game_state->engine.input,
                                          KEY_DOWN)) {
    game_state->player_pos.y -=
      50.0f * (1.0f / game_state->engine.window->frame_time);
  }

  if (binocle_input_is_key_pressed(&game_state->engine.input, KEY_SPACE)) {
    binocle_audio_play_sound(game_state->engine.sound);
  }

  if (binocle_input_is_key_pressed(&game_state->engine.input, KEY_1)) {
    float volume;
    binocle_audio_get_master_volume(&game_state->engine.audio, &volume);
    binocle_audio_set_master_volume(&game_state->engine.audio, volume - 0.2f);
  }
  if (binocle_input_is_key_pressed(&game_state->engine.input, KEY_2)) {
    float volume;
    binocle_audio_get_master_volume(&game_state->engine.audio, &volume);
    binocle_audio_set_master_volume(&game_state->engine.audio, volume + 0.2f);
  }

  kmVec2 mouse_pos;
  mouse_pos.x = (float)game_state->engine.input.mouseX;
  mouse_pos.y = (float)game_state->engine.input.mouseY;
  kmVec2 mouse_world_pos =
    binocle_camera_screen_to_world_point(game_state->camera, mouse_pos);

#ifdef WITH_PHYSICS
  cpVect pos = cpBodyGetPosition(ps.ball_body);
  cpBB bb = cpShapeGetBB(ps.ball_shape);

  kmAABB2 ball_bounds;
  ball_bounds.min.x = bb.l;
  ball_bounds.min.y = bb.b;
  ball_bounds.max.x = bb.r;
  ball_bounds.max.y = bb.t;
  if (kmAABB2ContainsPoint(&ball_bounds, &mouse_world_pos) &&
      binocle_input_is_mouse_down(game_state->engine.input, MOUSE_LEFT)) {
    ps.dragging_ball = true;
    binocle_log_info("caught");
  }

  if (ps.dragging_ball &&
      binocle_input_is_mouse_pressed(game_state->engine.input, MOUSE_LEFT)) {
    // set position
    cpBodySetPosition(ps.ball_body, cpv(mouse_world_pos.x, mouse_world_pos.y));

    // apply force
    //    dFloat mass;
    //    dFloat Ixx;
    //    dFloat Iyy;
    //    dFloat Izz;
    //
    //    NewtonBodyGetMass(ball_body, &mass, &Ixx, &Iyy, &Izz);
    //    float gravityForce[4] = {10 * (mouse_world_pos.x - mouse_prev_pos.x),
    //    0.0f, 0.0f, 0.0f}; NewtonBodySetVelocity(ball_body, &gravityForce[0]);
  }

  if (binocle_input_is_mouse_up(game_state->engine.input, MOUSE_LEFT)) {
    ps.dragging_ball = false;
  }
#endif

  // Main render loop

  // Create a viewport that corresponds to the size of our render target
  kmAABB2 viewport;
  viewport.min.x = 0;
  viewport.min.y = 0;
  viewport.max.x = DESIGN_WIDTH;
  viewport.max.y = DESIGN_HEIGHT;

#ifdef WITH_PHYSICS
  advance_simulation(dt);
#endif

  kmVec2 scale;
  scale.x = 1.0f;
  scale.y = 1.0f;

  update_game(dt);
  draw_game();

  binocle_sprite_draw(game_state->player, &game_state->engine.gd,
                      (int64_t)game_state->player_pos.x,
                      (int64_t)game_state->player_pos.y, &viewport, 0, &scale,
                      &game_state->camera, 0, NULL);

  kmMat4 view_matrix;
  kmMat4Identity(&view_matrix);

#ifdef WITH_PHYSICS
  binocle_sprite_draw(ps.ball_sprite, &game_state->engine.gd,
                      (int64_t)pos.x - 8, (int64_t)pos.y - 8, &viewport, 0,
                      &scale, &game_state->camera, 0, NULL);
  char mouse_str[256];
  sprintf(mouse_str, "x: %.0f y:%.0f %d", mouse_world_pos.x, mouse_world_pos.y,
          ps.dragging_ball);
  binocle_bitmapfont_draw_string(
    game_state->engine.font, mouse_str, 32, &game_state->engine.gd, 0,
    DESIGN_HEIGHT - 70, viewport, binocle_color_white(), view_matrix, 0);
#endif

  char fps_str[256];
  sprintf(fps_str, "FPS: %llu",
          binocle_window_get_fps(game_state->engine.window));
  binocle_bitmapfont_draw_string(
    game_state->engine.font, fps_str, 32, &game_state->engine.gd, 0,
    DESIGN_HEIGHT - 32, viewport, binocle_color_white(), view_matrix, 0);
  // binocle_sprite_draw(font_sprite, &gd, (uint64_t)font_sprite_pos.x,
  // (uint64_t)font_sprite_pos.y, adapter.viewport);

  // Gets the viewport calculated by the adapter
  kmAABB2 vp =
    binocle_viewport_adapter_get_viewport(*game_state->engine.adapter);

  // Render to screen
  binocle_gd_render(&game_state->engine.gd, game_state->engine.window,
                    DESIGN_WIDTH, DESIGN_HEIGHT, vp,
                    game_state->camera.viewport_adapter->scale_matrix,
                    game_state->camera.viewport_adapter->inverse_multiplier);

  binocle_window_refresh(game_state->engine.window);
  binocle_window_end_frame(game_state->engine.window);
  // binocle_log_info("FPS: %d", binocle_window_get_fps(&window));

#ifdef WITH_PHYSICS
  ps.mouse_prev_pos = mouse_world_pos;
#endif
}
#endif

#ifdef DEMOLOOP

binocle_render_target g_buffer;
binocle_render_target dof_buffer;
binocle_render_target dof_far_buffer1;
binocle_render_target dof_far_buffer2;
binocle_render_target dof_far_buffer3;
binocle_render_target dof_near_buffer1;
binocle_render_target dof_near_buffer2;
binocle_render_target bloom_depth_buffer;
binocle_render_target bloom_buffer1;
binocle_render_target bloom_buffer2;
binocle_render_target masking_buffer;
binocle_render_target aberration_buffer;
binocle_render_target compose_buffer;
binocle_render_target fxaa_buffer;

void init_render_targets() {
  g_buffer = binocle_gd_create_render_target(window.width, window.height, false, GL_RGBA);
  dof_buffer = binocle_gd_create_render_target(window.width, window.height, false, GL_RGBA);
  dof_far_buffer1 = binocle_gd_create_render_target(window.width, window.height, false, GL_RGBA);
  dof_far_buffer2 = binocle_gd_create_render_target(window.width, window.height, false, GL_RGBA);
  dof_far_buffer3 = binocle_gd_create_render_target(window.width, window.height, false, GL_RGBA);
  dof_near_buffer1 = binocle_gd_create_render_target(window.width, window.height, false, GL_RGBA);
  dof_near_buffer2 = binocle_gd_create_render_target(window.width, window.height, false, GL_RGBA);
  bloom_depth_buffer = binocle_gd_create_render_target(window.width, window.height, false, GL_RGBA);
  bloom_buffer1 = binocle_gd_create_render_target(window.width, window.height, false, GL_RGBA);
  bloom_buffer2 = binocle_gd_create_render_target(window.width, window.height, false, GL_RGBA);
  masking_buffer = binocle_gd_create_render_target(window.width, window.height, false, GL_RGBA);
  aberration_buffer = binocle_gd_create_render_target(window.width, window.height, false, GL_RGBA);
  compose_buffer = binocle_gd_create_render_target(window.width, window.height, false, GL_RGBA);
  fxaa_buffer = binocle_gd_create_render_target(window.width, window.height, false, GL_RGBA);
}

void draw_scene() {
  binocle_gd_apply_shader(&gd, sdf_shader);
  binocle_gd_set_uniform_float(sdf_shader, "time", running_time);
  binocle_gd_set_uniform_float2(sdf_shader, "resolution", window.width, window.height);
  // TODO: set all other uniforms
  binocle_gd_draw_quad(sdf_shader); // Draws the current vertices buffer to the render target
}

void gbuffer_pass() {
  binocle_gd_set_render_target(g_buffer);
  kmAABB2 vp = {.min.x = 0, .min.y = 0, .max.x = window.width, .max.y = window.height};
  binocle_gd_apply_viewport(vp);
  binocle_color clear_color = {.a = 1.0, .r = 0.0, .g = 1.0, .b = 0.0};
  binocle_gd_clear(clear_color);
  draw_scene();
}

void fxaa_pass() {
  binocle_gd_set_render_target(fxaa_buffer);
  binocle_gd_apply_shader(&gd, fxaa_shader);
  binocle_gd_set_uniform_float(fxaa_shader, "time", running_time);
  binocle_gd_set_uniform_float2(fxaa_shader, "resolution", window.width, window.height);
  binocle_gd_set_uniform_render_target_as_texture(fxaa_shader, "texture", g_buffer);
  binocle_gd_draw_quad(fxaa_shader);
}

void dof_pass() {
  binocle_gd_set_render_target(dof_buffer);
  binocle_gd_apply_shader(&gd, dof_shader);
  binocle_gd_set_uniform_float(dof_shader, "time", running_time);
  binocle_gd_set_uniform_float2(dof_shader, "resolution", window.width, window.height);
  binocle_gd_set_uniform_float2(dof_shader, "uv", 1.0, 1.0);
  binocle_gd_set_uniform_render_target_as_texture(dof_shader, "tInput", g_buffer);
  binocle_gd_set_uniform_render_target_as_texture(dof_shader, "tBias", g_buffer);
  binocle_gd_set_uniform_float(dof_shader, "radius", 0.5);
  binocle_gd_set_uniform_float(dof_shader, "amount", 1.0);
  binocle_gd_set_uniform_float(dof_shader, "focalDistance", 2.0);
  binocle_gd_set_uniform_float(dof_shader, "aperture", 2.0);
  binocle_gd_draw_quad(dof_shader);
}

void bloom_pass() {
  binocle_gd_set_render_target(bloom_buffer1);
  binocle_gd_apply_shader(&gd, bloom_shader);
  binocle_gd_set_uniform_float(bloom_shader, "time", running_time);
  binocle_gd_set_uniform_float2(bloom_shader, "resolution", window.width, window.height);
  binocle_gd_set_uniform_float2(bloom_shader, "uv", 1.0, 1.0);
  binocle_gd_set_uniform_render_target_as_texture(bloom_shader, "tInput", g_buffer);
  binocle_gd_draw_quad(bloom_shader);
}

void compose_pass() {

}

void masking_pass() {

}

void aberration_pass() {

}

void tonemap_pass() {

}

void screen_pass() {
  //binocle_gd_set_render_target(g_buffer);
  binocle_gd_apply_shader(&gd, quad_shader);
  binocle_gd_set_uniform_float(quad_shader, "time", running_time);
  binocle_gd_set_uniform_float2(quad_shader, "resolution", window.width, window.height);
  if (binocle_input_is_key_pressed(input, KEY_1)) {
    binocle_gd_draw_quad_to_screen(quad_shader, fxaa_buffer);
  } else if (binocle_input_is_key_pressed(input, KEY_2)) {
    binocle_gd_draw_quad_to_screen(quad_shader, dof_buffer);
  } else if (binocle_input_is_key_pressed(input, KEY_3)) {
    binocle_gd_draw_quad_to_screen(quad_shader, bloom_buffer1);
  } else {
    binocle_gd_draw_quad_to_screen(quad_shader, g_buffer);
  }
}

void render_loop() {
  init_render_targets();
  gbuffer_pass();
  fxaa_pass();
  dof_pass();
  bloom_pass();
  compose_pass();
  masking_pass();
  aberration_pass();
  tonemap_pass();
  screen_pass();
}

void main_loop() {
  binocle_window_begin_frame(&window);
  binocle_input_update(&input);

  if (input.resized) {
    kmVec2 oldWindowSize = {.x = window.width, .y = window.height};
    window.width = input.newWindowSize.x;
    window.height = input.newWindowSize.y;
    binocle_viewport_adapter_reset(&adapter, oldWindowSize, input.newWindowSize);
    input.resized = false;
  }


  binocle_window_clear(&window);

  render_loop();
  running_time += (binocle_window_get_frame_time(&window) / 1000.0);
  char fps_str[256];
  sprintf(fps_str, "FPS: %d", binocle_window_get_fps(&window));
  kmMat4 view_matrix;
  kmMat4Identity((&view_matrix));
  binocle_bitmapfont_draw_string(font, fps_str, 32, &gd, 0, window.height - 32, adapter.viewport, binocle_color_white(), view_matrix);

  binocle_window_refresh(&window);
  binocle_window_end_frame(&window);
}
#endif

int main(int argc, char *argv[]) {
  binocle_app_desc_t app_desc = {0};
  app = binocle_app_new();
  binocle_app_init(&app, &app_desc);

  game_state = binocle_memory_bootstrap_push_struct(
    game_state_t, engine.main_arena, binocle_memory_default_bootstrap_params(),
    binocle_memory_default_arena_params());
  game_state->engine.frame_arena = binocle_memory_bootstrap_push_size(
    BINOCLE_DEBUG_MEMORY_NAME("frame_arena") sizeof(binocle_memory_arena), 0,
    binocle_memory_non_restored_arena_bootstrap_params(),
    binocle_memory_default_arena_params());
  binocle_string s1 =
    binocle_memory_push_cstring(game_state->engine.main_arena, "Something");
  binocle_string s2 =
    binocle_memory_push_cstring(game_state->engine.main_arena, "Different");
  binocle_log_info("%s", s1.data);
  binocle_log_info("%s", s2.data);

  game_state->engine.window =
    binocle_window_new(DESIGN_WIDTH, DESIGN_HEIGHT, "Binocle Test Game");
  binocle_window_set_background_color(game_state->engine.window,
                                      binocle_color_azure());
  binocle_window_set_minimum_size(game_state->engine.window, DESIGN_WIDTH,
                                  DESIGN_HEIGHT);
  game_state->engine.adapter = binocle_viewport_adapter_new(
    game_state->engine.window, BINOCLE_VIEWPORT_ADAPTER_KIND_SCALING,
    BINOCLE_VIEWPORT_ADAPTER_SCALING_TYPE_PIXEL_PERFECT,
    game_state->engine.window->original_width,
    game_state->engine.window->original_height,
    game_state->engine.window->original_width,
    game_state->engine.window->original_height);
  game_state->camera = binocle_camera_new(game_state->engine.adapter);
  game_state->engine.input = binocle_input_new();
  game_state->engine.gd = binocle_gd_new();
  binocle_gd_init(&game_state->engine.gd, game_state->engine.window);

  game_state->engine.binocle_data_dir = binocle_sdl_assets_dir();
  binocle_log_info("Current base path: %s",
                   game_state->engine.binocle_data_dir);

  char filename[1024];
  sprintf(filename, "%s%s", game_state->engine.binocle_data_dir,
          "wabbit_alpha.png");
  game_state->wabbit_image = binocle_image_load(filename);

  sprintf(filename, "%s%s", game_state->engine.binocle_data_dir, "player.png");
  sg_image ball_image = binocle_image_load(filename);

  // Default shader (off-screen texture)
  char vert[1024];
#if defined(__IPHONEOS__)
  sprintf(vert, "%s%s", binocle_data_dir, DEFAULT_VS_FILENAME);
#else
  sprintf(vert, "%sshaders/%s/%s", game_state->engine.binocle_data_dir,
          SHADER_PATH, DEFAULT_VS_FILENAME);
#endif

  char frag[1024];
#if defined(__IPHONEOS__)
  sprintf(frag, "%s%s", binocle_data_dir, DEFAULT_FS_FILENAME);
#else
  sprintf(frag, "%sshaders/%s/%s", game_state->engine.binocle_data_dir,
          SHADER_PATH, DEFAULT_FS_FILENAME);
#endif

  char *shader_vs_src;
  size_t shader_vs_src_size;
  binocle_sdl_load_text_file(vert, &shader_vs_src, &shader_vs_src_size);

  char *shader_fs_src;
  size_t shader_fs_src_size;
  binocle_sdl_load_text_file(frag, &shader_fs_src, &shader_fs_src_size);

  sg_shader_desc default_shader_desc = {
    .label = "default-shader",
    .vs.source = shader_vs_src,
#if defined(BINOCLE_METAL)
    .vs.entry = "main0",
#endif
    .attrs =
      {
        [0] = {.name = "vertexPosition"},
        [1] = {.name = "vertexColor"},
        [2] = {.name = "vertexTCoord"},
        [3] = {.name = "vertexNormal"},
      },
    .vs.uniform_blocks[0] = {.size = sizeof(default_shader_params_t),
                             .layout = SG_UNIFORMLAYOUT_STD140,
                             .uniforms =
                               {
                                 [0] = {.name = "vs_params",
                                        .type = SG_UNIFORMTYPE_FLOAT4,
                                        .array_count = 12},
                               }},
    .fs.source = shader_fs_src,
#if defined(BINOCLE_METAL)
    .fs.entry = "main0",
#endif
    .fs.images[0] =
      {
        .used = true,
        .image_type = SG_IMAGETYPE_2D,
        .sample_type = SG_IMAGESAMPLETYPE_FLOAT,
      },
    .fs.samplers[0] =
      {
        .used = true,
        .sampler_type = SG_SAMPLERTYPE_FILTERING,
      },
    .fs.image_sampler_pairs[0] = {
      .used = true,
      .glsl_name = "tex0_smp",
      .image_slot = 0,
      .sampler_slot = 0,
    }};
  game_state->engine.default_shader = sg_make_shader(&default_shader_desc);

  // Screen shader
#if defined(__IPHONEOS__)
  sprintf(vert, "%s%s", binocle_data_dir, SCREEN_VS_FILENAME);
  sprintf(frag, "%s%s", binocle_data_dir, SCREEN_FS_FILENAME);
#else
  sprintf(vert, "%sshaders/%s/%s", game_state->engine.binocle_data_dir,
          SHADER_PATH, SCREEN_VS_FILENAME);
  sprintf(frag, "%sshaders/%s/%s", game_state->engine.binocle_data_dir,
          SHADER_PATH, SCREEN_FS_FILENAME);
#endif

  char *screen_shader_vs_src;
  size_t screen_shader_vs_src_size;
  binocle_sdl_load_text_file(vert, &screen_shader_vs_src,
                             &screen_shader_vs_src_size);

  char *screen_shader_fs_src;
  size_t screen_shader_fs_src_size;
  binocle_log_info("reading screen shader");
  binocle_sdl_load_text_file(frag, &screen_shader_fs_src,
                             &screen_shader_fs_src_size);
  binocle_log_info("done reading screen shader");

  sg_shader_desc screen_shader_desc = {
    .label = "screen-shader",
    .vs.source = screen_shader_vs_src,
#if defined(BINOCLE_METAL)
    .vs.entry = "main0",
#endif
    .attrs =
      {
        [0] = {.name = "position"},
        //      [1] = {.name = "color"},
        //      [2] = {.name = "tex"},
      },
    .vs.uniform_blocks[0] =
      {
        .size = sizeof(screen_shader_vs_params_t),
        .layout = SG_UNIFORMLAYOUT_STD140,
        .uniforms =
          {
            [0] = {.name = "vs_params",
                   .type = SG_UNIFORMTYPE_FLOAT4,
                   .array_count = 4},
          },
      },
    .fs.source = screen_shader_fs_src,
#if defined(BINOCLE_METAL)
    .fs.entry = "main0",
#endif
    .fs.images[0] =
      {
        .used = true,
        .image_type = SG_IMAGETYPE_2D,
        .sample_type = SG_IMAGESAMPLETYPE_FLOAT,
      },
    .fs.samplers[0] =
      {
        .used = true,
        .sampler_type = SG_SAMPLERTYPE_FILTERING,
      },
    .fs.image_sampler_pairs[0] =
      {
        .used = true,
        .glsl_name = "tex0_smp",
        .image_slot = 0,
        .sampler_slot = 0,
      },
    .fs.uniform_blocks[0] =
      {
        .size = sizeof(screen_shader_fs_params_t),
        .layout = SG_UNIFORMLAYOUT_STD140,
        .uniforms =
          {
            [0] = {.name = "fs_params",
                   .type = SG_UNIFORMTYPE_FLOAT4,
                   .array_count = 2},
          },
      },
  };
  binocle_log_info("creating screen shader");
  game_state->engine.screen_shader = sg_make_shader(&screen_shader_desc);
  binocle_log_info("done creating screen shader");

  binocle_material *material = binocle_material_new();
  material->albedo_texture = game_state->wabbit_image;
  material->shader = game_state->engine.default_shader;
  game_state->player = binocle_sprite_from_material(material);
  game_state->player_pos.x = 50;
  game_state->player_pos.y = 50;

#ifdef WITH_PHYSICS
  ps = setup_world();
  binocle_material *ball_material = binocle_material_new();
  ball_material->albedo_texture = ball_image;
  ball_material->shader = game_state->engine.default_shader;
  ps.ball_sprite = binocle_sprite_from_material(ball_material);
#endif

#if !defined(ANDROID)
  sprintf(filename, "%s%s", game_state->engine.binocle_data_dir,
          "test_simple.lua");
  lua_test(filename);
  sprintf(filename, "%s%s", game_state->engine.binocle_data_dir,
          "test_simple2.lua");
  lua_test2(filename);
  sprintf(filename, "%s%s", game_state->engine.binocle_data_dir,
          "test_profiler.lua");
  lua_test_profiler(filename);
  // sprintf(filename, "%s%s", binocle_data_dir, "test_ffi.lua");
  // lua_testffi(filename, &window);
#endif

#ifdef DEMOLOOP
  // Load the default quad shader
  sprintf(vert, "%s%s", binocle_data_dir, "screen.vert");
  sprintf(frag, "%s%s", binocle_data_dir, "screen.frag");
  quad_shader = binocle_shader_load_from_file(vert, frag);

  // Load the SDF shader
  sprintf(vert, "%s%s", binocle_data_dir, "test.vert");
  sprintf(frag, "%s%s", binocle_data_dir, "test2.frag");
  sdf_shader = binocle_shader_load_from_file(vert, frag);

  // Load the FXAA shader
  sprintf(vert, "%s%s", binocle_data_dir, "test.vert");
  sprintf(frag, "%s%s", binocle_data_dir, "fxaa.frag");
  fxaa_shader = binocle_shader_load_from_file(vert, frag);

  // Load the DOF shader
  sprintf(vert, "%s%s", binocle_data_dir, "dof.vert");
  sprintf(frag, "%s%s", binocle_data_dir, "dof.frag");
  dof_shader = binocle_shader_load_from_file(vert, frag);

  // Load the bloom shader
  sprintf(vert, "%s%s", binocle_data_dir, "dof.vert");
  sprintf(frag, "%s%s", binocle_data_dir, "bloom2.frag");
  bloom_shader = binocle_shader_load_from_file(vert, frag);
#endif

  char font_filename[1024];
  sprintf(font_filename, "%s%s", game_state->engine.binocle_data_dir,
          "font.fnt");
  binocle_log_info("creating font");
  game_state->engine.font = binocle_bitmapfont_from_file(font_filename, true);
  binocle_log_info("done creating font");

  char font_image_filename[1024];
  sprintf(font_image_filename, "%s%s", game_state->engine.binocle_data_dir,
          "font.png");
  game_state->engine.font_texture = binocle_image_load(font_image_filename);
  game_state->engine.font_material = binocle_material_new();
  game_state->engine.font_material->albedo_texture =
    game_state->engine.font_texture;
  game_state->engine.font_material->shader = game_state->engine.default_shader;
  game_state->engine.font->material = game_state->engine.font_material;
  game_state->engine.font_sprite =
    binocle_sprite_from_material(game_state->engine.font_material);

  binocle_log_info("audio init");
  game_state->engine.audio = binocle_audio_new();
  binocle_audio_init(&game_state->engine.audio);
  binocle_log_info("done audio init");
  char sound_filename[1024];
  sprintf(sound_filename, "%s%s", game_state->engine.binocle_data_dir,
          "Jump.wav");
  game_state->engine.sound =
    binocle_audio_load_sound(&game_state->engine.audio, sound_filename);
  char music_filename[1024];
  sprintf(music_filename, "%s%s", game_state->engine.binocle_data_dir,
          "8bit.ogg");
  game_state->engine.music =
    binocle_audio_load_music_stream(&game_state->engine.audio, music_filename);
  binocle_audio_play_music_stream(&game_state->engine.music);
  binocle_audio_set_music_volume(&game_state->engine.music, 0.00f);

  binocle_gd_setup_default_pipeline(
    &game_state->engine.gd, DESIGN_WIDTH, DESIGN_HEIGHT,
    game_state->engine.default_shader, game_state->engine.screen_shader);

  setup_game(game_state);

#if defined(BINOCLE_HTTP)
  test_http_get();
  test_http_post();
  test_http_put();
#endif

  game_state->engine.running_time = 0;
#ifdef GAMELOOP
  binocle_game_run(window, input);
#else
#ifdef __EMSCRIPTEN__
  emscripten_set_main_loop(main_loop, 0, 1);
#else
  while (!game_state->engine.input.quit_requested) {
    main_loop();
  }
#endif // __EMSCRIPTEN__
  binocle_log_info("Quit requested");
#endif // GAMELOOP
  binocle_audio_unload_sound(&game_state->engine.audio,
                             game_state->engine.sound);
  binocle_audio_unload_music_stream(&game_state->engine.audio,
                                    &game_state->engine.music);
  binocle_audio_destroy(&game_state->engine.audio);
#ifdef WITH_PHYSICS
  destroy_world();
#endif
  binocle_gd_destroy(&game_state->engine.gd);
  binocle_app_destroy(&app);
  binocle_sdl_exit();
}
