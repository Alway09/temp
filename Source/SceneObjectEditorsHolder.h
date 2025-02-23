#pragma once

#include <JuceHeader.h>
#include "WaveformSceneObjectEditor.h"
#include "BackgroundSceneObjectEditor.h"
#include "SceneObjectRealisationHelper.h"
#include "ArrayUtil.h"

using namespace juce;

class SceneObjectEditorsHolder : public Component, public DragAndDropContainer, public DragAndDropTarget, public SceneObjectEditor::Listener, public ComboBox::Listener
{
public:
    SceneObjectEditorsHolder(Viewport& vp);
    
    void addEditor(SceneObject* object, bool execResize = true);
    void reinitControls();
    void clear() { editors.clear(); }
    
    void resized() override {setSize(viewport.getWidth()-viewport.getScrollBarThickness(), setDefaultBounds());}

    class Listener
    {
    public:
        virtual ~Listener() = default;
        virtual void deleteObject(SceneObject* obj) = 0;
        virtual SceneObject* addObject(SceneObjectRealisation r) = 0;
        virtual void objectsReorder(int oldIdx, int newIdx) = 0;
    };
    void setListener(Listener* listener) { this->listener = listener; }
private:
    void comboBoxChanged(ComboBox*) override {
        SceneObjectRealisation r = SceneObjectRealisationHelper::fromInt(addObjectBox.getSelectedId() - 1);
        addEditor(listener->addObject(r));
        addObjectBox.setSelectedId(0, NotificationType::dontSendNotification);
    }
    
    void deleteButtonClicked(SceneObjectEditor* editor, SceneObject* object) override;
    
    bool isInterestedInDragSource (const SourceDetails &dragSourceDetails) override { return true; }
    void mouseDrag(const MouseEvent& e) override;
    void itemDropped(const SourceDetails &dragSourceDetails) override;
    void itemDragMove(const SourceDetails &dragSourceDetails) override;
    void itemDragExit(const SourceDetails &dragSourceDetails) override { underline.dragExit(); underlineReposition(); }
    void itemDragEnter(const SourceDetails &dragSourceDetails) override { underline.selectInitiator(true); }
    
    class DragAndDropUnderline : public Component
    {
    public:
        void paint(Graphics& g) override;
        
        void endDrag() { dragExit(); initiator = nullptr; }
        void dragExit() { underlined = nullptr; selectInitiator(false); }
        void beginDrag(SceneObjectEditor* init, SceneObjectEditor* initUp) {
            initiator = init;
            initiatorUpper = initUp;
            setUnderlined(nullptr);
        }
        
        bool hasMember() const { return underlined != nullptr; }
        SceneObjectEditor* const getUnderlined() { return underlined; }
        void setUnderlined(SceneObjectEditor* component) { underlined = component; }
        bool isUnderlined(SceneObjectEditor* component) const { return component == underlined; }
        
        void selectInitiator(bool shouldBeSelected) { if(initiator != nullptr) initiator->select(shouldBeSelected); }
        bool isInitiator(SceneObjectEditor* component) const { return initiator == component; }
        bool isInitiatorUpperObject(SceneObjectEditor* component) const { return initiatorUpper == component; }
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
    
    const int underlineHeight = 20;
    const int addObjectBoxHeight = 30;
};

class SceneObjectEditorsHolderViewport : public Viewport
{
public:
    SceneObjectEditorsHolderViewport() {
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
