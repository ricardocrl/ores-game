/*
 * BoxContainer.cpp
 *
 *  Created on: 23/04/2018
 *      Author: roliveira
 */

#include "BoxContainer.h"

#include "Box.h"
#include "AppUtils.hpp"
#include "AppTypes.h"

#include <functional>

std::string BoxContainer::backgroundFile = "res/background.jpeg";
SDL_Texture * BoxContainer::backgroundTexture = nullptr;
std::vector<std::shared_ptr<Box>> BoxContainer::blowingBoxes;

BoxContainer::BoxContainer() : lastUpdate(0), shakeSpeed(0), shakeGoal(0), offset_x(0) {
    Box::subscribeLeanedCbk(std::bind(&BoxContainer::leanedBoxSave_cbk, this, std::placeholders::_1, std::placeholders::_2));
    resetQueues();
}


bool BoxContainer::OnLoad(SDL_Renderer * renderer) {
    BoxContainer::backgroundTexture = apputils::loadImg(BoxContainer::backgroundFile, renderer);
    if (!BoxContainer::backgroundTexture) {
        return false;
    } else {
        return Box::OnLoad(renderer);
    }
}


void BoxContainer::OnCleanUp() {
    Box::OnCleanUp();
    SDL_DestroyTexture(BoxContainer::backgroundTexture);
}

std::pair<int, int> BoxContainer::getXYFromListIndexes(const int queueIdx, const int boxIdx, const bool shake) {
    int x = APP_SIZE_W + APP_PLAY_OFFSET_X - ((boxQueues.size() - queueIdx) * BOX_SIZE);
    if (shake) {
        x += offset_x;
    }
    int y = APP_SIZE_H + APP_PLAY_OFFSET_Y - ((boxIdx + 1) * BOX_SIZE);
    return {x, y};
}

void BoxContainer::OnRender(SDL_Renderer * renderer) {
    SDL_RenderCopy(renderer, BoxContainer::backgroundTexture, nullptr, nullptr);

    int queueIdx = 0;
    for (auto begin = boxQueues.begin(); begin != boxQueues.end(); ++begin) {
        int boxIdx = 0;
        for (auto & box : *begin) {
            if (!box) {
                std::cout << "nullptr box in position " << queueIdx << " " << boxIdx << std::endl;
                continue;
            }
            auto pos = getXYFromListIndexes(queueIdx, boxIdx);
            box->OnRender(renderer, pos.first, pos.second);
            boxIdx++;
        }
        queueIdx++;
    }
    for (auto & box : blowingBoxes) {
        box->OnRender(renderer);
    }
}


void BoxContainer::shake(int magn, double speed, int duration) {
    shakeGoal = magn;
    shakeSpeed = speed;
    shakeDuration = duration;
    shakeStart = SDL_GetTicks();
}


std::pair<int, int> BoxContainer::getBoxQueuePos(int mouse_x, int mouse_y) {
    int queueId = (APP_SIZE_W + APP_PLAY_OFFSET_X - mouse_x) / BOX_SIZE;
    queueId = boxQueues.size() - queueId - 1;
    int boxId = (APP_SIZE_H + APP_PLAY_OFFSET_Y - mouse_y) / BOX_SIZE;
    return {queueId, boxId};
}


void BoxContainer::markForBlowAround(BoxColor color, int queue, int box) {
    std::pair<int, int> around[4] = {
            { queue + 1, box },
            { queue - 1, box },
            { queue, box + 1 },
            { queue, box - 1 } };

	auto pos = getXYFromListIndexes(queue, box);
	getBox(queue, box)->blowFrom(pos.first, pos.second);

    for (int i = 0; i < 4; ++i) {
        if (around[i].first < 0 || around[i].second < 0) {
            continue;
        }
        std::shared_ptr<Box> aroundB = getBox(around[i].first, around[i].second);

        if (aroundB && !aroundB->isBlowing() && aroundB->color == color) {
            markForBlowAround(color, around[i].first, around[i].second);
        }
    }
}


std::shared_ptr<Box> BoxContainer::getBox(Uint8 queue, Uint8 box) {
    if (queue >= boxQueues.size()) {
        return nullptr;
    }
    auto itQ = boxQueues.begin();
    std::advance(itQ, queue);
    if (itQ == boxQueues.end()) {
        return nullptr;
    }
    if (box >= itQ->size()) {
        return nullptr;
    }
    auto itB = itQ->begin();
    std::advance(itB, box);
    if (itB == itQ->end()) {
        return nullptr;
    }
    return *itB;
}


