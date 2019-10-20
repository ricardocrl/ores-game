/*
 * App.cpp
 *
 *  Created on: 21/04/2018
 *      Author: roliveira
 */

#include "App.h"

#include <iostream>

App::App() {
    _renderer = nullptr;
    _window = nullptr;
    _running = false;
    _newColumnTimer = 0;
    _earthquakeTimer = 0;
    _newLevelTimer = 0;
}

Uint32 newColumnTimerFnc(Uint32 interval, void *param) {
    SDL_Event event;
    SDL_UserEvent userevent;

    userevent.type = SDL_USEREVENT;
    userevent.code = USR_EVT_ADDQUEUE;

    event.type = SDL_USEREVENT;
    event.user = userevent;

    SDL_PushEvent(&event);
    return(interval);
}

Uint32 earthquakeTimerFnc(Uint32 interval, void *param) {
    SDL_Event event;
    SDL_UserEvent userevent;

    userevent.type = SDL_USEREVENT;
    userevent.code = USR_EVT_EARTHQKE;

    event.type = SDL_USEREVENT;
    event.user = userevent;

    SDL_PushEvent(&event);
    interval = EARTHQKE_MIN_TIME + rand()%5000;
    return(interval);
}

Uint32 newLevelTimerFnc(Uint32 interval, void *param) {
    SDL_Event event;
    SDL_UserEvent userevent;

    userevent.type = SDL_USEREVENT;
    userevent.code = USR_EVT_NEXT_LEVEL;

    event.type = SDL_USEREVENT;
    event.user = userevent;

    SDL_PushEvent(&event);
    return(interval);
}

bool App::OnInit() {
    bool success = false;

    if(SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        std::cout << "SDL_Init failed:" << SDL_GetError() << std::endl;
    } else {
        _window = SDL_CreateWindow("Ores", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                APP_SIZE_W, APP_SIZE_H, SDL_WINDOW_RESIZABLE);

        if (!_window) {
            std::cout << "SDL_CreateWindow failed:" << SDL_GetError() << std::endl;
        } else {
            _renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_TARGETTEXTURE|SDL_RENDERER_PRESENTVSYNC);

            // Load textures for the whole app
            if (_boxContainer.OnLoad(_renderer)) {
                if (_levelManager.OnLoad(_renderer)) {
                    _levelManager.setLevel(1);
                    success = true;
                }
            }

            // Start app timers
            _newColumnTimer = SDL_AddTimer(_levelManager.getLevelDuration(), newColumnTimerFnc, nullptr);
            _earthquakeTimer = SDL_AddTimer(EARTHQKE_MIN_TIME + rand()%5000, earthquakeTimerFnc, nullptr);
            _newLevelTimer = SDL_AddTimer(LEVEL_INTERVAL, newLevelTimerFnc, nullptr);
        }
    }
    return success;
}

void App::OnEvent(SDL_Event* event) {
    switch (event->type) {
        case SDL_QUIT: {
            _running = false;
            break;
        }
        case SDL_MOUSEBUTTONDOWN: {
            switch(event->button.button) {
                case SDL_BUTTON_LEFT: {
                    auto boxPos = _boxContainer.getBoxQueuePos(event->button.x, event->button.y);
                    _boxContainer.blowBox(boxPos.first, boxPos.second);
                    break;
                }
            }
            break;
        }
        case SDL_USEREVENT: {
            switch(event->user.code) {
                case USR_EVT_ADDQUEUE: {
                    if (!_boxContainer.addQueue()) {
                        // Game over
                        _boxContainer.resetQueues();
                        _levelManager.setLevel(1);

                        // Restart new-level timer
                        SDL_RemoveTimer(_newLevelTimer);
                        _newLevelTimer = SDL_AddTimer(LEVEL_INTERVAL, newLevelTimerFnc, nullptr);
                    }
                    break;
                }
                case USR_EVT_EARTHQKE: {
                    _boxContainer.shake(5 + rand()%5, 50 + rand()%30, 1000 + rand()%2000);
                    break;
                }
                case USR_EVT_NEXT_LEVEL: {
                    if (_levelManager.next()) {

                    	// Reset box columns
                        _boxContainer.resetQueues();

                        // Restart new-column timer with a shorter interval
                        SDL_RemoveTimer(_newColumnTimer);
                        _newColumnTimer = SDL_AddTimer(_levelManager.getLevelDuration(), newColumnTimerFnc, nullptr);
                    } else {
                        // User won... let her continue to play... :-)
                    }
                    break;
                }
                default: {
                    break;
                }
            }
            break;
        }
    }
}

int App::OnExecute() {
    if(_running || !OnInit()) {
        return -1;
    } else {
        _running = true;
    }

    SDL_Event event;

    while(_running) {
        while(SDL_PollEvent(&event)) {
            OnEvent(&event);
        }

        OnLoop();
        OnRender();
    }

    OnCleanup();
    return 0;
}

void App::OnLoop() {
    _boxContainer.OnLoop();
    _levelManager.OnLoop();
}

void App::OnRender() {
    SDL_RenderClear(_renderer);

    _boxContainer.OnRender(_renderer);
    _levelManager.OnRender(_renderer);

    SDL_RenderPresent(_renderer);
}

void App::OnCleanup() {
    _boxContainer.OnCleanUp();
    _levelManager.OnCleanUp();
    SDL_DestroyRenderer(_renderer);
    SDL_DestroyWindow(_window);
    SDL_Quit();
}
