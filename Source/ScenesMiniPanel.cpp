#include "ScenesMiniPanel.h"

ScenesMiniPanel::ScenesMiniPanel(StatefulObject& parent, ScenesRender& render, SceneComponent::Listener* componentListener) : StatefulObject(parent, IDs::panelName), scenesRender(render), componentListener(componentListener)
{
    selfbounds = Rectangle<int>(0, 0, 100, panelHeight);
    setBounds(selfbounds);
    
    if(hasChildren()) { // expect that context initialized (parent component is visible)
        auto statesArray = getChildrenStates();
        for(auto state : statesArray) {
            SceneComponent* sc = createSceneInternal(&state, false);
            
            if(!sc->isDetached()) {
                sc->getScene().replaceContext(render.getContext(), false);
                render.addScene(&sc->getScene());
            }
            
            if(sc->getProperty(IDs::isChoosenScene)) {
                choosenComponent = sc;
            }
            
        }
        calcLocalBounds();
    }
}

void ScenesMiniPanel::resized() {
    auto bounds = selfbounds;
    for(int i = 0; i < wrappers.size(); ++i) {
        auto cellBounds = bounds.removeFromLeft(wrapperWidth);
        wrappers[i]->setBounds(cellBounds);
        
        if(needToPositionize(sceneComponents[i])) {
            sceneComponents[i]->setBounds(wrappers[i]->getBodyBounds().withX(cellBounds.getX()));
            sceneComponents[i]->getScene().changeBounds(cellBounds.getX() + scenesDrawingXOffset, scenesDrawingYOffset);
        }
    }
}

void ScenesMiniPanel::moved() {
    for(int i = 0; i < sceneComponents.size(); ++i) {
        if(needToPositionize(sceneComponents[i])) {
            sceneComponents[i]->getScene().changeBounds(i * wrapperWidth + scenesDrawingXOffset, scenesDrawingYOffset);
        }
    }
}

void ScenesMiniPanel::returnOnPannel(SceneComponent* sc) {
    if(choosenComponent == sc) {
        setChoosen(nullptr);
    }
    addAndMakeVisible(sc);
    resized();
}

void ScenesMiniPanel::sceneDeleting(SceneComponent& sceneComponent) {
    SceneComponentWrapper* wrap = nullptr;
    for(SceneComponentWrapper* w : wrappers) {
        if(w->isAssociatedWith(&sceneComponent)) {
            wrap = w;
            break;
        }
    }
    wrappers.removeObject(wrap);
    
    if(choosenComponent == &sceneComponent) {
        choosenComponent = nullptr;
    }
    sceneComponents.removeObject(&sceneComponent);
    calcLocalBounds();
}

void ScenesMiniPanel::sceneDetached(SceneComponent& component, bool isDetached) {
    if(isDetached) {
        setChoosen(nullptr);
    } else {
        if(choosenComponent == nullptr) {
            setChoosen(&component);
        } else {
            addAndMakeVisible(component);
            resized();
        }
    }
}

void ScenesMiniPanel::showOrHide(bool mustBeShown) {
    showing = mustBeShown;
    for(SceneComponent* sc : sceneComponents) {
        if(sc != choosenComponent) {
            sc->getScene().setNeedToRender(mustBeShown);
        }
    }
}

void ScenesMiniPanel::setListener(ScenesMiniPanel::Listener* l) {
    panelListener = l;
    for(SceneComponentWrapper* w : wrappers) {
        w->setListener(l);
    }
}

SceneComponent* ScenesMiniPanel::createSceneInternal(StatefulObject::ObjectState* objectState, bool calcBounds) {
    SceneComponent* sc = nullptr;
    if(objectState == nullptr) {
        sc = new SceneComponent(scenesRender.getContext(), *this);
        scenesRender.addScene(&sc->getScene());
    } else {
        sc = new SceneComponent(*this, *objectState);
    }
    
    sceneComponents.add(sc);
    sc->addSceneListener(componentListener);
    sc->addSceneListener(this);
    sc->setDeleter(this);
    
    SceneComponentWrapper* wrapper = new SceneComponentWrapper(sc);
    //sc->getScene().addListener(wrapper);
    wrapper->setListener(panelListener);
    wrappers.add(wrapper);
    
    addAndMakeVisible(wrapper);
    addAndMakeVisible(sc);
    if(calcBounds) calcLocalBounds();
    
    return sc;
}

void ScenesMiniPanel::calcLocalBounds() {
    selfbounds.setWidth(wrappers.size() * wrapperWidth);
    setBounds(selfbounds);
}

void ScenesMiniPanel::setChoosen(SceneComponent* sc) {
    if(sc != nullptr) sc->setProperty(IDs::isChoosenScene, true);
    if(choosenComponent != nullptr) choosenComponent->setProperty(IDs::isChoosenScene, false);
    choosenComponent = sc;
}

ScenesMiniPanel::SceneComponentWrapper::SceneComponentWrapper(SceneComponent* associatedComponent) : associatedComponent(associatedComponent) {
    sceneName.setText(associatedComponent->getScene().getName(), NotificationType::dontSendNotification);
    associatedComponent->getScene().addListener(this);
    addAndMakeVisible(sceneName);
    
    deleteButton.onClick = [this]() { this->associatedComponent->deleteScene(); };
    addAndMakeVisible(deleteButton);
    
    returnButton.setButtonText("return");
    returnButton.onClick = [this]() { listener->returnButtonClicked(this->associatedComponent); };
    addAndMakeVisible(returnButton);
}

void ScenesMiniPanel::SceneComponentWrapper::resized() {
    auto bounds = getLocalBounds();
    auto hb = bounds.removeFromTop(getWidth() * 0.12f);
    deleteButton.setBounds(hb.removeFromRight(getHeight() * 0.2f));
    sceneName.setBounds(hb);
    returnButton.setBounds(bounds);
    bodyBounds = bounds;
}
