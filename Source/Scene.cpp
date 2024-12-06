#include "Scene.h"

Scene::Scene(ValueTree treeAttachTo)
{
    identifier = Uuid().toString();
    valueTree = treeAttachTo.getOrCreateChildWithName(identifier, nullptr);
    
    //openGLContext.detach();
    //openGLContext.setComponentPaintingEnabled(false);// ??? мерцает отображение
    //openGLContext.attachTo(*this);
}

Scene::~Scene()
{
    shutdownOpenGL();
}

void Scene::initialise()
{
    createShaders();
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

    auto desktopScale = (float) openGLContext.getRenderingScale();
    OpenGLHelpers::clear (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
    
    //static Colour transparent;
    //OpenGLHelpers::clear (transparent);

    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    {
        const ScopedLock lock (mutex);
        glViewport (0, 0,
                    roundToInt (desktopScale * (float) bounds.getWidth()),
                    roundToInt (desktopScale * (float) bounds.getHeight()));
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
    auto rotationMatrix = viewMatrix.rotation ({ -0.3f, 5.0f * std::sin ((float) getFrameCounter() * 0.01f), 0.0f });

    return viewMatrix * rotationMatrix;
}

void Scene::createShaders()
{
    vertexShader =
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

    fragmentShader =
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
        "    gl_FragColor = colour;\n"
        "}\n";

    std::unique_ptr<OpenGLShaderProgram> newShader (new OpenGLShaderProgram (openGLContext));
    String statusText;

    if (newShader->addVertexShader (OpenGLHelpers::translateVertexShaderToV3 (vertexShader))
          && newShader->addFragmentShader (OpenGLHelpers::translateFragmentShaderToV3 (fragmentShader))
          && newShader->link())
    {
        shader = std::move (newShader);
        shader->use();
        
        for(auto obj : objects) {
            obj->reset(*shader);
        }
        uniforms  .reset (new Uniforms (*shader));

        statusText = "GLSL: v" + String (OpenGLShaderProgram::getLanguageVersion(), 2);
    }
    else
    {
        statusText = newShader->getLastError();
    }
}

void Scene::resized()
{
    const ScopedLock lock (mutex);
    bounds = getLocalBounds();
}

void Scene::paint(Graphics &g) {}

//==============================================================================
Scene::Uniforms::Uniforms(OpenGLShaderProgram& shaderProgram)
{
    projectionMatrix.reset (createUniform (shaderProgram, "projectionMatrix"));
    viewMatrix      .reset (createUniform (shaderProgram, "viewMatrix"));
}
//==============================================================================
