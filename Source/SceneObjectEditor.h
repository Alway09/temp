#pragma once

#include <JuceHeader.h>
#include "SceneObject.h"

using namespace juce;

class SceneObjectEditor : public Component, public Label::Listener, public Button::Listener
{
public:
    SceneObjectEditor(SceneObject& objectEditTo) : object(objectEditTo) {
        header.setObjectName(objectEditTo.getName());
        header.addsceneObjectNameLabelListener(this);
        header.addExpandButtonListener(this);
        addAndMakeVisible(header);
    }
    
    ~SceneObjectEditor() {
        //controls.clear();
    }
    
    void resized() override {
        auto bounds = getBoundingRectangle(0);
        
        header.setBounds(bounds.removeFromTop(header.getHeight()));
        for(auto c : controls) {
            c->setBounds(bounds.removeFromTop(controlHeight));
        }
    }
    
    void mouseDrag(const MouseEvent& e) override {
        Component* parent = getParentComponent();
        parent->mouseDrag(e.getEventRelativeTo(parent));
    }
    
    Rectangle<int> getBoundingRectangle(int Y) const { return Rectangle<int>{getParentWidth(), getHeight()}.withY(Y); }
    
    int getHeight() const {
        int result = header.getHeight();
        if(expanded) {
            result += controls.size() * controlHeight;
        }
        return result;
    }
    
    const OwnedArray<PropertyComponent>& getControls() { return controls; };
    virtual void initControls() = 0;
    
    void select(bool shouldBeSelected) { header.select(shouldBeSelected); }
protected:
    SceneObject& object;
    OwnedArray<PropertyComponent> controls;
private:
    void labelTextChanged(Label* l) override {
        String newName = l->getText();
        
        try {
            object.rename(newName);
        } catch (const NamedObject::NameException& e) {
            if(e.hasMessage()) {
                AlertWindow::showAsync(MessageBoxOptions::makeOptionsOk(MessageBoxIconType::WarningIcon, "Warning", e.getMessage()), nullptr);
            }
        } catch (const StatefulObject::StateException& e) {
            AlertWindow::showAsync(MessageBoxOptions::makeOptionsOk(MessageBoxIconType::WarningIcon, "Warning", e.getMessage()), nullptr);
        }
        
        header.setObjectName(object.getName());
        initControls();
    }
    
    void buttonClicked(Button*) override {
        expanded = !expanded;
        
        if(!controls.isEmpty()) {
            for(auto c : controls) {
                c->setVisible(expanded);
            }
            
            getParentComponent()->resized();
        }
    }
    
    class Header : public Component
    {
    public:
        Header() : expandButton("e", 0.0, Colours::orange), selected(false) {
            sceneObjectNameLabel.setEditable(false, true, true);
            
            //sceneObjectNameLabel.setInterceptsMouseClicks(false, false);
            addAndMakeVisible(sceneObjectNameLabel);
            addAndMakeVisible(expandButton);
        }
        
        void resized() override {
            auto localBounds = getLocalBounds().withHeight(height);
            
            expandButton.setBounds(localBounds.removeFromLeft(50));
            sceneObjectNameLabel.setBounds(localBounds);
        }
        
        void paint(Graphics& g) override {
            if(selected) {
                g.fillAll(Colours::red);
            }
        }
        
        int getHeight() const { return height; }
        
        void addsceneObjectNameLabelListener(Label::Listener* listener) { sceneObjectNameLabel.addListener(listener); }
        void addExpandButtonListener(Button::Listener* listener) { expandButton.addListener(listener); }
        void setObjectName(String name) { sceneObjectNameLabel.setText(name, NotificationType::dontSendNotification); }
        
        void mouseDrag(const MouseEvent& e) override {
            Component* parent = getParentComponent();
            parent->mouseDrag(e.getEventRelativeTo(parent));
        }
        
        void select(bool shouldBeSelected) { selected = shouldBeSelected; }
    private:
        class CustomLabel : public Label {
        public:
            void mouseDrag(const MouseEvent& e) {
                Component* parent = getParentComponent();
                parent->mouseDrag(e.getEventRelativeTo(parent));
            }
        };
    
        ArrowButton expandButton;
        CustomLabel sceneObjectNameLabel;
        bool selected;
        int height = 20;
    };
    
    Header header;
    int controlHeight = 40;
    bool expanded = false;
};
