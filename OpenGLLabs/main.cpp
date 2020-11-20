#define GLEW_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include <iostream>
#include <fstream>
#include <thread>
#include <functional>
#include <ctime>
#include <vector>
#include <string>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include "Shader.hpp"
#include "Texture.hpp"
#include "Renderer.hpp"
#include "Camera.hpp"

#define WIDTH 1000
#define HEIGHT 1000

static std::vector<float> vertices{
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
};

const char* get_random_colored_4am_cube(int var = -1) {
	
	unsigned int random = var == -1? rand() % 26 : (unsigned int)var;
	switch (random){
	case 0: case 1: 
		return "../OpenGLLabs/frame_yellow.jpg";
	case 2: case 3: case 4:
		return "../OpenGLLabs/frame_cian.jpg";
	case 5: case 6: case 7: case 8: 
		return "../OpenGLLabs/frame_magenta.jpg";
	case 9: case 10: case 11: case 12: case 13: case 14: case 15: 
		return "../OpenGLLabs/frame_red.jpg";
	case 16: case 17: case 18: case 19:
		return "../OpenGLLabs/frame_green.jpg";
	case 20: case 21: case 22:
		return "../OpenGLLabs/frame_blue.jpg";
	case 23: case 24:
		return "../OpenGLLabs/frame_white.jpg";
	default:
		return "../OpenGLLabs/frame_black.jpg";
	}
}

auto main() -> int {
	srand(time(NULL));
	if (!glfwInit())
		return -1;

	auto window = glfwCreateWindow(WIDTH, HEIGHT, "4am CUBES", NULL, NULL);
	if (!window) {
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	if (glewInit() != GLEW_OK) {
		std::cout << "Didn't manage to initialize GLEW." << std::endl;;
		return -1;
	}

	std::cout << glGetString(GL_VERSION) << std::endl;
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	try {
		auto shader   = Shader("../OpenGLLabs/vertex_shader.shader", "../OpenGLLabs/fragment_shader.shader");
		auto camera	  = Camera();
		auto texture1 = Texture(get_random_colored_4am_cube(1));
		auto texture2 = Texture(get_random_colored_4am_cube(3));
		auto texture3 = Texture(get_random_colored_4am_cube(7));
		auto texture4 = Texture(get_random_colored_4am_cube(13));
		auto texture5 = Texture(get_random_colored_4am_cube(18));
		auto texture6 = Texture(get_random_colored_4am_cube(21));
		auto texture7 = Texture(get_random_colored_4am_cube(23));

		Renderer renderer(&vertices, 2, 3, &shader, &texture1);
		camera.set_speed(10);
		auto func = [&](unsigned int shader, glm::vec3 coords) -> void {
			glm::mat4 model = glm::mat4(1.0f);
			glm::mat4 view = glm::mat4(1.0f);
			glm::mat4 projection = glm::mat4(1.0f);
			model = glm::rotate(model, (float)glfwGetTime() * glm::radians(66.6f), glm::vec3(4.04f, 4.2f, 1.3f));
			view = glm::translate((glm::mat4)camera, coords);
			projection = glm::perspective(glm::radians(45.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
			glUniform1f(glGetUniformLocation(shader, "time"), glfwGetTime());
			glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, glm::value_ptr(model));
			glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, &view[0][0]);
			glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, &projection[0][0]);
		};

		float deltaTime = 0.0f;	// Time between current frame and last frame
		float lastFrame = 0.0f;
		double x = 0, y = 0;
		glfwSetCursorPos(window, 0.0, 0.0);
		bool cam = true;
		while (!glfwWindowShouldClose(window)) {
			float currentFrame = glfwGetTime();
			deltaTime = currentFrame - lastFrame;
			lastFrame = currentFrame;
			glfwMakeContextCurrent(window);
			glEnable(GL_DEPTH_TEST);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
				cam = !cam;
			if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && cam)
				camera.move_forward(deltaTime);
			if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && cam)
				camera.move_backwards(deltaTime);
			if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS && cam)
				camera.move_left(deltaTime);
			if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS && cam)
				camera.move_right(deltaTime);
			if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && cam)
				camera.move_up(deltaTime);
			if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS && cam)
				camera.move_down(deltaTime);
			if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
				break;

			double sx = x, sy = y;
			glfwGetCursorPos(window, &x, &y);
			if (cam) {
				camera.rotate(x - sx, sy - y);
			}

			renderer.change_texture(&texture7);
			renderer.render(func, glm::vec3(7.0f, 0.0f, 0.0f));
			renderer.change_texture(&texture1);
			renderer.render(func, glm::vec3(4.5f, 0.0f, 5.5f));
			renderer.change_texture(&texture2);
			renderer.render(func, glm::vec3(-1.0f, 0.0f, 7.0f));
			renderer.change_texture(&texture3);
			renderer.render(func, glm::vec3(-6.5f, 0.0f, 3.0f));
			renderer.change_texture(&texture4);
			renderer.render(func, glm::vec3(-6.5f, 0.0f, -3.0f));
			renderer.change_texture(&texture5);
			renderer.render(func, glm::vec3(4.5f, 0.0f, -5.5f));
			renderer.change_texture(&texture6);
			renderer.render(func, glm::vec3(-1.0f, 0.0f, -7.0f));

			glfwSwapBuffers(window);
			glfwPollEvents();
		}
	}
	catch (const std::exception& ex) {
		std::cerr << ex.what() << std::endl;
	}
	glfwTerminate();
	return 0;
}











































