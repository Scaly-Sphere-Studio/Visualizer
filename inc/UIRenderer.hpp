#ifndef SSS_GL_UIRENDERER_HPP
#define SSS_GL_UIRENDERER_HPP

#include <SSS/GL/Objects/Models/Line.hpp>
#include <SSS/GL/Objects/Renderer.hpp>
#include <SSS/GL/Objects/Camera.hpp>


#include "Node_UI.h"
SSS_GL_BEGIN;


// Ignore warning about STL exports as they're private members
#pragma warning(push, 2)
#pragma warning(disable: 4251)
#pragma warning(disable: 4275)

class UIRenderer : public Renderer<UIRenderer>,public SSS::Observer, public SceneGraph {
    friend class SharedClass;
    friend class Window;


private:
    UIRenderer();

public:
    Camera::Shared camera;
    virtual void render() override;

    using SharedClass::create;
    virtual void _subjectUpdate(Subject const& subject, int event_id) override {};
    void updateResolution(const float _w, const float _h);
    void setWindow(SSS::GL::Window* pWindow) { _window = pWindow; };

    void push(Node_UI* n);

private:

    SSS::GL::Window* _window;
    Basic::VAO _vao;
    Basic::VBO _vbo;
    GLuint ssbo = 0;
    std::vector<int> _nodes;

    glm::vec2 _resolution;
    glm::mat4 _proj;
};

#pragma warning(pop)

SSS_GL_END;

#endif // SSS_GL_UIRenderer_HPP