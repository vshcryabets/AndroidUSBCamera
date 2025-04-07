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
#pragma once
#include <SDL2/SDL.h>
#include <functional>

class SdlWindow {
protected:
    SDL_Window* window {nullptr};
    SDL_Renderer* renderer {nullptr};
public:
    SdlWindow(const char* title, int width, int height);
    virtual ~SdlWindow();
    void clear() const;
    SDL_Renderer* getRenderer() const;
};

class SdlLoop {
protected:
    bool running = true;
    uint32_t frame = 0;
    std::function<void(uint32_t)> drawCallback {nullptr};
public:
    SdlLoop();
    virtual void stop() { running = false; }
    virtual ~SdlLoop();
    virtual void onEvent(const SDL_Event& event);
    virtual void loop();
    virtual void draw();
    void setDrawCallback(std::function<void(uint32_t frameNumber)> callback) { drawCallback = callback; }
};