#pragma once

#include <JuceHeader.h>
#include "Scene.h"
#include "ScenesRender.h"

using namespace juce;

class SceneComponent : public ResizableWindow
{
public:
    SceneComponent(Scene* scene);
    ~SceneComponent() { shutdown(); }
    
    void resized() override;
    void moved() override;
    void paint(Graphics&) override {};
    
    void mouseDown(const MouseEvent& e) override;
    void mouseUp(const MouseEvent& e) override;
    
    void deleteButtonClicked();
    void detachButtonClicked(bool detach);
    
    void shutdown() { scene->shutdown(); }
    void render() { scene->render(); }
    Scene* getScene() { return scene; }
    
    class Listener {
    public:
        virtual ~Listener(){}
        virtual void sceneMouseDown(Scene* scene){};
        virtual void sceneMouseUp(Scene* scene){};
        virtual void sceneMouseClicked(Scene* scene){};
        virtual void sceneDetachButtonClicked(SceneComponent* component, bool detach){};
        virtual void sceneDeleteButtonClicked(SceneComponent* sceneComponent){};
    };
    
    void addSceneListener(Listener* l) { listeners.add(l); }
    void setDeleterListener(Listener* l) { deleteListener = l; }
    
private:
    class SceneOverlayComponent : public Component
    {
    public:
        SceneOverlayComponent(SceneComponent* parent);
        
        void resized() override;
        
        void mouseEnter(const MouseEvent& e) override;
        void mouseExit(const MouseEvent& e) override;
        void mouseDoubleClick(const MouseEvent& e) override { parent->mouseDoubleClick(e); }
        void mouseDrag(const MouseEvent& e) override { parent->mouseDrag(e); }
        void mouseMove(const MouseEvent& e) override { parent->mouseMove(e); }
        void mouseDown(const MouseEvent& e) override { parent->mouseDown(e); }
        void mouseUp(const MouseEvent& e) override { parent->mouseUp(e); }
        
    private:
        Array<Component*> getAllComponents() { return {&deleteButton, &detachButton}; }
        
        SceneComponent* parent;
        TextButton deleteButton{"X"};
        TextButton detachButton{"d"};
    };
    
    Scene* scene;
    
    Rectangle<int> boundsInParent;
    int parentHeight;
    
    Array<Listener*> listeners;
    Listener* deleteListener;
    
    std::unique_ptr<ScenesRender> ownRender;
};
