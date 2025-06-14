#pragma once

#include <vector>
#include <memory>

#include "map.h"


class HistoryManager
{
    public:
        struct Action
        {   
            int tileID;
            int oldTex, newTex;

            bool overlay;
        };

        void undo();
        void redo();

        void recordAction(int tileID, int old_texture, int new_texture);

        HistoryManager(Map* map_instance, bool* overlayMode): map(map_instance), overlayMode(overlayMode){}

    private:
        std::vector<Action> undoStack;
        std::vector<Action> redoStack;

        bool* overlayMode;

        Map* map;

};