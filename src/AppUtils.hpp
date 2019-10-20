/*
 * AppUtils.cpp
 *
 *  Created on: 23/04/2018
 *      Author: roliveira
 */

#include <SDL2/SDL_image.h>

#include <string>
#include <iostream>

namespace apputils {
    /*
     * Not the best option for program size, because it generates duplicate code,
     * but it's a quick and structured solution
     */
    inline SDL_Texture* loadImg(const std::string & file, SDL_Renderer * renderer) {
        SDL_Surface* surfTemp = nullptr;

        if ((surfTemp = IMG_Load(file.c_str())) == nullptr) {
            std::cout << "Could not load file " << file << std::endl;
            return nullptr;
        }
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surfTemp);
        if (!texture) {
            std::cout << "Could not create texture from file " << file << std::endl;
            return nullptr;
        }
        SDL_FreeSurface(surfTemp);
        return texture;
    }
}


