#pragma once
#include <JuceHeader.h>

class CircularArray
{
public:
    class Iterator
    {
    public:
        Iterator(const CircularArray* l, size_t amount) {
            if(amount == 0) {
                throw std::range_error("Out of bounds.");
            }
            
            arr = l;
            
            if(arr->cursorIdx == 0) {
                lastIdx = arr->size - 1;
            } else {
                lastIdx = arr->cursorIdx - 1;
            }
            
            --amount; // need this cause lastIdx and firstIdx included in range
            if(lastIdx < amount) {
                size_t delta = amount - lastIdx;
                currentIdx = l->size - delta;
            } else {
                currentIdx = lastIdx - amount;
            }
            
            startIdx = currentIdx;
        }
        
        bool hasNext() {
            return currentIdx != lastIdx + 1;
        }
        
        float getNext() {
            checkEnd();
            
            float res = arr->values[currentIdx];
            increment();
            return res;
        }
        
        /*float get(size_t index) {
            checkEnd();
            
            if(lastIdx > startIdx) {
                if(startIdx + index > lastIdx) {
                    throw std::range_error("Out of bounds.");
                } else {
                    currentIdx = startIdx + index;
                }
            } else {
                if(startIdx + index >= arr->size) {
                    size_t delta = startIdx + index - arr->size;
                    if(delta > lastIdx) {
                        throw std::range_error("Out of bounds.");
                    } else {
                        currentIdx = delta;
                    }
                }  else {
                    currentIdx = startIdx + index;
                }
            }
            
            return arr->values[currentIdx];
        }*/
        
        /*bool getRaw(float*& buffer, size_t amount) {
            bool needToDelete = false;
            
            if(lastIdx > startIdx) {
                if(startIdx + amount - 1 > lastIdx) {
                    throw std::range_error("Out of bounds.");
                } else {
                    needToDelete = false;
                    buffer = arr->values + startIdx;
                }
            } else {
                if(startIdx + amount - 1 >= arr->size) {
                    size_t delta = startIdx + amount - 1 - arr->size;
                    if(delta > lastIdx) {
                        throw std::range_error("Out of bounds.");
                    } else {
                        buffer = new float[amount];
                        std::copy(arr->values + startIdx, arr->values + arr->size, buffer);
                        std::copy(arr->values, arr->values + delta + 1, buffer + amount - 1 - delta);
                        needToDelete = true;
                    }
                } else {
                    buffer = arr->values + startIdx;
                    needToDelete = false;
                }
            }
            
            return needToDelete;
        }*/
        
        /*void skipNext() {
            checkEnd();
            increment();
        }*/
        
    private:
        void checkEnd() {
            if(!hasNext()) {
                throw std::range_error("Iterator reaches the end.");
            }
        }
        
        void increment() {
            if(currentIdx + 1 >= arr->size) {
                currentIdx = 0;
            } else {
                ++currentIdx;
            }
        }
        
        const CircularArray* arr;
        size_t lastIdx; // index of last value
        size_t startIdx;// index of first value
        size_t currentIdx;// current read value index
    };
    
    CircularArray(size_t size, size_t cursor = 0) {
        setSize(size, cursor);
    };
    
    Iterator getIterator(size_t amount) {
        return Iterator(this, amount);
    }
    
    void put(const float* values, unsigned long amount) {
        if(cursorIdx + amount > size) {
            size_t delta = size - cursorIdx;
            std::copy(values, values + delta, this->values + cursorIdx);
            std::copy(values + (amount - delta) + 1, values + amount, this->values);
            cursorIdx = amount - delta;
        } else {
            std::copy(values, values + amount, this->values + cursorIdx);
            cursorIdx += amount;
        }
    }
    
    juce::String toString() {
        juce::String str = "";
        for(size_t i = 0; i < cursorIdx; ++i) {
            str.append(juce::String(values[i]) + " ", 10);
        }
        str.append("| ", 4);
        for(size_t i = cursorIdx + 1; i < size; ++i) {
            str.append(juce::String(values[i]) + " ", 10);
        }
        
        return str;
    }
    
    size_t getCursor() const {
        return cursorIdx;
    }
    
private:
    void setSize(size_t newSize, size_t cursor) {
        if(newSize == 0) { // can be 1?
            throw std::invalid_argument("Size can not be 0.");
        }
        
        newSize += 1;
        
        if(size == newSize) {
            return;
        }
        
        if(cursor > newSize) {
            throw std::invalid_argument("Cursor can not be greater than size.");
        }
        
        if(size == 0) {
            values = new float[newSize + 1];
        } else {
            if(size < newSize) {
                float* biggerArray = new float[newSize];
                std::copy(values, values + size, biggerArray);
                delete values;
                values = biggerArray;
            } else {
                float* smallerArray = new float[newSize];
                std::copy(values, values + newSize, smallerArray);
                delete values;
                values = smallerArray;
            }
        }
        
        cursorIdx = cursor;
        size = newSize;
    }
    
    float* values = nullptr;
    size_t size = 0;
    size_t cursorIdx = 0;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CircularArray)
};
