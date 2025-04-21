#pragma once
#include <JuceHeader.h>
#include "SceneEditor.h"
#include "ScenesMiniPanel.h"

using namespace juce;

class SceneManagerComponent : public StatefulObject, public Component, public Timer, public SceneComponent::Listener, public ScrollBar::Listener, public ScenesMiniPanel::Listener
{
public:
    SceneManagerComponent(StatefulObject& parent);
    ~SceneManagerComponent() {}
private:
    struct IDs {
        inline static const String name{"Scene Manager"};
        inline static const Identifier isEditorVisible{"isEditorVisible"};
        inline static const Identifier isPanelVisible{"isPanelVisible"};
    };
    
    void timerCallback() override;
    void resized() override;
    
    void handleEditorVisibility(bool mustBeVisible);
    void handleScenesPanelVisibility(bool mustBeVisible);
    void returnSceneOnPanel(SceneComponent* sc);
    void chooseScene(SceneComponent* sc);
    void returnButtonClicked(SceneComponent* sc) override {returnSceneOnPanel(sc);}
    
    void sceneMouseClicked(SceneComponent& sc) override;
    void sceneDeleting(SceneComponent& sceneComponent) override;
    void sceneDetached(SceneComponent& component, bool isDetached) override;

    void scrollBarMoved (ScrollBar *scrollBarThatHasMoved, double newRangeStart) override;
    
    class MySidePanel : public Component
    {
    public:
        MySidePanel(SceneManagerComponent& parent);
        
        void resized() override;
        void paint(Graphics& g) override {g.fillAll(Colours::darkolivegreen);}
        
        void setExpandMiniPanelState(bool shouldBeToggled);
        void setExpandEditorState(bool shouldBeToggled);
    private:
        SceneManagerComponent& parent;
        TextButton expandEditorButton;
        TextButton expandMiniPannelButton;
        TextButton addSceneButton;
    };
    
    MySidePanel sidePannel;
    ScenesRender scenesRender;
    
    Viewport scenesPanelViewport;
    std::unique_ptr<ScenesMiniPanel> scenesPanel;
    SceneEditor sceneEditor;
    SceneComponent* choosenScene = nullptr;
    
    Rectangle<int> scenesBound;
    
    static const int sidePannelWidth = 30;
};
