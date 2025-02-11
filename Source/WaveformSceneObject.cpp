#include "WaveformSceneObject.h"

WaveformSceneObject::WaveformSceneObject(StatefulObject& parent) : SceneObject(parent, "Waveform" , SceneObjectRealisation::Waveform)
{
    setProperty(IDs::gain, IDs::getDefault(IDs::gain));
    setProperty(IDs::secondsToShow, IDs::getDefault(IDs::secondsToShow));
    
    init();
}

WaveformSceneObject::WaveformSceneObject(StatefulObject& parent, ObjectState objectState) : SceneObject(parent, objectState) {
    for(auto id : IDs::getAll()) {
        //setPropertyIfNotExists(id, IDs::getDefault(id));
        stateChanged(id);
    }
    
    init();
}

void WaveformSceneObject::init() {
    config.source = SceneObject::Config::DrawSource::Vertices;
    config.primitiveType = SceneObject::Config::DrawPrimitiveType::Lines;
    config.drawBufferUsage = SceneObject::Config::DrawBufferUsage::Stream;
    
    //vertices.resize(samplesToShow);
    
    needToUpdateBuffer = true;
}

void WaveformSceneObject::fillBuffers() {
    float step = 2.f/samplesToShow;
    vertices.clearQuick();
    auto iter = CustomAudioBuffer::getInst()->get(samplesToShow);
    float counter = 0.f;
    float val = 0;
    while(iter->getNext(0, val)) {
        vertices.add({{-1.f + counter * step, val * gain,  0.f},
            {0.5f, 0.5f, 0.5f},
            {0.95f, 0.57f, 0.03f, 0.7f},
            {2.f, 2.f}});
        counter += 1;
    }
    putVertices(vertices);
}

void WaveformSceneObject::stateChanged(const Identifier& property) {
    if(property == IDs::gain) {
        gain = getProperty(property);
    }
    if(property == IDs::secondsToShow) {
        needToUpdateBuffer = false;
        float secondsToShow = getProperty(property);
        samplesToShow = CustomAudioBuffer::getInst()->getSampleRate() * secondsToShow;
        vertices.clearQuick();
        vertices.resize(samplesToShow);
        needToUpdateBuffer = true;
    }
    // hide when loud or hide when quiet
}
