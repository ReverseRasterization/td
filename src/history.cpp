#include "history.h"
#include <iostream>

void HistoryManager::undo()
{
    if (undoStack.empty()){return;}

    Action item = undoStack.back();
    undoStack.pop_back();
    redoStack.push_back(item);

    map->changeTileTexture(item.tileID, item.overlay, item.oldTex); 
}

void HistoryManager::redo()
{
    if (redoStack.empty()){return;}

    Action item = redoStack.back();
    redoStack.pop_back();
    undoStack.push_back(item);
    
    map->changeTileTexture(item.tileID, item.overlay, item.newTex);
}

void HistoryManager::recordAction(int tileID, int old_texture, int new_texture)
{
    Action nAction {
        .tileID = tileID,
        .oldTex = old_texture,
        .newTex = new_texture,
        .overlay = *overlayMode
    };

    if (undoStack.size() == 500)
    {
        undoStack = {};
    }
    undoStack.push_back(nAction);
    redoStack = {};
}