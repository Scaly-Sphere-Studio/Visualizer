#include "visualizer.h"

bool debugmode = true;
glm::vec3 debug_color{ 0.0f,1.0f,0.0f };

static const GLfloat g_vertex_buffer_data[] = {
 0.0f, -1.0f, 0.0f,
 1.0f, -1.0f, 0.0f,
 0.0f, 0.0f, 0.0f,
 1.0f, 0.0f, 0.0f,
};

Box::Box(float width, float height, std::string hex)
{

    pos = glm::vec3(width, height, 0);
	base_color = hex_to_rgb(hex);


    glm::vec3 newpos = glm::vec3(pos.x - _w /2 , pos.y + _h /2, 0.0);
    //Brightning the color
    glm::vec3 factor = (glm::vec3(1) - base_color) * glm::vec3(0.2);

    model.emplace_back(newpos, glm::vec2(_w -2, _h / 3), glm::vec4(base_color + factor, 0.9));
    model.emplace_back(newpos, glm::vec2(_w, _h), glm::vec4(base_color, 1.0f));
}

Box::~Box()
{
}


void Box::set_selected_col(std::string hex)
{
	selected_color = hex_to_rgb(hex);
}

void Box::set_col(std::string hex)
{
	base_color = hex_to_rgb(hex);
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
            /*std::cout << "entered" << std::endl;*/
        }
        return true;
    }

    if (_selected) {
        _selected ^= 1;
        /*std::cout << "leaved" << std::endl;*/
    }

    return false;
}

void Box::create_box()
{
    set_selected_col("C32530");
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
    setup();

    //tb.emplace_back(glm::vec3(1.0f, -1.0f, 0.3f), glm::vec2(148, 28), glm::vec4(0.8f, 0.8f, 0.8f, 1.0f));
    //tb.emplace_back(glm::vec3(0.0f, 0.0f, 0.3f), glm::vec2(150, 150), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    



    boxes.emplace_back(100.0f, -150.0f, "1E1022");
    boxes.emplace_back(-100.0f, 150.0f, "0E2556");

    clear_color = hex_to_rgb("#14213D");


    glClearColor(clear_color.r, clear_color.g, clear_color.b, 0);

}

Visualizer::~Visualizer()
{
    debug_batch.clear();
    batch.clear();
}

void Visualizer::run()
{
    

    //UPDATES THE BUFFER
    glGenBuffers(1, &billboard_vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    glGenBuffers(1, &particles_data);
    //glBindBuffer(GL_ARRAY_BUFFER, particles_data);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(testBox) * batch.size(), batch.data(), GL_STATIC_DRAW);

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        input();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        //Gen batch trough the frustrum
        //FRUSTRUM 
        if (debugmode) { rectangle(cam_pos.x, cam_pos.y, w_w-1, w_h-1); }
        
        for (int i = 0; i < boxes.size(); i++) {
            
            if (check_frustrum_render(boxes[i])) {
                debug_box(boxes[i]);
                batch.insert(batch.end(), boxes[i].model.begin(), boxes[i].model.end());
            }  
        }



        //TEST CURSOR DRAG
        //UPDATE MVP MATRIX
        view = glm::lookAt(
            cam_pos,
            glm::vec3(cam_pos.x, cam_pos.y, 0),
            glm::vec3(0, 1, 0)
        );
        mvp = projection * view;



        //glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        //glBufferData(GL_ARRAY_BUFFER,
        //    batch.size() * sizeof(Vertex),
        //    batch.data(),
        //    GL_STATIC_DRAW);

        ////Collision test
        //glfwGetCursorPos(window, &c_x, &c_y);
        //static glm::vec3 cur_pos;
        //for (unsigned int i = 0; i < boxes.size();i++) {
        //    if (boxes[i].check_collision((c_x - w_w/2) + cam_pos.x, (w_h/2 - c_y) + cam_pos.y)
        //        && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS ) {
        //        
        //        if (!boxes[i]._clicked) { 
        //            boxes[i]._clicked ^= 1; 
        //            /*std::cout << "clicked" << std::endl;*/
        //            //DELTA DE POSITION A CALCULER
        //            cur_pos = glm::vec3(c_x, c_y, 0);
        //        }

        //    }
        //    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE
        //        && boxes[i]._clicked) {
        //        boxes[i]._clicked ^= 1;
        //        /*std::cout << "released" << std::endl;*/
        //    }
        //    //DRAG BOX
        //    if (boxes[i]._clicked) {
        //        static glm::vec3 new_pos;
        //        new_pos = glm::vec3(c_x, c_y, 0);
        //        glm::vec3 delta = new_pos - cur_pos;
        //        cur_pos = new_pos;

        //        /*std::cout << delta.x << " : " << delta.y << std::endl;*/
        //        boxes[i].pos += glm::vec3(delta.x, - delta.y, 0);
        //        boxes[i].update();
        //    }
        //}


        if (batch.size() > 0) {
            glBindBuffer(GL_ARRAY_BUFFER, particles_data);
            glBufferData(GL_ARRAY_BUFFER, sizeof(testBox) * batch.size(), batch.data(), GL_STATIC_DRAW);
        }


        std::cout << batch.size() << std::endl;
        glUseProgram(programID);
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);
        draw();


        //DEBUG
        float cursor_size = 5;
        //ORIGIN CURSOR
        cross(0, 0, 0, cursor_size);
        circle(0, 0, 0, cursor_size);

        glBindBuffer(GL_ARRAY_BUFFER, debug_vb);
        glBufferData(GL_ARRAY_BUFFER,
            debug_batch.size() * sizeof(Vertex),
            debug_batch.data(),
            GL_STATIC_DRAW);


        glUseProgram(debugID);
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);
        debug_show(debug_vb, debug_batch.data(), debug_batch.size());
        

        debug_batch.clear();
        glfwSwapBuffers(window);

        batch.clear();
        
        
    }
}

