#include "WaveformSceneObject.h"

WaveformSceneObject::WaveformSceneObject(ValueTree treeAttachTo) : SceneObject(treeAttachTo, SceneObjectRealisation::Waveform)
{
    config.source = SceneObject::Config::DrawSource::Vertices;
    config.primitiveType = SceneObject::Config::DrawPrimitiveType::Lines;
    config.drawBufferUsage = SceneObject::Config::DrawBufferUsage::Stream;
    
    vertices.resize(4410);
    
    needToUpdateBuffer = true;
}

void WaveformSceneObject::fillBuffers() {
    float step = 2.f/4410.f;
    vertices.clearQuick();
    auto iter = SamplesHolderHolder::getInstance()->get(0, 4410);
    float counter = 0.f;
    while(iter.hasNext()) {
        vertices.add({{-1.f + counter * step, iter.getNext() * gain,  0.f},
            {0.5f, 0.5f, 0.5f},
            {0.95f, 0.57f, 0.03f, 0.7f},
            {2.f, 2.f}});
        counter += 1;
    }
    putVertices(vertices);
}
