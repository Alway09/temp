#pragma once

#include <JuceHeader.h>
#include "SceneObject.h"

using namespace juce;

class SceneObjectEditor : public Component, public Label::Listener, public Button::Listener
{
public:
    SceneObjectEditor(SceneObject& objectEditTo);
    
    Rectangle<int> getBoundingRectangle(int Y) const { return Rectangle<int>{getParentWidth(), getHeight()}.withY(Y); }
    int getHeight() const { return header.getHeight() + (expanded ? controls.size() * controlHeight : 0); }
    void select(bool shouldBeSelected) { header.select(shouldBeSelected); }
    const OwnedArray<PropertyComponent>& getControls() { return controls; };
    virtual void initControls() = 0;
    
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
    
    class Listener {
    public:
        virtual ~Listener() = default;
        virtual void deleteButtonClicked(SceneObjectEditor* editor) = 0;
    };
    
    void setListener(SceneObjectEditor::Listener* listener) { this->listener = listener; }
    
    SceneObject* getObject() { return &object; }
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
    
    void buttonClicked(Button* b) override {
        if(header.isExpandButton(b)) {
            expanded = !expanded;
            
            header.changeState(expanded);
            
            if(!controls.isEmpty()) {
                for(auto c : controls) {
                    c->setVisible(expanded);
                }
                
                getParentComponent()->resized();
            }
        } else if(header.isDeleteButton(b)) {
            listener->deleteButtonClicked(this);
        }
    }
    
    class Header : public Component
    {
    public:
        Header() : expandButton("e", Colours::orange, Colours::orange, Colours::orange), selected(false) {
            sceneObjectNameLabel.setEditable(false, true, true);
            
            expandButton.shouldUseOnColours(false);
            
            nonExpandedPath.startNewSubPath(-1, 1);
            nonExpandedPath.lineTo(1, 0);
            nonExpandedPath.lineTo(-1, -1);
            setExpandButtonPath(nonExpandedPath);
            
            expandedPath.startNewSubPath(-1, -1);
            expandedPath.lineTo(0, 1);
            expandedPath.lineTo(1, -1);
            
            expandButton.setBorderSize(BorderSize<int>(3));
            
            addAndMakeVisible(sceneObjectNameLabel);
            addAndMakeVisible(expandButton);
            addAndMakeVisible(deleteButton);
        }
        
        void resized() override {
            auto localBounds = getLocalBounds().withHeight(height);
            
            expandButton.setBounds(localBounds.removeFromLeft(30));
            deleteButton.setBounds(localBounds.removeFromRight(30));
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
        void addDeleteButtonListener(Button::Listener* listener) { deleteButton.addListener(listener); }
        void setObjectName(String name) { sceneObjectNameLabel.setText(name, NotificationType::dontSendNotification); }
        
        void mouseDrag(const MouseEvent& e) override {
            Component* parent = getParentComponent();
            parent->mouseDrag(e.getEventRelativeTo(parent));
        }
        
        void select(bool shouldBeSelected) {
            selected = shouldBeSelected;
        }
        
        void changeState(bool expanded) {
            setExpandButtonPath(expanded ? expandedPath : nonExpandedPath);
        }
        
        bool isExpandButton(Button* b) { return &expandButton == b; }
        bool isDeleteButton(Button* b) { return &deleteButton == b; }
    private:
        class CustomLabel : public Label {
        public:
            void mouseDrag(const MouseEvent& e) {
                Component* parent = getParentComponent();
                parent->mouseDrag(e.getEventRelativeTo(parent));
            }
        };
        
        void setExpandButtonPath(Path& path) {
            expandButton.setShape(path, false, true, false);
        }
    
        ShapeButton expandButton;
        Path expandedPath;
        Path nonExpandedPath;
        TextButton deleteButton{"X"};
        CustomLabel sceneObjectNameLabel;
        bool selected;
        int height = 20;
    };
    
    Header header;
    int controlHeight = 40;
    bool expanded = false;
    SceneObjectEditor::Listener* listener;
};
