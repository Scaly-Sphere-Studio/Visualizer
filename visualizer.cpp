#include "visualizer.h"

Box::Box(float width, float height, std::string hex)
{
	base_color = hex_to_rgb(hex);
	create_box();
}

Box::~Box()
{
	model.clear();
}

void Box::log()
{
	for (Vertex v : model) {
		std::cout << v._x << " ; " << v._y << " ; " << v._z << std::endl;
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
	
}

bool Box::check_collision(double x, double y)
{
    //si x1 < x < x2 and y1 < y < y2
    if( (x > pos.x && x < pos.x + _w) &&
        (y > pos.y && y < pos.y + _h) )  {
        
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
	model.emplace_back(pos.x, pos.y, 0.0f, base_color);
	model.emplace_back(pos.x, pos.y - _h, 0.0f, base_color);
	model.emplace_back(pos.x + _w, pos.y - _h, 0.0, base_color);

	model.emplace_back(pos.x + _w, pos.y - _h, 0.0, base_color);
	model.emplace_back(pos.x + _w, pos.y, 0.0, base_color);
	model.emplace_back(pos.x, pos.y, 0.0, base_color);
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

    // Create a window. This is where the OpenGL context is created.




    window = glfwCreateWindow(w_w, w_h, "Hello World", NULL, NULL);
    if (!window)
    {
        __LOG_ERR("GLFW couldn't create the window context");
    }

    glfwMakeContextCurrent(window);
    /*glfwSetWindowPos(window, 100, 100);*/
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
    }

    //Set the multiple input callbacks
    /*glfwSetCursorPosCallback(window, cursor_position_callback);*/

    //// Set the window's callbacks
    //window->setCallback(glfwSetKeyCallback, key_callback);
    //window->setCallback(glfwSetMouseButtonCallback, mouse_button_callback);



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
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER,
        box.model.size() * sizeof(Vertex),
        box.model.data(),
        GL_STATIC_DRAW);

    GLuint programID = LoadShaders("triangle.vert", "triangle.frag");
    glUseProgram(programID);

    cam_pos = glm::vec3(-static_cast<float>(w_w)/2.0f, -static_cast<float>(w_h)/2.0f, 3.0f);

    //CAMERA SETUP AND CANVAS
    projection = glm::ortho(0.0f, static_cast<float>(w_w), 0.0f, static_cast<float>(w_h), 0.0f, 100.0f);
    view = glm::lookAt(
        cam_pos, // Camera is at (4,3,3), in World Space
        glm::vec3(cam_pos.x, cam_pos.y, 0), // and looks at the origin
        glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
    );
    

    mvp = projection * view;
    MatrixID = glGetUniformLocation(programID, "MVP");
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);


    clear_color = hex_to_rgb("#1D3958");


}

Visualizer::~Visualizer()
{
}

void Visualizer::run()
{
    // Main loop
    while (!glfwWindowShouldClose(window)) {
        // Poll everything (events, texture threads, text areas threads...)


        glBufferData(GL_ARRAY_BUFFER,
            box.model.size() * sizeof(Vertex),
            box.model.data(),
            GL_STATIC_DRAW);


        glfwPollEvents();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(clear_color.r, clear_color.g, clear_color.b, 0);



        glfwGetCursorPos(window, &c_x, &c_y);


        box.check_collision(c_x + cam_pos.x, c_y + cam_pos.y);

        float speed = 1.0f;

        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
            cam_pos -= glm::vec3(0.0f, speed, 0.0f);
            std::cout << "test" << std::endl;
        }

        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
            cam_pos += glm::vec3(speed, 0.0f, 0.0f);
            std::cout << "test" << std::endl;
        }

        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
            cam_pos += glm::vec3(0.0f, speed, 0.0f);
            std::cout << "test" << std::endl;
        }

        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
            cam_pos -= glm::vec3(speed, 0.0f, 0.0f);
            std::cout << "test" << std::endl;
        }


        view = glm::lookAt(
            cam_pos, // Camera is at (4,3,3), in World Space
            glm::vec3(cam_pos.x, cam_pos.y, 0), // and looks at the origin
            glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
        );


        mvp = projection * view;
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);






        //Render
        //// 1st attribute buffer : vertices
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(
            0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
            3,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            6 * sizeof(float),                  // stride
            (void*)0            // array buffer offset
        );

        //// 1st attribute buffer : vertices
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(
            1,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
            3,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            6 * sizeof(float),                  // stride
            (void*)(3 * sizeof(float))           // array buffer offset
        );
        // Draw the triangle !

        glDrawArrays(GL_TRIANGLES, 0, box.model.size()); // Starting from vertex 0; 3 vertices total -> 1 triangle
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);

        glfwSwapBuffers(window);
    }
}

