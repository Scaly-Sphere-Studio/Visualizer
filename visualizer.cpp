#include "visualizer.h"

bool debugmode = true;
glm::vec3 debug_color{ 0.0f,1.0f,0.0f };

Box::Box(float width, float height, std::string hex)
{

    pos = glm::vec3(width, height, 0);
	base_color = hex_to_rgb(hex);
	create_box();
}

Box::~Box()
{
	model.clear();
}

void Box::log()
{
    for (size_t i = 0; i < model.size();i++) {
		std::cout << model[i]._x << " ; " 
            << model[i]._y << " ; " 
            << model[i]._z << std::endl;
	}
}

void Box::set_selected_col(std::string hex)
{
	selected_color = hex_to_rgb(hex);
}

void Box::set_col(std::string hex)
{
	base_color = hex_to_rgb(hex);
}

void Box::update()
{
    model.clear();
    model.emplace_back(pos.x - _w / 2.0f, pos.y + _h / 2.0f, 0.0f, base_color);
    model.emplace_back(pos.x - _w / 2.0f, pos.y - _h / 2.0f, 0.0f, base_color);
    model.emplace_back(pos.x + _w / 2.0f, pos.y - _h / 2.0f, 0.0, base_color);

    model.emplace_back(pos.x + _w / 2.0f, pos.y - _h / 2.0f, 0.0, base_color);
    model.emplace_back(pos.x + _w / 2.0f, pos.y + _h / 2.0f, 0.0, base_color);
    model.emplace_back(pos.x - _w / 2.0f, pos.y + _h / 2.0f, 0.0, base_color);
}

bool Box::check_collision(double x, double y)
{
    //Check if a point is hovering the box
    //Point to Box Collision test
    if( (glm::abs(x - pos.x) < _w / 2.0) &&
        (glm::abs(y - pos.y) < _h / 2.0) )  {
        
        if (!_selected) {
            //switch selected state
            _selected ^= 1;
            std::cout << "entered" << std::endl;
            for (int i = 0; i < model.size(); i++ ) {
                model[i]._col = selected_color;
            }
        }
        return true;
    }

    if (_selected) {
        _selected ^= 1;
        std::cout << "leaved" << std::endl;
        for (int i = 0; i < model.size(); i++) {
            model[i]._col = base_color;
        }
    }

    return false;
}

void Box::create_box()
{
    set_selected_col("C32530");
    update();
}

Vertex::Vertex(float x, float y, float z, glm::vec3 col)
{
	_x = x;
	_y = y;
	_z = z;

	_col = col;
}

Visualizer::Visualizer()
{
    /* Initialize the library */
    if (!glfwInit()) {
        std::cout << "couldn't init glfw" << std::endl;
    }

    window = glfwCreateWindow(w_w, w_h, "VISUALISEUR", NULL, NULL);
    if (!window)
    {
        __LOG_ERR("GLFW couldn't create the window context");
    }

    glfwMakeContextCurrent(window);
    glfwSetWindowPos(window, 600, 100);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
    }

    // Enable blending (transparency)
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    //GL TRIANGLE

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    glGenBuffers(1, &vertexbuffer);
    glGenBuffers(1, &debug_vb);

    GLuint programID = LoadShaders("triangle.vert", "triangle.frag");
    glUseProgram(programID);

    //CAMERA SETUP AND CANVAS
    projection = glm::ortho(-static_cast<float>(w_w)/2, static_cast<float>(w_w)/2, -static_cast<float>(w_h)/2, static_cast<float>(w_h)/2, 0.0f, 100.0f);
    view = glm::lookAt(
        cam_pos, // Camera is at (4,3,3), in World Space
        glm::vec3(cam_pos.x, cam_pos.y, 0), // and looks at the origin
        glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
    );
    

    mvp = projection * view;
    MatrixID = glGetUniformLocation(programID, "MVP");
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);


    clear_color = hex_to_rgb("#1D3958");
    boxes.emplace_back(0.0f, 0.0f, "FFCFD2");
    boxes.emplace_back(-100.0f, 150.0f, "00CFD2");


    



}

Visualizer::~Visualizer()
{
    debug_batch.clear();
    batch.clear();
}

