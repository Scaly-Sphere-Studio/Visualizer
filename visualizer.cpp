#include "visualizer.h"

std::vector<testBox>Box::box_batch{};
GLuint Box::box_shader = 0;

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

    pos = glm::vec3(width - _w / 2, height + _h / 2, 0.0);
	base_color = hex_to_rgb(hex);


    glm::vec3 newpos = glm::vec3(width - _w /2 , height + _h /2, 3.9);
    //Brightning the color
    glm::vec3 factor = (glm::vec3(1) - base_color) * glm::vec3(0.2);


    
    model.emplace_back(pos, glm::vec2(_w, _h), glm::vec4(base_color, 1.0f));
    model.emplace_back(pos, glm::vec2(_w - 2, _h / 3), glm::vec4(base_color + factor, 1.0f));
    

   
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
    if( ((x > pos.x) && (x < static_cast<double>(pos.x) + static_cast<double>(_w))) && 
        ((y < pos.y) && (y > static_cast<double>(pos.y) - static_cast<double>(_h))) )  {

        if (!_hovered) {
            //switch selected state
            _hovered ^= 1;
        }
        return true;
    }

    if (_hovered) {

        _hovered ^= 1;
    }

    return false;
}

void Box::create_box()
{
    set_selected_col("C32530");
}

void Box::update()
{
    for (size_t i = 0; i < model.size(); i++) {
        model[i].pos = pos;
    }
}

debug_Vertex::debug_Vertex(float x, float y, float z, glm::vec3 col)
{
	_x = x;
	_y = y;
	_z = z;

	_col = col;
}

Visualizer::Visualizer()
{
    //TODO RANDSEED 
    std::srand(std::time(nullptr));
    setup();
    
    box_map.insert(std::make_pair(rand_color(), Box( 0.0f, 0.0f, "1E1022")));
    box_map.insert(std::make_pair(rand_color(), Box(-100.0f, 150.0f, "0E2556")));

    clear_color = hex_to_rgb("#14213D");
}

Visualizer::~Visualizer()
{

    debug_batch.clear();
    /*boxes.clear();*/
    Box::box_batch.clear();
}