void BoxContainer::blowBox(int queueIdx, int boxIdx) {
    std::shared_ptr<Box> box;
    if (!(box = getBox(queueIdx, boxIdx))) {
        return;
    }
    markForBlowAround(box->color, queueIdx, boxIdx);
    removeBlowingBoxes();
}

void BoxContainer::removeBlowingBoxes() {
    for (auto itQ = boxQueues.begin(); itQ != boxQueues.end();) {
        for (auto itB = itQ->begin(); itB != itQ->end();) {
            if ((*itB)->isBlowing()) {
                /**
                 * push box to a seperate container to keep it
                 * alive while the animation runs (until blowing = false)
                 */
                BoxContainer::blowingBoxes.push_back(*itB);

                itB = itQ->erase(itB);
                for (auto itBNext = itB; itBNext != itQ->end(); ++itBNext) {
                    (*itBNext)->fallFrom(BOX_SIZE);
                }
            } else {
                ++itB;
            }
        }
        if ((*itQ).empty()) {
            itQ = boxQueues.erase(itQ);
            for (auto it = boxQueues.begin(); it != itQ; ++it) {
                for (auto & box : *it) {
                    box->slideFrom(-BOX_SIZE);
                }
            }
        } else {
            ++itQ;
        }
    }
}

void BoxContainer::leanedBoxProcess(std::shared_ptr<Box> box, BoxLeanDir direction) {
    if (!box || direction == BoxLeanDir::ORIGIN) {
        return;
    }
    bool found = false;
    for (auto itQ = boxQueues.begin(); itQ != boxQueues.end(); ++itQ) {
        int boxIdx = 0;
        for (auto itB = itQ->begin(); itB != itQ->end();) {
            if (*itB == box) {
                found = true;

                std::list<Queue>::iterator itNewQ;
                if (direction == BoxLeanDir::LEFT) {
                    itNewQ = std::prev(itQ);
                } else {
                    itNewQ = std::next(itQ);
                }
                if (itNewQ == boxQueues.end()) {
                    // Could happen if initially there was a "next" queue but
                    // by the time leaning completed, the queue is no longer there
                    box->leanTo(BoxLeanDir::ORIGIN);
                } else if (boxIdx - static_cast<int>(itNewQ->size()) < 0) {
                    // Cannot move anymore
                    // Probably some other box fell there in the meantime
                    box->leanTo(BoxLeanDir::ORIGIN);
                } else {
                    // Remove current position
                    itB = itQ->erase(itB);

                    // Update fall of above boxes
                    for (auto itBNext = itB; itBNext != itQ->end(); ++itBNext) {
                        (*itBNext)->fallFrom(BOX_SIZE);
                    }

                    // Store to new position
                    itNewQ->push_back(box);

                    // Box is now straight in the new queue
                    box->resetAngle();

                    // Set fall for the new column
                    Uint16 deltaHeight = BOX_SIZE * (boxIdx - static_cast<int>(itNewQ->size()) + 1);
                    box->fallFrom(deltaHeight);
                }
                break;
            } else {
                ++itB;
            }
            ++boxIdx;
        }
        if (found) {
            break;
        }
    }
}

void BoxContainer::leanBoxes(BoxLeanDir leanDir) {
    std::size_t prevQSize = BOX_QUEUE_HEIGHT;
    std::size_t nextQSize = 0;
    for (auto itQ = boxQueues.begin(); itQ != boxQueues.end(); ++itQ) {
        if (std::next(itQ) != boxQueues.end()) {
            nextQSize = std::next(itQ)->size();
        } else {
            nextQSize = BOX_QUEUE_HEIGHT;
        }
        std::size_t currQSize = itQ->size();

        if (leanDir == BoxLeanDir::LEFT && currQSize > prevQSize) {
            itQ->back()->leanTo(leanDir);
        } else if (leanDir == BoxLeanDir::RIGHT && currQSize > nextQSize) {
            itQ->back()->leanTo(leanDir);
        } else if (leanDir == BoxLeanDir::ORIGIN) {
            itQ->back()->leanTo(leanDir);
        }
        prevQSize = currQSize;
    }
}

