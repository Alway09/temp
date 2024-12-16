#pragma once

#include <JuceHeader.h>
#include "Scene.h"

using namespace juce;

class SceneComponent : public ResizableWindow, public Button::Listener
{
public:
    SceneComponent(Scene* scene);
    ~SceneComponent() { shutdown(); }
    
    void resized() override;
    void moved() override;
    void paint(Graphics&) override {};
    
    void mouseDown(const MouseEvent& e) override;
    void mouseUp(const MouseEvent& e) override;
    
    void buttonClicked(Button* button) override;
    
    void shutdown() { scene->shutdown(); }
    void render() { scene->render(); }
    Scene* getScene() { return scene; }
    
    class Listener {
    public:
        virtual ~Listener(){}
        virtual void sceneMouseDown(Scene* scene){};
        virtual void sceneMouseUp(Scene* scene){};
        virtual void sceneMouseClicked(Scene* scene){};
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
        
        void mouseDoubleClick(const MouseEvent& e) override { parent->mouseDoubleClick(e); }
        void mouseEnter(const MouseEvent& e) override { parent->mouseEnter(e); }
        void mouseExit(const MouseEvent& e) override { parent->mouseExit(e); }
        void mouseDrag(const MouseEvent& e) override { parent->mouseDrag(e); }
        void mouseMove(const MouseEvent& e) override { parent->mouseMove(e); }
        void mouseDown(const MouseEvent& e) override { parent->mouseDown(e); }
        void mouseUp(const MouseEvent& e) override { parent->mouseUp(e); }
        
    private:
        SceneComponent* parent;
        TextButton deleteButton{"d"};
    };
    
    Scene* scene;
    
    Rectangle<int> boundsInParent;
    int parentHeight;
    
    Array<Listener*> listeners;
    Listener* deleteListener;
};