void Visualizer::run()
{
    

    //UPDATES THE BUFFER
    glGenBuffers(1, &billboard_vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    glGenBuffers(1, &particles_data);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    glm::vec4 black = glm::vec4(0, 0, 0, 1);
    glm::vec4 white = glm::vec4(1, 1, 1, 1);
    glm::vec4 blue = glm::vec4(0, 0.8, 1, 1);

    std::vector<Vertex>path;
    path.emplace_back(glm::vec3(-400.0f, 250.0f, 1.0f), black);
    path.emplace_back(glm::vec3(-400.0f, -250.0f, 1.0f), blue);
    path.emplace_back(glm::vec3(400.0f, 250.0f, 1.0f), blue);
    path.emplace_back(glm::vec3(400.0f, -250.0f, 1.0f), white);


    auto line = Polyline::Line(path, 10, blue);

    glDisable(GL_DEPTH_TEST);

    glClearColor(clear_color.r, clear_color.g, clear_color.b, 1.0f);

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        glfwGetCursorPos(window, &c_x, &c_y);
        input();

        glClear(GL_COLOR_BUFFER_BIT);


        //Gen batch trough the frustrum
        //FRUSTRUM 
        frustrum_test();

        //TEST CURSOR DRAG
        //UPDATE MVP MATRIX
        view = glm::lookAt(
            cam_pos,
            glm::vec3(cam_pos.x, cam_pos.y, 0),
            glm::vec3(0, 1, 0)
        );
        mvp = projection * view;

        //Collision test
        //drag_boxes();

        static glm::vec3 cur_pos;
        static std::string ID = "";
        static std::string last_selected_ID = box_map.begin()->first;

        //CHECK THE MAP FOR A COLLISION WITH A BOX 
        if (ID.empty() && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            for (auto it = box_map.begin(); it != box_map.end(); it++) {
                if (it->second.check_collision((c_x - w_w / 2.0) + cam_pos.x, (w_h / 2.0 - c_y) + cam_pos.y))
                {
                    std::string on_top_box_ID = it->first;

                    //Priority test for the box that is already on top for the collision test
                    if (ID.empty()) {
                        ID = on_top_box_ID;
                    }

                    if (!ID.empty() && (box_map.at(ID).pos.z < box_map.at(on_top_box_ID).pos.z)) {
                        //Check if the current Box is on top of the already other selected box
                        ID = on_top_box_ID;
                    }

                }
            }
            std::cout << ID << std::endl;


            if (!last_selected_ID.empty() && (last_selected_ID != ID)) {
                //Reset the Z offset for priority 
                box_map.at(last_selected_ID).pos.z = 0;
                box_map.at(last_selected_ID).update();
            }

            if (!ID.empty()) {
                box_map.at(ID)._clicked = true;
                box_map.at(ID).pos.z = 1;
                box_map.at(ID).update();
                std::cout << "[" << ID << "] : " << box_map.at(ID).pos.z << " [CLICKED]" << std::endl;
                
            }
            //DELTA DE POSITION A CALCULER
            cur_pos = glm::vec3(c_x, c_y, 0);
            //update the front selected box

            for (auto it = box_map.begin(); it != box_map.end(); it++) {
                std::cout << "[" << it->first << "]" << " z:" << it->second.pos.z << std::endl;
            }



            last_selected_ID = ID;
            std::sort(Box::box_batch.begin(), Box::box_batch.end(), sort_box);
        }

        

        if (!ID.empty()) {
            //DRAG BOX

            
            

            if (box_map.at(ID)._clicked) {
                glm::vec3 new_pos = { c_x, c_y, 0 };
                glm::vec3 delta = new_pos - cur_pos;

                box_map.at(ID).pos += glm::vec3(delta.x, -delta.y, 0);
                box_map.at(ID).update();

                cur_pos = new_pos;
            }

            if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE
                && box_map.at(ID)._clicked) {
                box_map.at(ID)._clicked = false;

                std::cout << "[" << ID << "] : " << box_map.at(ID).pos.z << " [RELEASED]"<< std::endl;
                
                ID = "";
            }
        }
        

        

        //LINE RENDERER
        {
            glUseProgram(line_shader_ID);
            MatrixID = glGetUniformLocation(line_shader_ID, "u_MVP");
            glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);
            Line_Batch::render();
        }

        //BOX RENDERER
        {
            glBindVertexArray(VertexArrayID);
            glUseProgram(Box::box_shader);

            //for (auto it = box_map.begin(); it != box_map.end(); it++) {
            //    std::cout << it->second.pos.z;
            //}
            //std::cout << std::endl;


            if (Box::box_batch.size() > 0) {
                glBindBuffer(GL_ARRAY_BUFFER, particles_data);
                glBufferData(GL_ARRAY_BUFFER, sizeof(testBox) * Box::box_batch.size(), Box::box_batch.data(), GL_STATIC_DRAW);            
            }

            
            MatrixID = glGetUniformLocation(Box::box_shader, "u_MVP");
            glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);
            
            draw();

        }

        //DEBUG RENDERER
        {
            
            float cursor_size = 5;
            //ORIGIN CURSOR
            cross(0, 0, 0, cursor_size);
            circle(0, 0, 0, cursor_size);
            glBindBuffer(GL_ARRAY_BUFFER, debug_vb);
            glBufferData(GL_ARRAY_BUFFER,
                debug_batch.size() * sizeof(debug_Vertex),
                debug_batch.data(),
                GL_STATIC_DRAW);
            glUseProgram(debugID);
            MatrixID = glGetUniformLocation(debugID, "u_MVP");
            glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);
            debug_show(debug_vb, debug_batch.data(), debug_batch.size());
        }

        glfwSwapBuffers(window);

        debug_batch.clear();
        Box::box_batch.clear(); 
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

    //Particles
    int first = 1, second = 2, third = 3;

    glBindBuffer(GL_ARRAY_BUFFER, particles_data);



    //Size separate by width and height
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        1,
        2, GL_FLOAT, GL_FALSE,
        sizeof(testBox), (void*)(sizeof(glm::vec3))
    );

    //Color
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(
        2,
        4, GL_FLOAT, GL_FALSE,
        sizeof(testBox), (void*)(sizeof(glm::vec3) + sizeof(glm::vec2))
    );

    //Position
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(
        3,
        3, GL_FLOAT, GL_FALSE,
        sizeof(testBox), (void*)0
    );

    glVertexAttribDivisor(0     , 0);
    glVertexAttribDivisor(1 , 1);
    glVertexAttribDivisor(2, 1);
    glVertexAttribDivisor(3 , 1);


    // Draw the triangle !
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, Box::box_batch.size());

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

    //TEST AJOUT
    if (glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_PRESS) {
        /*boxes.emplace_back((c_x - w_w / 2) + cam_pos.x, (w_h / 2 - c_y) + cam_pos.y,rand_color());*/
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

    }

    glfwMakeContextCurrent(window);
    glfwSetWindowPos(window, 200, 100);
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

    VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    glGenBuffers(1, &vertexbuffer);
    glGenBuffers(1, &debug_vb);

    Box::box_shader = LoadShaders("instance.vert", "instance.frag");
    debugID = LoadShaders("triangle.vert", "triangle.frag");
    line_shader_ID = LoadShaders("line.vert", "line.frag");
    
    //CAMERA SETUP AND CANVAS
    projection = glm::ortho(-static_cast<float>(w_w) / 2, static_cast<float>(w_w) / 2, -static_cast<float>(w_h) / 2, static_cast<float>(w_h) / 2, 0.0f, 100.0f);
}

