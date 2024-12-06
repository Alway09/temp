#include "WaveformSceneObject.h"

WaveformSceneObject::WaveformSceneObject(SamplesHolder* samplesHolder) : SceneObject(samplesHolder)
{
    config.source = SceneObject::Config::DrawSource::Vertices;
    config.primitiveType = SceneObject::Config::DrawPrimitiveType::Lines;
    
    vertices.resize(4410);
    
    
    for(int i = 0; i < 4410; ++i) {
        vertices.add({{0.f, 0.f, 0.f},
            {0.5f, 0.5f, 0.5f},
            {1.f, 1.f, 1.f, 1.f},
            {2.f, 2.f}});
    }
}

void WaveformSceneObject::fillBuffers() {
    float step = 2.f/4410.f;
    vertices.clearQuick();
    auto iter = samplesHolder->get(0, 4410);
    float counter = 0.f;
    while(iter.hasNext()) {
        vertices.add({{-1.f + counter * step, iter.getNext() * gain,  0.f},
            {0.5f, 0.5f, 0.5f},
            {1.f, 1.f, 1.f, 1.f},
            {2.f, 2.f}});
        counter += 1;
    }
    putVertices(vertices);
}
