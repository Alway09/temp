#pragma once

#include <JuceHeader.h>
#include "WaveformSceneObjectEditor.h"
#include "BackgroundSceneObjectEditor.h"

using namespace juce;

class SceneObjectEditorsHolder : public Component, public DragAndDropContainer, public DragAndDropTarget
{
public:
    SceneObjectEditorsHolder(){ addChildComponent(underline); }
    ~SceneObjectEditorsHolder(){}
    
    void resized() override {
        int heightPadding = 0;
        for(auto editor : editors) {
            auto bounds = editor->getBoundingRectangle(heightPadding);
            editor->setBounds(bounds);
            
            heightPadding += bounds.getHeight();
            
            if(underline.isVisible() && underline.isUnderlined(editor)) {
                underline.setBounds(0, heightPadding, getWidth(), 20);
                heightPadding += 20;
            }
        }
    }
    
    void addEditor(SceneObject* object) {
        SceneObjectRealisation realisation = object->getRealisation();
        
        SceneObjectEditor* editor;
        switch (realisation) {
            case SceneObjectRealisation::Waveform:
                editor = new WaveformSceneObjectEditor(*object);
                break;
            case SceneObjectRealisation::Background:
                editor = new BackgroundSceneObjectEditor(*object);
                break;
        }
        
        
        addAndMakeVisible(editor);
        editors.add(editor);
        resized();
    }
    
    void clear() {
        editors.clear();
    }
    
    void mouseDrag(const MouseEvent& e) override {
        if(!isDragAndDropActive()) {
            auto editor = findEditorAt(e.getMouseDownPosition());
            if(editor != nullptr) {
                startDragging("some", findEditorAt(e.getMouseDownPosition()));
            }
        }
    }
    
    bool isInterestedInDragSource (const SourceDetails &dragSourceDetails) override { return true; }
    
    void itemDropped (const SourceDetails &dragSourceDetails) override {
        auto draggedEditor = static_cast<SceneObjectEditor*>(dragSourceDetails.sourceComponent.get()) ;
        auto targetEditor = underline.getUnderlied();
        
        int draggedIdx = editors.indexOf(draggedEditor);
        int targetIdx = editors.indexOf(targetEditor);
        
        if(draggedIdx != targetIdx) {
            if(targetIdx > draggedIdx) 
            {
                for(int i = draggedIdx; i < targetIdx; ++i) {
                    editors.set(i, editors[i+1], false);
                }
                editors.set(targetIdx, draggedEditor, false);
            } else 
            {
                for(int i = draggedIdx; i > targetIdx; --i) {
                    editors.set(i, editors[i-1], false);
                }
                editors.set(targetIdx + 1, draggedEditor, false);
            }
        }
        
        underline.setVisible(false);
        resized();
    }
    
    void itemDragMove (const SourceDetails &dragSourceDetails) override {
        auto c = findEditorAt(dragSourceDetails.localPosition);
        if(!underline.isUnderlined(c)) {
            underline.setUnderlined(c);
            resized();
        }
    }
    
    void itemDragEnter (const SourceDetails &dragSourceDetails) override {
        underline.setVisible(true);
        resized();
    }
    
    void itemDragExit (const SourceDetails &dragSourceDetails) override {
        underline.setVisible(false);
        resized();
    }
    
    void reinitControls() {
        for(int i = 0; i < editors.size(); ++i) {
            editors[i]->initControls();
        }
    }
    
private:
    class DragAndDropUnderline : public Component
    {
    public:
        void paint(Graphics& g) override {
            g.setColour(Colours::blue);
            g.fillRect(getLocalBounds());
        }
        
        bool isUnderlined(SceneObjectEditor* component) const { return component == underlined; }
        void setUnderlined(SceneObjectEditor* component) { underlined = component; }
        SceneObjectEditor* getUnderlied() { return underlined; }
        
    private:
        SceneObjectEditor* underlined;
    };
    
    SceneObjectEditor* findEditorAt(Point<int> p) {
        auto c = getComponentAt(p);
        if(c != this) {
            Component* comp = c->getParentComponent()->getParentComponent();
            if(comp == this) {
                return static_cast<SceneObjectEditor*>(c->getParentComponent()); // parameter
            }
            return static_cast<SceneObjectEditor*>(comp); // header
        }
        return nullptr;
    }
    
    OwnedArray<SceneObjectEditor> editors;
    DragAndDropUnderline underline;
};