void Visualizer::debug_box(const Box &b)
{
    float cursor_size = 5;
    //center
    cross(b.pos.x + b._w/2 , b.pos.y - b._h /2,     0.8, cursor_size);
    circle(b.pos.x + b._w / 2, b.pos.y - b._h / 2,  0.8, cursor_size);

    //cage
    rectangle(b.pos.x, b.pos.y, b._w, b._h);

    //corner
    circle(b.pos.x, b.pos.y,                0.8, cursor_size);
    circle(b.pos.x, b.pos.y - b._h,         0.8, cursor_size);
    circle(b.pos.x + b._w, b.pos.y ,        0.8, cursor_size);
    circle(b.pos.x + b._w, b.pos.y - b._h,  0.8, cursor_size);

    //mid
    circle(b.pos.x + b._w / 2, b.pos.y, 0.8, cursor_size);
    circle(b.pos.x + b._w / 2, b.pos.y - b._h,             0.8, cursor_size);
    circle(b.pos.x, b.pos.y - b._h / 2,  0.8, cursor_size);
    circle(b.pos.x + b._w, b.pos.y - b._h / 2,  0.8, cursor_size);
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
    debug_batch.emplace_back(x + width, y - height, 0.8f, debug_color);
    debug_batch.emplace_back(x + width, y, 0.8f, debug_color);

    debug_batch.emplace_back(x + width, y , 0.8f, debug_color);
    debug_batch.emplace_back(x, y, 0.8f, debug_color);

    debug_batch.emplace_back(x , y, 0.8f, debug_color);
    debug_batch.emplace_back(x , y - height, 0.8f, debug_color);

    debug_batch.emplace_back(x , y - height, 0.8f, debug_color);
    debug_batch.emplace_back(x + width, y - height, 0.8f, debug_color);
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

void Visualizer::frustrum_test()
{
    if (debugmode) { rectangle(cam_pos.x - w_w / 2 + 1, cam_pos.y + w_h / 2, w_w - 1, w_h - 1); }

    //for (int i = 0; i < boxes.size(); i++) {
    //    Box::box_batch.insert(Box::box_batch.end(), boxes[i].model.begin(), boxes[i].model.end());
    //    debug_box(boxes[i]);
    //    //if (check_frustrum_render(boxes[i])) {
    //    //    debug_box(boxes[i]);
    //    //    Box::box_batch.insert(Box::box_batch.end(), boxes[i].model.begin(), boxes[i].model.end());
    //    //}
    //}

    for (auto it = box_map.begin(); it != box_map.end(); it++) {
        Box::box_batch.insert(Box::box_batch.end(), it->second.model.begin(), it->second.model.end());
        debug_box(it->second);
        //if (check_frustrum_render(boxes[i])) {
        //    debug_box(boxes[i]);
        //    Box::box_batch.insert(Box::box_batch.end(), boxes[i].model.begin(), boxes[i].model.end());
        //}
    }
}

void Visualizer::drag_boxes()
{
//TODO 
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

