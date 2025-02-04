#pragma once

#include <JuceHeader.h>

using namespace juce;


template<class T>
static bool reorder(OwnedArray<T>& arr, int oldIdx, int newIdx) {
    if(oldIdx != newIdx) {
        T* obj = arr[oldIdx];
            
        if(newIdx > oldIdx)
        {
            for(int i = oldIdx; i < newIdx; ++i) {
                arr.set(i, arr[i+1], false);
            }
            arr.set(newIdx, obj, false);
        } else
        {
            for(int i = oldIdx; i > newIdx; --i) {
                arr.set(i, arr[i-1], false);
            }
            arr.set(newIdx + 1, obj, false);
        }
            
        return true;
    }
    return false;
}