void Visualizer::run()
{
    // Main loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        input();

        //Gen batch trough the frustrum
        //FRUSTRUM 
        if (debugmode) { rectangle(cam_pos.x, cam_pos.y, w_w * 2 / 3, w_h * 2 / 3); }
        for (int i = 0; i < boxes.size(); i++) {
            
            if (check_frustrum_render(boxes[i])) {
                debug_box(boxes[i]);
                batch.insert(batch.end(), boxes[i].model.begin(), boxes[i].model.end());
            }  
        }


        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glBufferData(GL_ARRAY_BUFFER,
            batch.size() * sizeof(Vertex),
            batch.data(),
            GL_STATIC_DRAW);

        glClearColor(clear_color.r, clear_color.g, clear_color.b, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 

        //Collision test
        glfwGetCursorPos(window, &c_x, &c_y);
        static glm::vec3 cur_pos;
        for (unsigned int i = 0; i < boxes.size();i++) {
            if (boxes[i].check_collision((c_x - w_w/2) + cam_pos.x, (w_h/2 - c_y) + cam_pos.y)
                && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS ) {
                
                if (!boxes[i]._clicked) { 
                    boxes[i]._clicked ^= 1; 
                    std::cout << "clicked" << std::endl;
                    //DELTA DE POSITION A CALCULER
                    cur_pos = glm::vec3(c_x, c_y, 0);
                }

            }
            if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE
                && boxes[i]._clicked) {
                boxes[i]._clicked ^= 1;
                std::cout << "released" << std::endl;
            }
            //DRAG BOX
            if (boxes[i]._clicked) {
                static glm::vec3 new_pos;
                new_pos = glm::vec3(c_x, c_y, 0);
                glm::vec3 delta = new_pos - cur_pos;
                cur_pos = new_pos;

                std::cout << delta.x << " : " << delta.y << std::endl;
                boxes[i].pos += glm::vec3(delta.x, - delta.y, 0);
                boxes[i].update();
            }

        }


        //TEST CURSOR DRAG



















        

        //UPDATE MVP MATRIX
        view = glm::lookAt(
            cam_pos, // Camera is at (4,3,3), in World Space
            glm::vec3(cam_pos.x, cam_pos.y, 0), // and looks at the origin
            glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
        );
        mvp = projection * view;
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);


        draw(vertexbuffer, batch.data(), batch.size());


        float cursor_size = 5;
        //ORIGIN CURSOR
        cross(0, 0, cursor_size);
        circle(0, 0, cursor_size);


        glBindBuffer(GL_ARRAY_BUFFER, debug_vb);
        glBufferData(GL_ARRAY_BUFFER,
            debug_batch.size() * sizeof(Vertex),
            debug_batch.data(),
            GL_STATIC_DRAW);

        debug_show(debug_vb, debug_batch.data(), debug_batch.size());
        glfwSwapBuffers(window);

        batch.clear();
        debug_batch.clear();
    }
}

void Visualizer::draw(GLuint buffer, void* data, size_t size)
{

    //Render
    //// 1st attribute buffer : vertices
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glVertexAttribPointer(
        0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
        3,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        sizeof(Vertex),                  // stride
        (void*)0            // array buffer offset
    );

    //// 2nd attribute buffer : colors
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glVertexAttribPointer(
        1,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
        3,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        sizeof(Vertex),                  // stride
        (void*)(3 * sizeof(float))           // array buffer offset
    );

    // Draw the triangle !
    glDrawArrays(GL_TRIANGLES, 0, size); // Starting from vertex 0; 3 vertices total -> 1 triangle
    
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}

