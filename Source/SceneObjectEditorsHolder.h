#pragma once

#include <JuceHeader.h>
#include "WaveformSceneObjectEditor.h"
#include "BackgroundSceneObjectEditor.h"
#include "ArrayUtil.h"

using namespace juce;

class SceneObjectEditorsHolder : public Component, public DragAndDropContainer, public DragAndDropTarget, public SceneObjectEditor::Listener, public ComboBox::Listener
{
public:
    SceneObjectEditorsHolder(Viewport& vp);
    
    void resized() override {
        //setBounds({300 - viewport->getScrollBarThickness(), setDefaultBounds() + 30});
        setBounds({300 - 10, setDefaultBounds() + 30});
        //setBounds({500, 500});
    }
    
    void addEditor(SceneObject* object);
    void reinitControls();
    void clear() { editors.clear(); }
    //void addObjectAppendListener(ComboBox::Listener* listener) { addObjectBox.addListener(listener); }
    
    bool isInterestedInDragSource (const SourceDetails &dragSourceDetails) override { return true; }
    void mouseDrag(const MouseEvent& e) override;
    void itemDropped(const SourceDetails &dragSourceDetails) override;
    void itemDragMove(const SourceDetails &dragSourceDetails) override;
    void itemDragExit(const SourceDetails &dragSourceDetails) override;
    void itemDragEnter(const SourceDetails &dragSourceDetails) override { underline.selectInitiator(true); }
    
    void deleteButtonClicked(SceneObjectEditor* editor) override;
    
    void comboBoxChanged(ComboBox*) override {
        SceneObjectRealisation r = SceneObjectRealisationHelper::fromInt(addObjectBox.getSelectedId() - 1);
        addEditor(listener->addObject(r));
        addObjectBox.setSelectedId(0, NotificationType::dontSendNotification);
    }
    
    class Listener
    {
    public:
        virtual ~Listener() = default;
        
        virtual void deleteObject(SceneObject* obj) {}
        virtual SceneObject* addObject(SceneObjectRealisation r) { return nullptr; }
        virtual void objectsReorder(int oldIdx, int newIdx) {}
    };
    
    void setListener(Listener* listener) { this->listener = listener; }
    
private:
    class DragAndDropUnderline : public Component
    {
    public:
        void paint(Graphics& g) override;
        
        void setUnderlined(SceneObjectEditor* component) { underlined = component; }
        bool hasMember() { return underlined != nullptr; }
        bool isUnderlined(SceneObjectEditor* component) const { return component == underlined; }
        SceneObjectEditor* getUnderlined() { return underlined; }
        
        void setInitiatorUpperObject(SceneObjectEditor* component) { initiatorUpper = component; }
        bool isInitiatorUpperObject(SceneObjectEditor* component) { return initiatorUpper == component; }
        
        void setInitiator(SceneObjectEditor* component);
        bool isInitiator(SceneObjectEditor* component) { return initiator == component; }
        void selectInitiator(bool shouldBeSelected) { if(initiator != nullptr) initiator->select(shouldBeSelected); }
        
        int getHeight() const { return 20; }
    private:
        SceneObjectEditor* underlined = nullptr;
        SceneObjectEditor* initiatorUpper = nullptr;
        SceneObjectEditor* initiator = nullptr;
    };
    
    SceneObjectEditor* findEditorAt(Point<int> p);
    void underlineReposition();
    int setDefaultBounds();
    
    OwnedArray<SceneObjectEditor> editors;
    DragAndDropUnderline underline;
    ComboBox addObjectBox;
    Viewport& viewport;
    Listener* listener;
};

class SceneObjectEditorsHolderViewport : public Viewport
{
public:
    SceneObjectEditorsHolderViewport() {
        //setViewedComponent(component);
    }
    
    void componentMovedOrResized(Component& c, bool wasMoved, bool wasResized) override {
        if(wasResized) {
            auto pos = getViewPosition();
            Viewport::componentMovedOrResized(c, wasMoved, wasResized);
            setViewPosition(pos);
        } else {
            Viewport::componentMovedOrResized(c, wasMoved, wasResized);
        }
    }
};