bool BoxContainer::addQueue(const int n, bool init) {
    for (int i = 0; i < n; ++i) {
        if (!addQueue(init)) {
            return false;
        }
    }
    return true;
}

std::vector<std::pair<int,int>> BoxContainer::getNeighbours(int queue, int box) {
    std::pair<int, int> candidates[4] = {
            { queue + 1, box },
            { queue - 1, box },
            { queue, box + 1 },
            { queue, box - 1 } };
    std::vector<std::pair<int, int>> neighbours;
    for (int i = 0; i < 4; ++i) {
        if (candidates[i].first < 0 || candidates[i].second < 0) {
            continue;
        }
        if (static_cast<int>(boxQueues.size()) > candidates[i].first) {
            auto it = boxQueues.begin();
            std::advance(it, candidates[i].first);
            if (it != boxQueues.end() && static_cast<int>(it->size()) > candidates[i].second) {
                neighbours.push_back(candidates[i]);
            }
        }
    }
    return neighbours;
}

bool BoxContainer::addQueue(const bool init) {
    boxQueues.push_back(Queue());

    for (int i = 0; i < BOX_QUEUE_HEIGHT; ++i) {

        // Randomize color or take the same color from a neighbor to
        // increase playability
        BoxColor  color = BoxColor(rand()%static_cast<int>(BoxColor::COUNT));
        if (rand()%100+1 < BOX_SAME_COLOR_PROB) {
            auto neighbors = getNeighbours(boxQueues.size()-1, i);
            if (!neighbors.empty()) {
                auto neighborIdx = rand()%(neighbors.size());
                auto boxNeighbor = getBox(neighbors[neighborIdx].first, neighbors[neighborIdx].second);
                // Here the pointer is always valid because that is what getNeighbours guarantees
                color = boxNeighbor->color;
            }
        }

        // Save in its queue
        boxQueues.back().push_back(std::make_shared<Box>(BoxColor(color)));
        boxQueues.back().back()->fallFrom(BOX_QUEUE_HEIGHT * BOX_SIZE);
    }
    if (!init) {
        for (auto it = boxQueues.begin(); std::next(it) != boxQueues.end(); ++it) {
            for (auto & box : *it) {
                box->slideFrom(BOX_SIZE);
            }
        }
    }
    /*
     *  Let the queue be inserted before checking the limit
     * to allow the user to see why the game will be over
     */
    if (boxQueues.size() == BOX_COUNT_MAX) {
        return false;
    }
    return true;
}

void BoxContainer::resetQueues() {
    boxQueues.erase(boxQueues.begin(), boxQueues.end());
    addQueue(BOX_COUNT_INIT, true);
}

void BoxContainer::earthquakeLoop() {
    auto time = SDL_GetTicks();
    if (time < lastUpdate + updatePosRate) {
        return;
    }
    lastUpdate += updatePosRate;

    // Either shaking or stabilizing
    if (shakeGoal != 0 || (offset_x * shakeSpeed) < 0) {
        offset_x += (shakeSpeed * deltaT);
    }
    if (shakeGoal != 0) {
        if (time < shakeStart + shakeDuration) {
            // Keep shaking
            if ((shakeGoal - offset_x) * shakeSpeed < 0) {
                offset_x = shakeGoal;
                shakeGoal = -shakeGoal;
                shakeSpeed = -shakeSpeed;
                if (shakeSpeed > 0) {
                    leanBoxes(BoxLeanDir::LEFT);
                } else {
                    leanBoxes(BoxLeanDir::RIGHT);
                }
            }
        } else {
            shakeGoal = 0;
            // Force offset to go to zero
            if (offset_x * shakeSpeed > 0) {
                shakeSpeed = -shakeSpeed;
            }
        }
    }
}

void BoxContainer::OnLoop() {
    for (auto & queue : boxQueues) {
        for (auto & box : queue) {
            box->OnLoop();
        }
    }
    for (auto & leanedBox : leanedBoxes) {
        leanedBoxProcess(leanedBox.first, leanedBox.second);
    }
    leanedBoxes.clear();
    for (auto it = blowingBoxes.begin(); it != blowingBoxes.end();) {
        if ((*it)->isBlowing()) {
            (*it)->OnLoop();
            ++it;
        } else {
            it = blowingBoxes.erase(it);
        }
    }
    earthquakeLoop();
}