void Visualizer::draw()
{

    //Render
    //// 1st attribute buffer : vertices
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
    glVertexAttribPointer(
        0,
        3, GL_FLOAT, GL_FALSE,
        sizeof(float) * 3, (void*)0
    );

    //Position
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, particles_data);
    glVertexAttribPointer(
        1,
        3, GL_FLOAT, GL_FALSE,
        sizeof(testBox), (void*)0
    );

    //Size separate by width and height
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, particles_data);
    glVertexAttribPointer(
        2,
        2, GL_FLOAT, GL_FALSE,
        sizeof(testBox), (void*)(sizeof(glm::vec3))
    );

    //Color
    glEnableVertexAttribArray(3);
    glBindBuffer(GL_ARRAY_BUFFER, particles_data);
    glVertexAttribPointer(
        3,
        4, GL_FLOAT, GL_FALSE,
        sizeof(testBox), (void*)(sizeof(glm::vec3) + sizeof(glm::vec2))
    );

    glVertexAttribDivisor(0, 0);
    glVertexAttribDivisor(1, 1);
    glVertexAttribDivisor(2, 1);
    glVertexAttribDivisor(3, 1);


    // Draw the triangle !
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, batch.size());

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(3);


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
        0,               
        3, GL_FLOAT, GL_FALSE,          
        sizeof(Vertex), 
        (void*)0           
    );

    //// 2nd attribute buffer : colors
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glVertexAttribPointer(
        1,                  
        3, GL_FLOAT, GL_FALSE,           
        sizeof(Vertex),     
        (void*)(3 * sizeof(float))   
    );

    // Draw the triangle !
    glDrawArrays(GL_LINES, 0, size); // Starting from vertex 0; 3 vertices total -> 1 triangle
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}

void Visualizer::setup()
{
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

    programID = LoadShaders("instance.vert", "instance.frag");
    debugID = LoadShaders("triangle.vert", "triangle.frag");


    //CAMERA SETUP AND CANVAS
    projection = glm::ortho(-static_cast<float>(w_w) / 2, static_cast<float>(w_w) / 2, -static_cast<float>(w_h) / 2, static_cast<float>(w_h) / 2, 0.0f, 100.0f);
    view = glm::lookAt(
        cam_pos,
        glm::vec3(cam_pos.x, cam_pos.y, 0), 
        glm::vec3(0, 1, 0)  
    );


    mvp = projection * view;
    MatrixID = glGetUniformLocation(programID, "MVP");


}

