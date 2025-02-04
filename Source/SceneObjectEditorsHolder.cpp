#include "SceneObjectEditorsHolder.h"

SceneObjectEditorsHolder::SceneObjectEditorsHolder(Viewport& vp) : viewport(vp) {
    addObjectBox.setTextWhenNothingSelected("Add object");
    addObjectBox.addListener(this);
    
    for(auto r : SceneObjectRealisationHelper::getAll()) {
        addObjectBox.addItem(SceneObjectRealisationHelper::toString(r), r + 1);
    }
    
    addAndMakeVisible(underline);
    addAndMakeVisible(addObjectBox);
}

void SceneObjectEditorsHolder::addEditor(SceneObject* object, bool execResize) {
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
    
    if(execResize) resized();
}

void SceneObjectEditorsHolder::reinitControls() {
    for(int i = 0; i < editors.size(); ++i) {
        editors[i]->initControls();
    }
}

SceneObjectEditor* SceneObjectEditorsHolder::findEditorAt(Point<int> p) {
    for(SceneObjectEditor* editor : editors) {
        if(editor->getBoundsInParent().contains(p)) {
            return editor;
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
        if(underlinedEditorIdx != editors.size() - 1) {
            for(int i = 1; i < editors.size() - underlinedEditorIdx; ++i) {
                int currentIdx = underlinedEditorIdx + i;
                auto currentEditor = editors[currentIdx];
                int upperHeight = i == 1 ? 0 : editors[currentIdx - 1]->getHeight();
                paddingY += upperHeight;
                currentEditor->setBounds(currentEditor->getBounds().withY(paddingY));
            }
            paddingY += editors.getLast()->getHeight();
        }
        addObjectBox.setBounds(addObjectBox.getBounds().withY(paddingY));
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
    addObjectBox.setBounds(0, heightPadding, getWidth(), addObjectBoxHeight);
    heightPadding += addObjectBoxHeight;
    
    underline.setBounds(0, heightPadding, getWidth(), underlineHeight);
    heightPadding += underlineHeight;
    return heightPadding;
}

void SceneObjectEditorsHolder::mouseDrag(const MouseEvent& e) {
    if(!isDragAndDropActive()) {
        auto editor = findEditorAt(e.getMouseDownPosition());
        if(editor != nullptr) {
            int upperIdx = editors.indexOf(editor) - 1;
            underline.beginDrag(editor, upperIdx >= 0 ? editors[upperIdx] : nullptr);
            startDragging("", editor);
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
    
    underline.endDrag();
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
//---------------------------------------------------------

