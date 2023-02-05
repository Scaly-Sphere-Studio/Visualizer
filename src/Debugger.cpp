#include "Debugger.h"
#include "Visualizer.h"

static constexpr glm::vec3 debug_color{ 0.0f,1.0f,0.0f };

debug_Vertex::debug_Vertex(float x, float y, float z, glm::vec3 col)
{
    _x = x;
    _y = y;
    _z = z;

    _col = col;
}

Debugger::Debugger(SSS::GL::Window::Shared win)
    : SSS::GL::Renderer<Debugger>(win), vbo(win)
{
    SSS::GL::Context const context = getContext();

    vbo.bind();

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0,
        3, GL_FLOAT, GL_FALSE,
        sizeof(debug_Vertex),
        (void*)0
    );

    //// 2nd attribute buffer : colors
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        1,
        3, GL_FLOAT, GL_FALSE,
        sizeof(debug_Vertex),
        (void*)(3 * sizeof(float))
    );
}

Debugger::~Debugger()
{
    debug_batch.clear();
}


void Debugger::debug_box(const Box& b)
{
    float cursor_size = 5.f;
    //center
    cross(b._pos.x + b._size.x / 2.f, b._pos.y - b._size.y / 2.f, 0.8f, cursor_size);
    circle(b._pos.x + b._size.x / 2.f, b._pos.y - b._size.y / 2.f, 0.8f, cursor_size);

    //cage
    rectangle(b._pos.x, b._pos.y, b._size.x, b._size.y);

    //corner
    circle(b._pos.x, b._pos.y, 0.8f, cursor_size);
    circle(b._pos.x, b._pos.y - b._size.y, 0.8f, cursor_size);
    circle(b._pos.x + b._size.x, b._pos.y, 0.8f, cursor_size);
    circle(b._pos.x + b._size.x, b._pos.y - b._size.y, 0.8f, cursor_size);

    //mid
    circle(b._pos.x + b._size.x / 2.f, b._pos.y, 0.8f, cursor_size);
    circle(b._pos.x + b._size.x / 2.f, b._pos.y - b._size.y, 0.8f, cursor_size);
    circle(b._pos.x, b._pos.y - b._size.y / 2.f, 0.8f, cursor_size);
    circle(b._pos.x + b._size.x, b._pos.y - b._size.y / 2.f, 0.8f, cursor_size);
}

void Debugger::circle(float x, float y, float z, float radius)
{
    float branch = 20.0f;
    for (unsigned int i = 0; i < static_cast<unsigned int>(branch); i++) {

        debug_batch.emplace_back(
            x + radius * glm::cos(2.0f * i * glm::pi<float>() / branch),
            y + radius * glm::sin(2.0f * i * glm::pi<float>() / branch),
            z, debug_color);
        debug_batch.emplace_back(
            x + radius * glm::cos(2.0f * ((i + 1) % static_cast<int>(branch)) * glm::pi<float>() / branch),
            y + radius * glm::sin(2.0f * ((i + 1) % static_cast<int>(branch)) * glm::pi<float>() / branch),
            z, debug_color);
    }
}

void Debugger::square(float x, float y, float radius)
{
    rectangle(x, y, radius, radius);
}

void Debugger::rectangle(float x, float y, float width, float height)
{
    debug_batch.emplace_back(x + width, y - height, 0.8f, debug_color);
    debug_batch.emplace_back(x + width, y, 0.8f, debug_color);

    debug_batch.emplace_back(x + width, y, 0.8f, debug_color);
    debug_batch.emplace_back(x, y, 0.8f, debug_color);

    debug_batch.emplace_back(x, y, 0.8f, debug_color);
    debug_batch.emplace_back(x, y - height, 0.8f, debug_color);

    debug_batch.emplace_back(x, y - height, 0.8f, debug_color);
    debug_batch.emplace_back(x + width, y - height, 0.8f, debug_color);
}


void Debugger::cross(float x, float y, float radius, float angle)
{
    debug_batch.emplace_back(
        x + radius * glm::cos(glm::radians(angle)),
        y + radius * glm::sin(glm::radians(angle)),
        0.0f, debug_color);
    debug_batch.emplace_back(
        x + radius * glm::cos(glm::pi<float>() + glm::radians(angle)),
        y + radius * glm::sin(glm::pi<float>() + glm::radians(angle)),
        0.0f, debug_color);

    debug_batch.emplace_back(
        x + radius * glm::cos(0.5f * glm::pi<float>() + glm::radians(angle)),
        y + radius * glm::sin(0.5f * glm::pi<float>() + glm::radians(angle)),
        0.0f, debug_color);
    debug_batch.emplace_back(
        x + radius * glm::cos(1.5f * glm::pi<float>() + glm::radians(angle)),
        y + radius * glm::sin(1.5f * glm::pi<float>() + glm::radians(angle)),
        0.0f, debug_color);

}

void Debugger::render()
{
    if (!isActive()) return;

    Visualizer::Ptr const& visu = Visualizer::get();

    SSS::GL::Context const context = getContext();

    glm::vec3 const cam_pos = camera->getPosition();
    rectangle(cam_pos.x - visu->_info._w / 2 + 1, cam_pos.y + visu->_info._h / 2, visu->_info._w - 1, visu->_info._h - 1);
    for (auto it = visu->_proj.box_map.begin(); it != visu->_proj.box_map.end(); it++) {
        debug_box(it->second);
    }
    float cursor_size = 5;
    //ORIGIN CURSOR
    cross(0, 0, 0, cursor_size);
    circle(0, 0, 0, cursor_size);
    vbo.edit(
        debug_batch.size() * sizeof(debug_Vertex),
        debug_batch.data(),
        GL_STATIC_DRAW
    );

    auto const shader = getShaders();
    glm::mat4 const mvp = camera->getVP();
    shader->use();
    shader->setUniformMat4fv("u_MVP", 1, GL_FALSE, &mvp[0][0]);

    //Render
    vbo.bind();
    glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(debug_batch.size())); // Starting from vertex 0; 3 vertices total -> 1 triangle

    debug_batch.clear();
}
