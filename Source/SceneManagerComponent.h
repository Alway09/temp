#pragma once
#include <JuceHeader.h>
#include "CommandManagerHolder.h"
#include "SceneEditor.h"
#include "ScenesMiniPanel.h"

using namespace juce;

class SceneManagerComponent : public Component, public ApplicationCommandTarget, public SceneComponent::Listener, public ScrollBar::Listener, public ScenesMiniPanel::Listener
{
public:
    SceneManagerComponent(StatefulObject& parent);
    ~SceneManagerComponent() {}
private:
    enum Commands
    {
        addScene = 200
    };
    
    void resized() override;
    
    void handleEditorVisibility(bool mustBeVisible);
    void handleScenesPanelVisibility(bool mustBeVisible);
    void returnSceneOnPanel();
    void returnButtonClicked(SceneComponent* sc) override {
        returnSceneOnPanel();
    }
    
    void sceneMouseClicked(SceneComponent& sc) override;
    void sceneDeleting(SceneComponent& sceneComponent) override;
    void sceneDetached(SceneComponent& component, bool isDetached) override;
    
    ApplicationCommandTarget* getNextCommandTarget() override { return findFirstTargetParentComponent(); }
    void getAllCommands (Array<CommandID> &commands) override { commands.add(Commands::addScene); }
    void getCommandInfo(CommandID commandID, ApplicationCommandInfo &result) override;
    bool perform (const InvocationInfo &info) override;
    void scrollBarMoved (ScrollBar *scrollBarThatHasMoved, double newRangeStart) override;
    
    class MySidePanel : public Component
    {
    public:
        MySidePanel(SceneManagerComponent& parent) : parent(parent) {
            expandEditorButton.setToggleable(true);
            expandEditorButton.setClickingTogglesState(true);
            expandEditorButton.setToggleState(false, NotificationType::dontSendNotification);
            expandEditorButton.setButtonText("<");
            expandEditorButton.onClick = [this](){ this->parent.handleEditorVisibility(expandEditorButton.getToggleState());
                expandEditorButton.setButtonText(expandEditorButton.getToggleState() ? ">" : "<");
            };
            
            expandMiniPannelButton.setToggleable(true);
            expandMiniPannelButton.setClickingTogglesState(true);
            expandMiniPannelButton.setToggleState(false, NotificationType::dontSendNotification);
            expandMiniPannelButton.setButtonText("/\\");
            expandMiniPannelButton.onClick = [this](){ this->parent.handleScenesPanelVisibility(expandMiniPannelButton.getToggleState());
                expandMiniPannelButton.setButtonText(expandMiniPannelButton.getToggleState() ? "\\/" : "/\\");
                addSceneButton.setVisible(expandMiniPannelButton.getToggleState());
                resized();
            };
            
            addSceneButton.setButtonText("+");
            addSceneButton.onClick = [this]() { this->parent.scenesPanel.createScene(&(this->parent)); };
            addChildComponent(addSceneButton);

            addAndMakeVisible(expandEditorButton);
            addAndMakeVisible(expandMiniPannelButton);
        }
        
        void resized() override {
            auto bounds = getLocalBounds();
            expandEditorButton.setBounds(bounds.removeFromTop(30).withY(20));
            int h = parent.scenesPanel.isShowing() ? parent.scenesPanel.getNormalHeight() : 0;
            Rectangle<int> r(0, getHeight() - 30 - h, getWidth(), 30);
            //expandMiniPannelButton.setBounds(bounds.removeFromBottom(30).withY(getHeight() - 30 - parent.scenesPanel.getCurrentHeight()));
            expandMiniPannelButton.setBounds(r);
            addSceneButton.setBounds(bounds.removeFromBottom(h));
        }
        
        void paint(Graphics& g) override {
            g.fillAll(Colours::darkolivegreen);
        }
        
    private:
        SceneManagerComponent& parent;
        TextButton expandEditorButton;
        TextButton expandMiniPannelButton;
        TextButton addSceneButton;
    };
    
    MySidePanel sidePannel;
    //std::unique_ptr<ScenesRender> scenesRender;
    ScenesRender scenesRender;
    
    Viewport scenesPanelViewport;
    ScenesMiniPanel scenesPanel;
    //ScenesView scenesView;
    SceneEditor sceneEditor;
    SceneComponent* choosenScene = nullptr;
    
    Rectangle<int> scenesBound;
};
