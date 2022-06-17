#include "../inc/Debugger.h"


glm::vec3 debug_color{ 0.0f,1.0f,0.0f };

debug_Vertex::debug_Vertex(float x, float y, float z, glm::vec3 col)
{
    _x = x;
    _y = y;
    _z = z;

    _col = col;
}

Debugger::Debugger()
{
    debug_vb = 0;
    debugID = 0;
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

void Debugger::debug_show(GLuint buffer, void* data, size_t size)
{

    //Render
    //// 1st attribute buffer : vertices
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glVertexAttribPointer(
        0,
        3, GL_FLOAT, GL_FALSE,
        sizeof(debug_Vertex),
        (void*)0
    );

    //// 2nd attribute buffer : colors
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glVertexAttribPointer(
        1,
        3, GL_FLOAT, GL_FALSE,
        sizeof(debug_Vertex),
        (void*)(3 * sizeof(float))
    );

    // Draw the triangle !
    glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(size)); // Starting from vertex 0; 3 vertices total -> 1 triangle
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}