void Visualizer::debug_box(Box &b)
{
    float cursor_size = 5;
    cross(b.pos.x,  b.pos.y, 0, cursor_size);
    circle(b.pos.x, b.pos.y, 0, cursor_size);


    rectangle(b.pos.x, b.pos.y, b._w, b._h);

    circle(b.pos.x - b._w / 2, b.pos.y, 0, cursor_size);
    circle(b.pos.x + b._w / 2, b.pos.y, 0, cursor_size);
    circle(b.pos.x, b.pos.y + b._h / 2, 0, cursor_size);
    circle(b.pos.x, b.pos.y - b._h / 2, 0, cursor_size);

    circle(b.pos.x + b._w / 2, b.pos.y + b._h / 2, 0, cursor_size);
    circle(b.pos.x + b._w / 2, b.pos.y - b._h / 2, 0, cursor_size);
    circle(b.pos.x - b._w / 2, b.pos.y + b._h / 2, 0, cursor_size);
    circle(b.pos.x - b._w / 2, b.pos.y - b._h / 2, 0, cursor_size);
}

void Visualizer::circle(float x, float y, float z, float radius)
{
    float branch = 20.0f;
    for (unsigned int i = 0; i < static_cast<unsigned int>(branch); i++) {

        debug_batch.emplace_back(
            x + radius * glm::cos(2.0f * i * glm::pi<float>() / branch),
            y + radius * glm::sin(2.0f * i * glm::pi<float>() / branch),
            z, debug_color);
        debug_batch.emplace_back(
            x + radius * glm::cos(2.0f * ((i+1) % static_cast<int>(branch)) * glm::pi<float>() / branch),
            y + radius * glm::sin(2.0f * ((i+1) % static_cast<int>(branch)) * glm::pi<float>() / branch),
            z, debug_color);
    }
}

void Visualizer::square(float x, float y, float radius)
{
    rectangle(x, y, radius, radius);
}

void Visualizer::rectangle(float x, float y, float width, float height)
{
    debug_batch.emplace_back(x + width / 2.0f, y - height / 2.0f, 0.8f, debug_color);
    debug_batch.emplace_back(x + width / 2.0f, y + height / 2.0f, 0.8f, debug_color);
    debug_batch.emplace_back(x + width / 2.0f, y + height / 2.0f, 0.8f, debug_color);
    debug_batch.emplace_back(x - width / 2.0f, y + height / 2.0f, 0.8f, debug_color);
    debug_batch.emplace_back(x - width / 2.0f, y + height / 2.0f, 0.8f, debug_color);
    debug_batch.emplace_back(x - width / 2.0f, y - height / 2.0f, 0.8f, debug_color);
    debug_batch.emplace_back(x - width / 2.0f, y - height / 2.0f, 0.8f, debug_color);
    debug_batch.emplace_back(x + width / 2.0f, y - height / 2.0f, 0.8f, debug_color);
}

bool Visualizer::check_frustrum_render(Box &b)
{
    //CHECK IF A BOX IS IN THE RENDERED WINDOW TROUGH THE SELECTED CAMERA
    float dx = glm::abs(cam_pos.x - b.pos.x);
    float dxmax = (b._w + w_w) * 0.5;    
    float dy = glm::abs(cam_pos.y - b.pos.y);
    float dymax = (b._h + w_h) * 0.5;


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
        0.0f, debug_color);
    debug_batch.emplace_back(
        x + radius * glm::cos(glm::pi<float>() + glm::radians(angle)),
        y + radius * glm::sin(glm::pi<float>() + glm::radians(angle)),
        0.0f, debug_color);

    debug_batch.emplace_back(
        x + radius * glm::cos(0.5 * glm::pi<float>() + glm::radians(angle)),
        y + radius * glm::sin(0.5 * glm::pi<float>() + glm::radians(angle)),
        0.0f, debug_color);
    debug_batch.emplace_back(
        x + radius * glm::cos(1.5 * glm::pi<float>() + glm::radians(angle)),
        y + radius * glm::sin(1.5 * glm::pi<float>() + glm::radians(angle)),
        0.0f, debug_color);

}

testBox::testBox(glm::vec3 _pos, glm::vec2 s, glm::vec4 _col) :
    pos(_pos), size(s), color(_col)
{
}
testBox::~testBox()
{
}