//#define GLEW_STATIC
//#define STB_IMAGE_IMPLEMENTATION
//#include <iostream>
//#include <vector>
//#include <string>
//#include <fstream>
//#include <GL/glew.h>
//#include <GLFW/glfw3.h>
//#include <glm.hpp>
//#include <gtc/matrix_transform.hpp>
//#include <gtc/type_ptr.hpp>
//#include "stb_image.h"
//
//class Renderer {
//private:
//    unsigned int _buff_index;
//    unsigned int _dim;
//    std::vector<float> _data;
//
//public:
//    Renderer(std::vector<float>&& data, size_t dim, unsigned int shaders) : _dim(dim), _data(std::move(data)) {
//        glGenBuffers(1, &_buff_index);
//        glBindBuffer(GL_ARRAY_BUFFER, _buff_index);
//        glBufferData(GL_ARRAY_BUFFER, _data.size() * sizeof(float), &_data[0], GL_STATIC_DRAW);
//        glVertexAttribPointer(0, _dim, GL_FLOAT, false, _dim * sizeof(float), 0);
//        glEnableVertexAttribArray(0);
//        glUseProgram(shaders);
//
//
//    }
//
//    auto render() -> void {
//        glClear(GL_COLOR_BUFFER_BIT);
//        glBindBuffer(GL_ARRAY_BUFFER, _buff_index);
//        auto count = static_cast<size_t>(_data.size() / _dim);
//        glDrawArrays(GL_TRIANGLES, 0, count);
//        glDrawElements(GL_TRIANGLES, count, GL_FLOAT, &_data[0]);
//    }
//};
//
//static std::vector<float> verticies{
//    -0.5f, -0.5f, -0.5f,
//     0.5f, -0.5f, -0.5f,
//     0.5f,  0.5f, -0.5f,
//     0.5f,  0.5f, -0.5f,
//    -0.5f,  0.5f, -0.5f,
//    -0.5f, -0.5f, -0.5f,
//    -0.5f, -0.5f,  0.5f,
//     0.5f, -0.5f,  0.5f,
//     0.5f,  0.5f,  0.5f,
//     0.5f,  0.5f,  0.5f,
//    -0.5f,  0.5f,  0.5f,
//    -0.5f, -0.5f,  0.5f,
//    -0.5f,  0.5f,  0.5f,
//    -0.5f,  0.5f, -0.5f,
//    -0.5f, -0.5f, -0.5f,
//    -0.5f, -0.5f, -0.5f,
//    -0.5f, -0.5f,  0.5f,
//    -0.5f,  0.5f,  0.5f,
//     0.5f,  0.5f,  0.5f,
//     0.5f,  0.5f, -0.5f,
//     0.5f, -0.5f, -0.5f,
//     0.5f, -0.5f, -0.5f,
//     0.5f, -0.5f,  0.5f,
//     0.5f,  0.5f,  0.5f,
//    -0.5f, -0.5f, -0.5f,
//     0.5f, -0.5f, -0.5f,
//     0.5f, -0.5f,  0.5f,
//     0.5f, -0.5f,  0.5f,
//    -0.5f, -0.5f,  0.5f,
//    -0.5f, -0.5f, -0.5f,
//    -0.5f,  0.5f, -0.5f,
//     0.5f,  0.5f, -0.5f,
//    -0.5f,  0.5f,  0.5f,
//     0.5f,  0.5f,  0.5f,
//     0.5f,  0.5f,  0.5f,
//    -0.5f,  0.5f, -0.5f
//};
//
//static unsigned int compile_shader( unsigned int type, const std::string& source) {
//    unsigned int id = glCreateShader(type);
//    const char* src = source.c_str();
//    int result;
//
//    glShaderSource(id, 1, &src, nullptr);
//    glCompileShader(id);
//    
//    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
//    if (result == GL_FALSE) {
//        int length;
//        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
//        char* message = (char*)alloca(length * sizeof(char));
//        glGetShaderInfoLog(id, length, &length, message);
//        std::cerr << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!" << std::endl;
//        std::cerr << message << std::endl;
//        return 0;
//    }
//
//    return id;
//}
//
//static unsigned int create_shader(const std::string& vertex_shader, const std::string& fragment_shader) {
//    unsigned int program = glCreateProgram();
//    unsigned int vs = compile_shader(GL_VERTEX_SHADER, vertex_shader);
//    unsigned int fs = compile_shader(GL_FRAGMENT_SHADER, fragment_shader);
//
//    glAttachShader(program, vs);
//    glAttachShader(program, fs);
//    glLinkProgram(program);
//    glValidateProgram(program);
//
//    glDeleteShader(vs);
//    glDeleteShader(fs);
//
//    return program;
//}
//
//int main() {
//    if (!glfwInit())
//        return -1;
//
//    auto window = glfwCreateWindow(600, 600, "Hello World", NULL, NULL);
//    if (!window) {
//        glfwTerminate();
//        return -1;
//    }
//    glfwMakeContextCurrent(window);
//    if (glewInit() != GLEW_OK) {
//        std::cout << "Didn't manage to initialize GLEW." << std::endl;;
//        return -1;
//    }
//    std::cout << glGetString(GL_VERSION) << std::endl;
//
//    std::string fragment_shader, vertex_shader, tmp;
//    std::ifstream f_shader("../OpenGLLabs/fragment_shader.shader");
//    std::ifstream v_shader("../OpenGLLabs/vertex_shader.shader");
//    
//    while (std::getline(f_shader, tmp)) {
//        fragment_shader += tmp + '\n';
//        tmp.clear();
//    }
//    while (std::getline(v_shader, tmp)) {
//        vertex_shader += tmp + '\n';
//        tmp.clear();
//    }
//
//    unsigned int texture;
//    glGenTextures(1, &texture);
//    glBindTexture(GL_TEXTURE_2D, texture);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//    int width, height, nrChannels;
//    /*std::fstream f("../OpenGLLabs/frame.jpg");
//    std::cout << f.is_open() << std::endl;*/
//    unsigned char* image = stbi_load("../OpenGLLabs/frame.jpg", &width, &height, &nrChannels, 0);
//    glBindTexture(GL_TEXTURE_2D, texture);
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
//    glGenerateMipmap(GL_TEXTURE_2D);
//    stbi_image_free(image);
//    glActiveTexture(GL_TEXTURE0);
//    glBindTexture(GL_TEXTURE_2D, texture);
//
//    unsigned int shader = create_shader(vertex_shader, fragment_shader);
//    Renderer r(std::move(verticies), 3, shader);
//
//    while (!glfwWindowShouldClose(window)) {
//        glm::mat4 trans = glm::mat4(1.0f);
//        trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 1.0f));
//        unsigned int transformLoc = glGetUniformLocation(shader, "transform");
//        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));
//        r.render();
//        glfwSwapBuffers(window);
//        glfwPollEvents();
//    }
//
//    glfwTerminate();
//    return 0;
//}
//
