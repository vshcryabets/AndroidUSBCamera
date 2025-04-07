/*
 * Copyright 2025 vschryabets@gmail.com
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "SdlTools.h"

SdlWindow::SdlWindow(const char* title, int width, int height) {
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, 0);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
}

SdlWindow::~SdlWindow() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void SdlWindow::clear() const {
    SDL_RenderClear(renderer);
}

SDL_Renderer* SdlWindow::getRenderer() const {
    return renderer;
}

SdlLoop::SdlLoop() {
}

SdlLoop::~SdlLoop() {
}
void SdlLoop::onEvent(const SDL_Event& event) {
    if (event.type == SDL_QUIT) {
        stop();
    }
}

void SdlLoop::loop() {
    SDL_Event event;
    while (running) {                
        while (SDL_PollEvent(&event)) {
            onEvent(event);
        }
        draw();
        SDL_Delay(1000 / 60); // ~60 fps
        ++frame;
    }
}

void SdlLoop::draw() {
    if (drawCallback) {
        drawCallback(frame);
    }
}