void Visualizer::input()
{
    float speed = 10.0f;

    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        cam_pos -= glm::vec3(0.0f, speed, 0.0f);
    }

    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        cam_pos += glm::vec3(speed, 0.0f, 0.0f);
    }

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        cam_pos += glm::vec3(0.0f, speed, 0.0f);
    }

    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        cam_pos -= glm::vec3(speed, 0.0f, 0.0f);
    }

    if (glfwGetKey(window, GLFW_KEY_KP_0) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

void Visualizer::debug_show(GLuint buffer, void* data, size_t size)
{
    //Render
    //// 1st attribute buffer : vertices
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glVertexAttribPointer(
        0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
        3,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        sizeof(Vertex),                  // stride
        (void*)0            // array buffer offset
    );

    //// 2nd attribute buffer : colors
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glVertexAttribPointer(
        1,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
        3,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        sizeof(Vertex),                  // stride
        (void*)(3 * sizeof(float))           // array buffer offset
    );

    // Draw the triangle !
    glDrawArrays(GL_LINES, 0, size); // Starting from vertex 0; 3 vertices total -> 1 triangle
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}

void Visualizer::debug_box(Box &b)
{
    float cursor_size = 5;
    cross(b.pos.x, b.pos.y, cursor_size);
    circle(b.pos.x, b.pos.y, cursor_size);


    rectangle(b.pos.x, b.pos.y, b._w, b._h);

    circle(b.pos.x - b._w / 2, b.pos.y, cursor_size);
    circle(b.pos.x + b._w / 2, b.pos.y, cursor_size);
    circle(b.pos.x, b.pos.y + b._h / 2, cursor_size);
    circle(b.pos.x, b.pos.y - b._h / 2, cursor_size);

    circle(b.pos.x + b._w / 2, b.pos.y + b._h / 2, cursor_size);
    circle(b.pos.x + b._w / 2, b.pos.y - b._h / 2, cursor_size);
    circle(b.pos.x - b._w / 2, b.pos.y + b._h / 2, cursor_size);
    circle(b.pos.x - b._w / 2, b.pos.y - b._h / 2, cursor_size);
}

void Visualizer::circle(float x, float y, float radius)
{
    float branch = 20.0f;
    for (unsigned int i = 0; i < static_cast<unsigned int>(branch); i++) {

        debug_batch.emplace_back(
            x + radius * glm::cos(2.0f * i * glm::pi<float>() / branch),
            y + radius * glm::sin(2.0f * i * glm::pi<float>() / branch),
            0.1f, debug_color);
        debug_batch.emplace_back(
            x + radius * glm::cos(2.0f * ((i+1) % static_cast<int>(branch)) * glm::pi<float>() / branch),
            y + radius * glm::sin(2.0f * ((i+1) % static_cast<int>(branch)) * glm::pi<float>() / branch),
            0.1f, debug_color);
    }
}

void Visualizer::square(float x, float y, float radius)
{
    rectangle(x, y, radius, radius);
}

void Visualizer::rectangle(float x, float y, float width, float height)
{
    debug_batch.emplace_back(x + width / 2.0f, y - height / 2.0f, 0.1f, debug_color);
    debug_batch.emplace_back(x + width / 2.0f, y + height / 2.0f, 0.1f, debug_color);

    debug_batch.emplace_back(x + width / 2.0f, y + height / 2.0f, 0.1f, debug_color);
    debug_batch.emplace_back(x - width / 2.0f, y + height / 2.0f, 0.1f, debug_color);
 
    debug_batch.emplace_back(x - width / 2.0f, y + height / 2.0f, 0.1f, debug_color);
    debug_batch.emplace_back(x - width / 2.0f, y - height / 2.0f, 0.1f, debug_color);

    debug_batch.emplace_back(x - width / 2.0f, y - height / 2.0f, 0.1f, debug_color);
    debug_batch.emplace_back(x + width / 2.0f, y - height / 2.0f, 0.1f, debug_color);
}

bool Visualizer::check_frustrum_render(Box &b)
{
    //CHECK IF A BOX IS IN THE RENDERED WINDOW TROUGH THE SELECTED CAMERA
    float dx = glm::abs(cam_pos.x - b.pos.x);
    float dxmax = b._w/2 + w_w*2/6;    
    float dy = glm::abs(cam_pos.y - b.pos.y);
    float dymax = b._h/2 + w_h*2/6;


    if ((dx < dxmax) && (dy < dymax)) {
        return true;
    }
    return false;
}

void Visualizer::cross(float x, float y, float radius, float angle)
{
    debug_batch.emplace_back(
        x + radius * glm::cos(glm::radians(angle)),
        y + radius * glm::sin(glm::radians(angle)),
        0.1f, debug_color);
    debug_batch.emplace_back(
        x + radius * glm::cos(glm::pi<float>() + glm::radians(angle)),
        y + radius * glm::sin(glm::pi<float>() + glm::radians(angle)),
        0.1f, debug_color);

    debug_batch.emplace_back(
        x + radius * glm::cos(0.5 * glm::pi<float>() + glm::radians(angle)),
        y + radius * glm::sin(0.5 * glm::pi<float>() + glm::radians(angle)),
        0.1f, debug_color);
    debug_batch.emplace_back(
        x + radius * glm::cos(1.5 * glm::pi<float>() + glm::radians(angle)),
        y + radius * glm::sin(1.5 * glm::pi<float>() + glm::radians(angle)),
        0.1f, debug_color);

}

