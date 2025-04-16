#include "ScenesMiniPanel.h"

ScenesMiniPanel::ScenesMiniPanel(StatefulObject& parent, ScenesRender& render, SceneComponent::Listener* componentListener) : StatefulObject(parent, "Panel"), scenesRender(render), componentListener(componentListener)
{
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
