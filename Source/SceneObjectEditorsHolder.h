#pragma once

#include <JuceHeader.h>
#include "WaveformSceneObjectEditor.h"
#include "BackgroundSceneObjectEditor.h"

using namespace juce;

class SceneObjectEditorsHolder : public Viewport
{
public:
    SceneObjectEditorsHolder() {
        innerComponent = new InnerComponent(this);
        setViewedComponent(innerComponent);
    }
    
    void addEditor(SceneObject* object) { innerComponent -> addEditor(object); }
    void clear() { innerComponent->clear(); }
    void addObjectAppendListener(ComboBox::Listener* listener) { innerComponent->addObjectAppendListener(listener); }
    void reinitControls() { innerComponent->reinitControls(); }
    
    void componentMovedOrResized(Component& c, bool wasMoved, bool wasResized) override {
        Viewport::componentMovedOrResized(c, wasMoved, wasResized);
        if(wasResized) {
            setViewPositionProportionately(1.0, 1.0);
        }
    }
    
private:
    class InnerComponent : public Component, public DragAndDropContainer, public DragAndDropTarget
    {
    public:
        InnerComponent(Viewport* viewport) : viewport(viewport) {
            addObjectBox.setTextWhenNothingSelected("+");
            
            for(auto r : SceneObjectRealisationHelper::getAll()) {
                addObjectBox.addItem(SceneObjectRealisationHelper::toString(r), r + 1);
            }
            
            addAndMakeVisible(underline);
            addAndMakeVisible(addObjectBox);
        }
        ~InnerComponent(){}
        
        void resized() override {
            setBounds({300, setDefaultBounds() + 30});
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
                    auto initiator = findEditorAt(e.getMouseDownPosition());
                    underline.setInitiator(initiator);
                    int idx = editors.indexOf(initiator) - 1;
                    if(idx >= 0) {
                        underline.setInitiatorUpperObject(editors[idx]);
                    } else {
                        underline.setInitiatorUpperObject(nullptr);
                    }
                    underline.setUnderlined(nullptr);
                    startDragging("some", initiator);
                    beginDragAutoRepeat(100);
                }
            }
        }
        
        bool isInterestedInDragSource (const SourceDetails &dragSourceDetails) override { return true; }
        
        void itemDropped (const SourceDetails &dragSourceDetails) override {
            //DBG("drop");
            auto draggedEditor = static_cast<SceneObjectEditor*>(dragSourceDetails.sourceComponent.get()) ;
            auto targetEditor = underline.getUnderlined();
            
            if(targetEditor == nullptr) {
                return;
            }
            
            int draggedIdx = editors.indexOf(draggedEditor);
            int targetIdx = editors.indexOf(targetEditor);
            
            if(targetIdx == draggedIdx - 1) {
                return;
            }
            
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
            
            underline.setUnderlined(nullptr);
            underline.setInitiator(nullptr);
            setDefaultBounds();
        }
        
        void itemDragMove (const SourceDetails &dragSourceDetails) override {
            auto pos = dragSourceDetails.localPosition;
            auto c = findEditorAt(pos);
            
            if(underline.isInitiator(c) || underline.isInitiatorUpperObject(c)) {
                if(underline.hasMember()) {
                    underline.setUnderlined(nullptr);
                    underlineReposition();
                }
            } else {
                if(!underline.isUnderlined(c)) {
                    underline.setUnderlined(c);
                    underlineReposition();
                }
            }
            
            viewport->autoScroll(pos.getX(), pos.getY() - viewport->getViewPositionY(), 60, 3);
        }
        
        void itemDragEnter (const SourceDetails &dragSourceDetails) override {
        }
        
        void itemDragExit (const SourceDetails &dragSourceDetails) override {
            underline.setUnderlined(nullptr);
            underlineReposition();
        }
        
        void reinitControls() {
            for(int i = 0; i < editors.size(); ++i) {
                editors[i]->initControls();
            }
        }
        
        void addObjectAppendListener(ComboBox::Listener* listener) {
            addObjectBox.addListener(listener);
        }
        
    private:
        class DragAndDropUnderline : public Component
        {
        public:
            void paint(Graphics& g) override {
                if(hasMember()) {
                    g.setColour(Colours::blue);
                } else {
                    g.setColour(Colours::transparentBlack);
                }
                g.fillAll();
            }
            
            bool hasMember() { return underlined != nullptr; }
            bool isUnderlined(SceneObjectEditor* component) const { return component == underlined; }
            void setUnderlined(SceneObjectEditor* component) { underlined = component; }
            SceneObjectEditor* getUnderlined() { return underlined; }
            void setInitiatorUpperObject(SceneObjectEditor* component) { initPrev = component; }
            bool isInitiatorUpperObject(SceneObjectEditor* component) { return initPrev == component; }
            void setInitiator(SceneObjectEditor* component) {
                if(initiator != nullptr) {
                    initiator->select(false);
                }
                
                initiator = component;
                
                if(initiator != nullptr) {
                    initiator->select(true);
                }
            }
            bool isInitiator(SceneObjectEditor* component) { return initiator == component; }
            int getHeight() const { return 20; }
            
        private:
            SceneObjectEditor* underlined = nullptr;
            SceneObjectEditor* initPrev = nullptr;
            SceneObjectEditor* initiator = nullptr;
        };
        
        SceneObjectEditor* findEditorAt(Point<int> p) {
            for(int i = 0; i < editors.size(); ++i) {
                if(editors[i]->getBoundsInParent().contains(p)) {
                    return editors[i];
                }
            }
            
            return nullptr;
        }
        
        void underlineReposition() {
            if(underline.hasMember()) {
                int paddingY = underline.getUnderlined()->getBottom();
                underline.setBounds(underline.getBounds().withY(paddingY));
                
                int underlinedEditorIdx = editors.indexOf(underline.getUnderlined());
                
                paddingY = underline.getBounds().getBottom();
                int i;
                SceneObjectEditor* memPrev = nullptr;
                for(i = 1; i < editors.size() - underlinedEditorIdx; ++i) {
                    auto mem = editors[underlinedEditorIdx + i];
                    int prevHeight = memPrev == nullptr ? 0 : memPrev->getHeight();
                    paddingY += prevHeight;
                    mem->setBounds(mem->getBounds().withY(paddingY));
                    memPrev = mem;
                }
                addObjectBox.setBounds(addObjectBox.getBounds().withY(paddingY + (memPrev == nullptr ? 0 : memPrev->getHeight())));
            } else {
                setDefaultBounds();
            }
        }
        
        int setDefaultBounds() {
            int heightPadding = 0;
            for(auto editor : editors) {
                auto bounds = editor->getBoundingRectangle(heightPadding);
                editor->setBounds(bounds);
                
                heightPadding += bounds.getHeight();
            }
            addObjectBox.setBounds(0, heightPadding, getWidth(), 30);
            
            underline.setBounds(0, heightPadding + 30, getWidth(), underline.getHeight());
            heightPadding += underline.getHeight();
            return heightPadding;
        }
        
        OwnedArray<SceneObjectEditor> editors;
        DragAndDropUnderline underline;
        ComboBox addObjectBox;
        Viewport* viewport;
    };
    
    InnerComponent* innerComponent;

};
