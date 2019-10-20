/*
 * App.h
 *
 *  Created on: 21/04/2018
 *      Author: roliveira
 */

#ifndef APP_H_
#define APP_H_

#include "BoxContainer.h"
#include "LevelManager.h"
#include "AppTypes.h"

#include "SDL2/SDL.h"

class App {
public:
    App();
    virtual ~App() = default;

    int OnExecute();

private:

    bool OnInit();
    void OnEvent(SDL_Event* Event);
    void OnLoop();
    void OnRender();
    void OnCleanup();

    bool _running; // app running or not

    SDL_Renderer * _renderer;    // renderer for the whole app
    SDL_Window * _window;        // window associated to our renderer

    BoxContainer _boxContainer;  // Controls all actions within the boxes world
    LevelManager _levelManager;                // level manages the game level and it's rendering

    SDL_TimerID _newColumnTimer; // Timer to trigger new column added
    SDL_TimerID _earthquakeTimer;// Timer to trigger earthquake events
    SDL_TimerID _newLevelTimer;  // Timer to trigger next level jumps
};

#endif /* APP_H_ */
