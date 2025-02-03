#include "SceneObjectEditorsHolder.h"

SceneObjectEditorsHolder::SceneObjectEditorsHolder(Viewport& vp) : viewport(vp) {
    addObjectBox.setTextWhenNothingSelected("+");
    addObjectBox.addListener(this);
    
    for(auto r : SceneObjectRealisationHelper::getAll()) {
        addObjectBox.addItem(SceneObjectRealisationHelper::toString(r), r + 1);
    }
    
    addAndMakeVisible(underline);
    addAndMakeVisible(addObjectBox);
}

void SceneObjectEditorsHolder::addEditor(SceneObject* object) {
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
    editor->setListener(this);
    resized();
}

void SceneObjectEditorsHolder::reinitControls() {
    for(int i = 0; i < editors.size(); ++i) {
        editors[i]->initControls();
    }
}

SceneObjectEditor* SceneObjectEditorsHolder::findEditorAt(Point<int> p) {
    for(int i = 0; i < editors.size(); ++i) {
        if(editors[i]->getBoundsInParent().contains(p)) {
            return editors[i];
        }
    }
    
    return nullptr;
}

void SceneObjectEditorsHolder::underlineReposition() {
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

int SceneObjectEditorsHolder::setDefaultBounds() {
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

void SceneObjectEditorsHolder::mouseDrag(const MouseEvent& e) {
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

void SceneObjectEditorsHolder::itemDropped (const SourceDetails &dragSourceDetails) {
    auto draggedEditor = static_cast<SceneObjectEditor*>(dragSourceDetails.sourceComponent.get()) ;
    auto targetEditor = underline.getUnderlined();
    
    if(targetEditor == nullptr) {
        underline.selectInitiator(false);
        return;
    }
    
    int draggedIdx = editors.indexOf(draggedEditor);
    int targetIdx = editors.indexOf(targetEditor);
    
    if(targetIdx == draggedIdx - 1) {
        underline.selectInitiator(false);
        return;
    }
    
    if(reorder(editors, draggedIdx, targetIdx)) {
        listener->objectsReorder(draggedIdx, targetIdx);
    }
    
    underline.setUnderlined(nullptr);
    underline.selectInitiator(false);
    underline.setInitiator(nullptr);
    setDefaultBounds();
}

void SceneObjectEditorsHolder::itemDragMove (const SourceDetails &dragSourceDetails) {
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
    
    viewport.autoScroll(pos.getX(), pos.getY() - viewport.getViewPositionY(), 60, 3);
}

void SceneObjectEditorsHolder::itemDragExit (const SourceDetails &dragSourceDetails) {
    underline.setUnderlined(nullptr);
    underline.selectInitiator(false);
    underlineReposition();
}

void SceneObjectEditorsHolder::deleteButtonClicked(SceneObjectEditor* editor) {
    SceneObject* obj = editor->getObject();
    editors.removeObject(editor);
    resized();
    listener->deleteObject(obj);
}
//---------------------------------------------------------
void SceneObjectEditorsHolder::DragAndDropUnderline::paint(Graphics& g) {
    if(hasMember()) {
        g.setColour(Colours::blue);
    } else {
        g.setColour(Colours::transparentBlack);
    }
    g.fillAll();
}

void SceneObjectEditorsHolder::DragAndDropUnderline::setInitiator(SceneObjectEditor* component) {
    /*if(initiator != nullptr) {
        initiator->select(false);
    }*/
    
    initiator = component;
    
    /*if(initiator != nullptr) {
        initiator->select(true);
    }*/
}
//---------------------------------------------------------

