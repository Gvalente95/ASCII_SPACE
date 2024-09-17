#include "../../headers/1_includes.h"

SDL_Window *Init_window() {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) printf("SDL Initialization Failed: %s\n", SDL_GetError());
  SDL_Window *window = SDL_CreateWindow("ASCII Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_SHOWN);
  if (!window) {
    printf("Window Creation Failed: %s\n", SDL_GetError());
    SDL_Quit();
    return NULL;
  }
  SDL_RaiseWindow(window);
  // SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);

  return (window);
}

TTF_Font *init_ttf_fond() {
  if (TTF_Init() == -1) {
    printf("TTF Initialization Failed: %s\n", TTF_GetError());
    return NULL;
  }
  TTF_Font *font = TTF_OpenFont("Century751 No2 BT Bold.ttf", 16); // Ensure you have a valid font path
  if (!font) {
    printf("Font Load Failed: %s\n", TTF_GetError());
    SDL_Quit();
    return NULL;
  }
  SDL_Color white = {255, 255, 255, 255};

  return (font);
}

SDL_Renderer *init_renderer(SDL_Window *window) {
  SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (!renderer) {
    printf("Renderer Creation Failed: %s\n", SDL_GetError());
    return NULL;
  }
  return renderer;
}
