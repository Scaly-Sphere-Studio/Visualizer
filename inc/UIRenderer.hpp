#ifndef SSS_GL_UIRENDERER_HPP
#define SSS_GL_UIRENDERER_HPP

#include <SSS/GL/Objects/Models/Line.hpp>
#include <SSS/GL/Objects/Renderer.hpp>
#include <SSS/GL/Objects/Models/PlaneRenderer.hpp>
#include <SSS/GL/Objects/Models/Plane.hpp>
#include <SSS/GL/Objects/Camera.hpp>
#include <SSS/GL/Objects/Shaders.hpp>
#include <SSS/GL/Objects/Basic.hpp>
#include <SSS/SceneGraph/scenegraph.h>
#include <SSS/SceneGraph/Node_UI.h>

SSS_GL_BEGIN;


// Ignore warning about STL exports as they're private members
#pragma warning(push, 2)
#pragma warning(disable: 4251)
#pragma warning(disable: 4275)

/** Unified ortho renderer for both textured planes (including text) and SDF shapes.
 *  Renders everything in a single pass with orthographic projection.
 */
class UIRenderer : public Renderer<UIRenderer>, public Observer {
    friend class SharedClass;
    friend class Window;

private:
    UIRenderer();

public:
    virtual ~UIRenderer() = default;
    Camera::Shared camera;
    virtual void render() override;

    using SharedClass::create;
    virtual void _subjectUpdate(Subject const& subject, int event_id) override;
    void updateResolution(const float _w, const float _h);
    void setWindow(SSS::GL::Window* pWindow) { _window = pWindow; };

    /** Add UI node with SDF primitives. */
    void push(Node_UI* n);

    /** Add a plane (textured geometry, including text). */
    void addPlane(std::shared_ptr<PlaneBase> plane);
    /** Remove a plane. */
    void removePlane(std::shared_ptr<PlaneBase> plane);

private:
    SSS::GL::Window* _window;
    
    // For SDF shapes
    Basic::VAO _sdf_vao;
    Basic::VBO _sdf_vbo;
    GLuint _sdf_ssbo = 0;
    std::vector<int> _nodes;

    // For planes (text and textured geometry)
    std::vector<std::shared_ptr<PlaneBase>> _planes;
    Basic::VAO _plane_vao;
    Basic::VBO _plane_static_vbo;
    Basic::IBO _plane_static_ibo;
    Basic::VBO _plane_model_vbo;
    Basic::VBO _plane_alpha_vbo;
    Basic::VBO _plane_tex_offset_vbo;
    bool _planes_update_vbos = true;

    glm::vec2 _resolution;
    glm::mat4 _proj;

    // Plane rendering helper
    void _setupPlaneVAO();
    void _updatePlaneVBOs();
    void _renderPlanes();
    void _renderSDFShapes();
};

#pragma warning(pop)

SSS_GL_END;

#endif // SSS_GL_UIRenderer_HPP