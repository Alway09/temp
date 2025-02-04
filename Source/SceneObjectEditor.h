#pragma once

#include <JuceHeader.h>
#include "SceneObject.h"

using namespace juce;

class SceneObjectEditor : public Component, public Label::Listener, public Button::Listener
{
public:
    SceneObjectEditor(SceneObject& objectEditTo);
    
    virtual void initControls() = 0;
    
    Rectangle<int> getBoundingRectangle(int Y) const { return Rectangle<int>{getParentWidth(), getHeight()}.withY(Y); }
    int getHeight() const { return header.getHeight() + (expanded ? controls.size() * controlHeight : 0); }
    void select(bool shouldBeSelected) { header.select(shouldBeSelected); }
    const OwnedArray<PropertyComponent>& getControls() { return controls; };

    class Listener {
    public:
        virtual ~Listener() = default;
        virtual void deleteButtonClicked(SceneObjectEditor* editor, SceneObject* object) = 0;
    };
    void setListener(SceneObjectEditor::Listener* listener) { this->listener = listener; }
    
protected:
    void resized() override;
    SceneObject& object;
    OwnedArray<PropertyComponent> controls;
private:
    void mouseDrag(const MouseEvent& e) override;
    void labelTextChanged(Label* l) override;
    void buttonClicked(Button* b) override;
    
    class Header : public Component
    {
    public:
        Header();

        int getHeight() const { return height; }
        
        void addSceneObjectNameLabelListener(Label::Listener* listener) { sceneObjectNameLabel.addListener(listener); }
        void addExpandButtonListener(Button::Listener* listener) { expandButton.addListener(listener); }
        void addDeleteButtonListener(Button::Listener* listener) { deleteButton.addListener(listener); }
        void setObjectName(String name) { sceneObjectNameLabel.setText(name, NotificationType::dontSendNotification); }
        
        void mouseDrag(const MouseEvent& e) override;
        
        void select(bool shouldBeSelected) { selected = shouldBeSelected; }
        void changeState(bool expanded) { setExpandButtonPath(expanded ? expandedPath : nonExpandedPath); }
        bool isExpandButton(Button* b) { return &expandButton == b; }
        bool isDeleteButton(Button* b) { return &deleteButton == b; }
    private:
        void resized() override;
        void paint(Graphics& g) override { if(selected) g.fillAll(Colours::red); }
        void setExpandButtonPath(Path& path) { expandButton.setShape(path, false, true, false); }
        
        struct CustomLabel : public Label {
            void mouseDrag(const MouseEvent& e) override {
                Component* parent = getParentComponent();
                parent->mouseDrag(e.getEventRelativeTo(parent));
            }
        };
    
        ShapeButton expandButton{"e", Colours::orange, Colours::orange, Colours::orange};
        Path expandedPath;
        Path nonExpandedPath;
        TextButton deleteButton{"X"};
        CustomLabel sceneObjectNameLabel;
        bool selected = false;
        const int height = 20;
        const int expandButtonWidth = 30;
        const int deleteButtonWidth = 30;
    };
    
    Header header;
    const int controlHeight = 40;
    bool expanded = false;
    SceneObjectEditor::Listener* listener;
};
