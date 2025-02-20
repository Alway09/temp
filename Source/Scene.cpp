#include "Scene.h"

Scene::Scene(StatefulObject& parent, OpenGLContext& context) : StatefulObject(parent, parent.getName(), String("Scene")), context(&context)
{
    uuidIdentifier = Uuid();
}

Scene::Scene(StatefulObject& parent, ObjectState objectState, OpenGLContext& context) : StatefulObject(parent, objectState), context(&context) {
    uuidIdentifier = Uuid();
    
    if(hasChildren()) {
        auto statesArray = getChildrenStates();
        for(auto state : statesArray) {
            createObject(SceneObjectRealisationHelper::fromString(state.getTree().getProperty("Type")), state);
        }
    }
}

Scene::~Scene()
{
    //shutdownOpenGL();
}

void Scene::shutdown()
{
    objects.clear();
    shader    .reset();
    uniforms  .reset();
}

void Scene::render()
{
    using namespace ::juce::gl;
    
    jassert (OpenGLHelpers::isContextActive());
    
    const ScopedLock lock (renderMutex);

    OpenGLContext* currentContext = OpenGLContext::getCurrentContext();
    auto desktopScale = (float) currentContext->getRenderingScale();

    {
        const ScopedLock lock (mutex);
        glViewport (roundToInt(desktopScale * (float) bounds.getX()),
                    roundToInt(desktopScale * (float) (parentHeight - bounds.getHeight() - bounds.getY())),
                    roundToInt(desktopScale * (float) bounds.getWidth()),
                    roundToInt(desktopScale * (float) bounds.getHeight()));
    }

    shader->use();

    if (uniforms->projectionMatrix != nullptr)
        uniforms->projectionMatrix->setMatrix4 (getProjectionMatrix().mat, 1, false);

    if (uniforms->viewMatrix != nullptr)
        uniforms->viewMatrix->setMatrix4 (getViewMatrix().mat, 1, false);

    for(auto obj : objects) {
        obj->draw();
    }

    // Reset the element buffers so child Components draw correctly
    glBindBuffer (GL_ARRAY_BUFFER, 0);
    glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, 0);
}

Matrix3D<float> Scene::getProjectionMatrix() const
{
    const ScopedLock lock (mutex);

    auto w = 1.0f / (0.5f + 0.1f);
    auto h = w * bounds.toFloat().getAspectRatio (false);

    return Matrix3D<float>::fromFrustum (-w, w, -h, h, 4.0f, 30.0f);
}

Matrix3D<float> Scene::getViewMatrix() const
{
    auto viewMatrix = Matrix3D<float>::fromTranslation ({ 0.0f, 0.0f, -10.0f });
    /*auto rotationMatrix = viewMatrix.rotation ({ -0.3f, 5.0f * std::sin ((float) getFrameCounter() * 0.01f), 0.0f });*/
    auto rotationMatrix = viewMatrix.rotation ({ -0.3f, 5.0f * std::sin ((float) 1 * 0.01f), 0.0f });

    return viewMatrix * rotationMatrix;
}

void Scene::createShaders()
{
    const ScopedLock lock (renderMutex);
    String vertexShader =
        "attribute vec4 position;\n"
        "attribute vec4 sourceColour;\n"
        "attribute vec2 textureCoordIn;\n"
        "\n"
        "uniform mat4 projectionMatrix;\n"
        "uniform mat4 viewMatrix;\n"
        "\n"
        "varying vec4 destinationColour;\n"
        "varying vec2 textureCoordOut;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    destinationColour = sourceColour;\n"
        "    textureCoordOut = textureCoordIn;\n"
        "    gl_Position = position;\n"
        "}\n";
    String fragmentShader =
       #if JUCE_OPENGL_ES
        "varying lowp vec4 destinationColour;\n"
        "varying lowp vec2 textureCoordOut;\n"
       #else
        "varying vec4 destinationColour;\n"
        "varying vec2 textureCoordOut;\n"
       #endif
        "\n"
        "void main()\n"
        "{\n"
       #if JUCE_OPENGL_ES
        "    lowp vec4 colour = vec4(0.95, 0.57, 0.03, 0.7);\n"
       #else
        "    vec4 colour = vec4(0.95, 0.57, 0.03, 0.7);\n"
       #endif
        "    gl_FragColor = destinationColour;\n"
        "}\n";

    OpenGLContext* currentContext = OpenGLContext::getCurrentContext();
    //std::unique_ptr<OpenGLShaderProgram> newShader (new OpenGLShaderProgram (*currentContext));
    OpenGLShaderProgram* newShader = new OpenGLShaderProgram (*currentContext);
    String statusText;

    if (newShader->addVertexShader (OpenGLHelpers::translateVertexShaderToV3 (vertexShader))
          && newShader->addFragmentShader (OpenGLHelpers::translateFragmentShaderToV3 (fragmentShader))
          && newShader->link())
    {
        //shader = std::move (newShader);
        shader.reset(newShader);
        shader->use();
        
        for(auto obj : objects) {
            obj->reset(shader);
        }
        uniforms  .reset (new Uniforms (*shader));

        statusText = "GLSL: v" + String (OpenGLShaderProgram::getLanguageVersion(), 2);
    }
    else
    {
        delete newShader;
        statusText = newShader->getLastError();
    }
}

//==============================================================================
Scene::Uniforms::Uniforms(OpenGLShaderProgram& shaderProgram)
{
    projectionMatrix.reset (createUniform (shaderProgram, "projectionMatrix"));
    viewMatrix      .reset (createUniform (shaderProgram, "viewMatrix"));
}

OpenGLShaderProgram::Uniform* Scene::Uniforms::createUniform(OpenGLShaderProgram& shaderProgram, const char* uniformName)
{
    using namespace ::juce::gl;

    if (glGetUniformLocation (shaderProgram.getProgramID(), uniformName) < 0)
        return nullptr;

    return new OpenGLShaderProgram::Uniform (shaderProgram, uniformName);
}
//==============================================================================
