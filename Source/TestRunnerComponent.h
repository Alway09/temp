/*
  ==============================================================================

   This file is part of the JUCE framework examples.
   Copyright (c) Raw Material Software Limited

   The code included in this file is provided under the terms of the ISC license
   http://www.isc.org/downloads/software-support-policy/isc-license. Permission
   to use, copy, modify, and/or distribute this software for any purpose with or
   without fee is hereby granted provided that the above copyright notice and
   this permission notice appear in all copies.

   THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
   REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
   AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
   INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
   LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
   OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
   PERFORMANCE OF THIS SOFTWARE.

  ==============================================================================
*/

/*******************************************************************************
 The block below describes the properties of this PIP. A PIP is a short snippet
 of code that can be read by the Projucer and used to generate a JUCE project.

 BEGIN_JUCE_PIP_METADATA

 name:             TestRunnerComponent
 version:          1.0.0
 vendor:           JUCE
 website:          http://juce.com
 description:      Performs unit tests.

 dependencies:     juce_analytics, juce_audio_basics, juce_audio_devices,
                   juce_audio_formats, juce_audio_processors, juce_audio_utils,
                   juce_core, juce_cryptography, juce_data_structures, juce_dsp,
                   juce_events, juce_graphics, juce_gui_basics, juce_gui_extra,
                   juce_opengl, juce_osc, juce_product_unlocking, juce_video,
                   juce_midi_ci
 exporters:        xcode_mac, vs2022, linux_make, androidstudio, xcode_iphone

 moduleFlags:      JUCE_STRICT_REFCOUNTEDPOINTER=1,JUCE_PLUGINHOST_VST3=1,JUCE_PLUGINHOST_LV2=1
 defines:          JUCE_UNIT_TESTS=1

 type:             Component
 mainClass:        TestRunnerComponent

 useLocalCopy:     1

 END_JUCE_PIP_METADATA

*******************************************************************************/

#pragma once

#include <JuceHeader.h>
#include "SamplesHolderUnitTest.h"

using namespace juce;

//==============================================================================
class TestRunnerComponent final : public Component
{
public:
    TestRunnerComponent()
    {
        setOpaque (true);

        addAndMakeVisible (startTestButton);
        startTestButton.onClick = [this] { start(); };

        addAndMakeVisible (testResultsBox);
        testResultsBox.setMultiLine (true);
        testResultsBox.setFont (FontOptions (Font::getDefaultMonospacedFontName(), 12.0f, Font::plain));

        addAndMakeVisible (categoriesBox);
        categoriesBox.addItem ("All Tests", 1);

        auto categories = UnitTest::getAllCategories();
        categories.sort (true);

        categoriesBox.addItemList (categories, 2);
        categoriesBox.setSelectedId (1);

        logMessage ("This panel runs the built-in JUCE unit-tests from the selected category.\n");
        logMessage ("To add your own unit-tests, see the JUCE_UNIT_TESTS macro.");

        setSize (500, 500);
    }

    ~TestRunnerComponent() override
    {
        stopTest();
    }

    //==============================================================================
    void paint (Graphics& g) override
    {
        /*g.fillAll (LookAndFeel_V4::ColourScheme::UIColour::windowBackground,
                                           Colours::grey);*/
        g.fillAll(Colours::grey);
    }

    void resized() override
    {
        auto bounds = getLocalBounds().reduced (6);

        auto topSlice = bounds.removeFromTop (25);
        startTestButton.setBounds (topSlice.removeFromLeft (200));
        topSlice.removeFromLeft (10);
        categoriesBox  .setBounds (topSlice.removeFromLeft (250));

        bounds.removeFromTop (5);
        testResultsBox.setBounds (bounds);
    }

    void start()
    {
        startTest (categoriesBox.getText());
    }

    void startTest (const String& category)
    {
        testResultsBox.clear();
        startTestButton.setEnabled (false);

        currentTestThread.reset (new TestRunnerThread (*this, category));
        currentTestThread->startThread();
    }

    void stopTest()
    {
        if (currentTestThread.get() != nullptr)
        {
            currentTestThread->stopThread (15000);
            currentTestThread.reset();
        }
    }

    void logMessage (const String& message)
    {
        testResultsBox.moveCaretToEnd();
        testResultsBox.insertTextAtCaret (message + newLine);
        testResultsBox.moveCaretToEnd();
    }

    void testFinished()
    {
        stopTest();
        startTestButton.setEnabled (true);
        logMessage (newLine + "*** Tests finished ***");
    }

private:
    //==============================================================================
    class TestRunnerThread final : public Thread,
                                   private Timer
    {
    public:
        TestRunnerThread (TestRunnerComponent& utd, const String& ctg)
            : Thread ("Unit Tests"),
              owner (utd),
              category (ctg)
        {}

        void run() override
        {
            CustomTestRunner runner (*this);

            if (category == "All Tests")
                runner.runAllTests();
            else
                runner.runTestsInCategory (category);

            startTimer (50); // when finished, start the timer which will
                             // wait for the thread to end, then tell our component.
        }

        void logMessage (const String& message)
        {
            WeakReference<TestRunnerComponent> safeOwner (&owner);

            MessageManager::callAsync ([=]
            {
                if (auto* o = safeOwner.get())
                    o->logMessage (message);
            });
        }

        void timerCallback() override
        {
            if (! isThreadRunning())
                owner.testFinished(); // inform the demo page when done, so it can delete this thread.
        }

    private:
        //==============================================================================
        // This subclass of UnitTestRunner is used to redirect the test output to our
        // TextBox, and to interrupt the running tests when our thread is asked to stop..
        class CustomTestRunner final : public UnitTestRunner
        {
        public:
            CustomTestRunner (TestRunnerThread& trt)  : owner (trt) {}

            void logMessage (const String& message) override
            {
                owner.logMessage (message);
            }

            bool shouldAbortTests() override
            {
                return owner.threadShouldExit();
            }

        private:
            TestRunnerThread& owner;

            JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CustomTestRunner)
        };

        TestRunnerComponent& owner;
        const String category;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TestRunnerThread)
    };
    std::unique_ptr<TestRunnerThread> currentTestThread;

    TextButton startTestButton { "Run Unit Tests..." };
    ComboBox categoriesBox;
    TextEditor testResultsBox;

    void lookAndFeelChanged() override
    {
        testResultsBox.applyFontToAllText (testResultsBox.getFont());
    }

    JUCE_DECLARE_WEAK_REFERENCEABLE (TestRunnerComponent)
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TestRunnerComponent)
};
