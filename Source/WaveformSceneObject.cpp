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
    
    needToUpdateBuffer = true;
}

void WaveformSceneObject::fillBuffers() {
    auto iter = CustomAudioBuffer::getInst()->get(samplesToShow);
    float val = 0.f;
    int counter = 0;
    while(iter->getNext(0, val)) {
        vertices.getReference(counter).position[1] = val * gain;
        ++counter;
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
        updateBuffer();
        needToUpdateBuffer = true;
    }
    // hide when loud or hide when quiet
}
