#include "visualizer.h"

static const GLfloat g_vertex_buffer_data[] = {
 0.0f, -1.0f, 0.0f,
 1.0f, -1.0f, 0.0f,
 0.0f, 0.0f, 0.0f,
 1.0f, 0.0f, 0.0f,
};



Visualizer::Visualizer()
{
    //TODO RANDSEED 
    std::srand(std::time(nullptr));
    setup();

    std::string i1 = rand_color();
    std::string i2 = rand_color();
    
    Box b1 = Box(400.0f, 0.0f, "1E1022");
    Box b2 = Box(-450.0f, 75.0f, "0E2556");

    b1.id = i1;
    //b1.link_from.emplace_back(i2);
    //b1.link_to.emplace_back(i2);
    b2.id = i2;

    box_map.insert(std::make_pair(i1, b1));
    box_map.insert(std::make_pair(i2, b2));

    link_box(box_map.at(i1), box_map.at(i2));
    link_box(box_map.at(i2), box_map.at(i1));

    clear_color = hex_to_rgb("#4d5f83");
}

Visualizer::~Visualizer()
{

    
    /*boxes.clear();*/
    Box::box_batch.clear();
    arrow_map.clear();
    box_map.clear();
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
        drag_boxes();




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
            if (debug.debugmode) { debug.rectangle(cam_pos.x - w_w / 2 + 1, cam_pos.y + w_h / 2, w_w - 1, w_h - 1); }
            for (auto it = box_map.begin(); it != box_map.end(); it++) {
                debug.debug_box(it->second);
            }
            float cursor_size = 5;
            //ORIGIN CURSOR
            debug.cross(0, 0, 0, cursor_size);
            debug.circle(0, 0, 0, cursor_size);
            glBindBuffer(GL_ARRAY_BUFFER, debug.debug_vb);
            glBufferData(GL_ARRAY_BUFFER,
                debug.debug_batch.size() * sizeof(debug_Vertex),
                debug.debug_batch.data(),
                GL_STATIC_DRAW);
            glUseProgram(debug.debugID);
            MatrixID = glGetUniformLocation(debug.debugID, "u_MVP");
            glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);
            debug.debug_show(debug.debug_vb, debug.debug_batch.data(), debug.debug_batch.size());
        }





        glfwSwapBuffers(window);
        debug.debug_batch.clear();
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

glVertexAttribDivisor(0, 0);
glVertexAttribDivisor(1, 1);
glVertexAttribDivisor(2, 1);
glVertexAttribDivisor(3, 1);


// Draw the triangle !
glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, Box::box_batch.size());

glDisableVertexAttribArray(0);
glDisableVertexAttribArray(1);
glDisableVertexAttribArray(2);
glDisableVertexAttribArray(3);

}

void Visualizer::input()
{

    //INPUT CAMERA
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


    //INPUTS BOX
    //TEST AJOUT
    if (glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_PRESS) {
        push_box(rand_color());
    }

    //TEST SUPPRESSION
    if (glfwGetKey(window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS) {
        pop_box(last_selected_ID);
    }



    //INPUT WINDOW
    if (glfwGetKey(window, GLFW_KEY_KP_0) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

}



void Visualizer::link_box(Box& a, Box& b)
{

    //Create a bezier curve to link the two boxes
    Gradient<glm::vec4> Col_grdt;
    Col_grdt.push(std::make_pair(0.0, glm::vec4(a.base_color, 1.0)));
    Col_grdt.push(std::make_pair(1.0, glm::vec4(b.base_color, 1.0)));

    Gradient<float> Thk_grdt;
    Thk_grdt.push(std::make_pair(0.0, rand_float() * 100));
    Thk_grdt.push(std::make_pair(1.0, rand_float() * 100));

    auto seg = Polyline::Bezier(
        a.center(), a.center() + glm::vec3(0, 1000, 0),
        b.center() + glm::vec3(0, -1000, 0), b.center(),
        Thk_grdt, Col_grdt,
        JointType::BEVEL, TermType::SQUARE
    );

    //Add the dst box to the 'link to' list of the src box
    //And add the src box to the 'link from' list of the dst box
    a.link_to.emplace_back(b.id);
    b.link_from.emplace_back(a.id);

    //The arrow ID is the cat of the two boxes ID as it keeps the order
    arrow_map.insert(std::make_pair(a.id + b.id, seg));

}


void Visualizer::push_box(std::string boxID)
{
    box_map.insert(std::make_pair(boxID, Box((c_x - w_w / 2) + cam_pos.x, (w_h / 2 - c_y) + cam_pos.y, rand_color())));
}

void Visualizer::pop_box(std::string ID)
{
    if (!last_selected_ID.empty() ){
        //Clear the connected arrows and remove the ID from the ID lists 

        //Erase the arrows connected to the box
        //Erase the ID from their 'Link to' list
        for (std::string f_ID : box_map.at(ID).link_from) {
            arrow_map.erase(f_ID + ID);
            box_map.at(f_ID).link_to.remove(ID);
        }

        //Clear all the arrows connected to other boxes

        //Erase the arrows that connect to other boxes
        //Erase the ID from their 'link from' list
        for (std::string l_ID : box_map.at(ID).link_to) {
            arrow_map.erase(ID + l_ID);
            box_map.at(l_ID).link_from.remove(ID);
        }

        //Clear the box from the map
        box_map.erase(last_selected_ID);
    }
   
    //Erase the selection
    last_selected_ID = "";
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
    glDepthFunc(GL_LESS);

    //GL TRIANGLE

    VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    glGenBuffers(1, &vertexbuffer);
    glGenBuffers(1, &debug.debug_vb);

    Box::box_shader = LoadShaders("instance.vert", "instance.frag");
    line_shader_ID = LoadShaders("line.vert", "line.frag");
    debug.debugID = LoadShaders("triangle.vert", "triangle.frag");
    
    //CAMERA SETUP AND CANVAS
    projection = glm::ortho(-static_cast<float>(w_w) / 2, static_cast<float>(w_w) / 2, -static_cast<float>(w_h) / 2, static_cast<float>(w_h) / 2, 0.0f, 100.0f);
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
        //if (check_frustrum_render(boxes[i])) {
        //    debug_box(boxes[i]);
        //    Box::box_batch.insert(Box::box_batch.end(), boxes[i].model.begin(), boxes[i].model.end());
        //}
    }
}

void Visualizer::drag_boxes()
{
    static glm::vec3 cur_pos;
    static std::string ID = "";

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

        if (!last_selected_ID.empty() && (last_selected_ID != ID)) {
            //Reset the Z offset for priority 
            box_map.at(last_selected_ID).pos.z = 0;
            box_map.at(last_selected_ID).update();
        }

        if (!ID.empty()) {
            box_map.at(ID)._clicked = true;
            box_map.at(ID).pos.z = 1;
            box_map.at(ID).update();
        }
        //DELTA DE POSITION A CALCULER
        cur_pos = glm::vec3(c_x, c_y, 0);
        //update the front selected box
        last_selected_ID = ID;

    }

    //Sort every frame for now, until I have a real frustrum calling that sort before selecting
    std::sort(Box::box_batch.begin(), Box::box_batch.end(), sort_box);


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

            ID = "";
        }
    }
}


