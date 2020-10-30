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
#include "stb_image.h"


#define WIDTH 1000
#define HEIGHT 1000

class Shader final {
private:
	unsigned int _id;
	auto compile_shader(unsigned int type, const std::string& source) -> unsigned int {
		unsigned int id = glCreateShader(type);
		const char* src = source.c_str();
		int result;
		glShaderSource(id, 1, &src, nullptr);
		glCompileShader(id);

		glGetShaderiv(id, GL_COMPILE_STATUS, &result);
		if (result == GL_FALSE) {
			int length;
			glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
			char* message = (char*)_malloca(length * sizeof(char));
			glGetShaderInfoLog(id, length, &length, message);
			throw std::runtime_error((std::string)"Failed to compile "
				+ (type == GL_VERTEX_SHADER ? "vertex" : "fragment")
				+ " shader!\n"
				+ (std::string)message);
		}

		return id;
	}

	auto create_shader(const std::string& vertex_shader, const std::string& fragment_shader) -> unsigned int {
		unsigned int program =	glCreateProgram();
		unsigned int vs =		compile_shader(GL_VERTEX_SHADER, vertex_shader);
		unsigned int fs =		compile_shader(GL_FRAGMENT_SHADER, fragment_shader);

		glAttachShader(program, vs);
		glAttachShader(program, fs);
		glLinkProgram(program);
		glValidateProgram(program);

		glDeleteShader(vs);
		glDeleteShader(fs);

		return program;
	}
public:
	Shader() :_id(0) {}
	Shader(const std::string& vertex_shader_path, const std::string& fragment_shader_path) {
		std::ifstream vs(vertex_shader_path);
		std::ifstream fs(fragment_shader_path);
		if (!vs.is_open() || !fs.is_open()) {
			throw std::runtime_error("Didn't manage to find shader.");
		}
		else {
			std::string buff, vertex = "", fragment = "";
			while (std::getline(vs, buff)) {
				vertex += buff + '\n';
			} while (std::getline(fs, buff)) {
				fragment += buff + '\n';
			}
			this->_id = create_shader(vertex, fragment);
		}
	}
	auto get_id() -> unsigned int {
		return _id;
	}
	auto select() -> void {
		glUseProgram(_id);
	}
	explicit operator unsigned int() {
		return _id;
	}
};

class Texture final {
private:
	unsigned int _id;
public:
	Texture() :_id(0) {}
	Texture(const std::string& path_to_texture) {
		glActiveTexture(GL_TEXTURE0);
		glGenTextures(1, &_id);
		glBindTexture(GL_TEXTURE_2D, _id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		int width, height, channels;

		unsigned char* data = stbi_load(path_to_texture.c_str(), &width, &height, &channels, 0);
		if (!data) {
			stbi_image_free(data);
			throw std::runtime_error("Didn't manage to lead texture.");
		}
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		stbi_image_free(data);
	}
	auto select() {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, _id);
	}
	explicit operator unsigned int() {
		return _id;
	}
};

class Renderer final {
private:
	unsigned int _VBO;
	unsigned int _VAO;
	unsigned int _dim;
	unsigned int _tex_dim;
	Shader _shader;
	Texture _texture;
	std::vector<float> _data;
public:
	Renderer(const std::vector<float>& data, unsigned int tex_dim, unsigned int dim, const Shader& shader, const Texture& texture) : _dim(dim), _data(data), _shader(shader), _tex_dim(tex_dim), _texture(texture) {
		glGenVertexArrays(1, &_VAO);
		glGenBuffers(1, &_VBO);
		glBindVertexArray(_VAO);

		glBindBuffer(GL_ARRAY_BUFFER, _VBO);
		glBufferData(GL_ARRAY_BUFFER, _data.size() * sizeof(float), _data.data(), GL_STATIC_DRAW);

		auto stride = (_tex_dim + _dim) * sizeof(float);

		glVertexAttribPointer(0, _dim, GL_FLOAT, false, stride, (void*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, _tex_dim, GL_FLOAT, false, stride, (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
	}

	template<class Fn, class... Args>
	auto render(const Fn& func, Args... args) -> void {
		glActiveTexture(GL_TEXTURE0);
		_texture.select();
		_shader.select();
		func((unsigned int)_shader, args...);
		glBindVertexArray(_VAO);
		glDrawArrays(GL_TRIANGLES, 0, _data.size() / (_dim + _tex_dim));
	}
};

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
	srand(time(NULL));
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
		return "../OpenGLLabs/frame_black.jpg"; //-Moge-ge-ge-ge-//
	}
}

auto main() -> int {

	if (!glfwInit())
		return -1;
	/*glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);*/
	auto window = glfwCreateWindow(WIDTH, HEIGHT, "4AM CUBE", NULL, NULL);
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
	try {
		auto shader = Shader("../OpenGLLabs/vertex_shader.shader", "../OpenGLLabs/fragment_shader.shader");
		Renderer renderer(vertices, 2, 3, shader, Texture(get_random_colored_4am_cube(0)));
		//std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		Renderer renderer2(vertices, 2, 3, shader, Texture(get_random_colored_4am_cube(2)));
		Renderer renderer3(vertices, 2, 3, shader, Texture(get_random_colored_4am_cube(8)));

		auto func = [](unsigned int shader, glm::vec3 coords) -> void {
			glm::mat4 model = glm::mat4(1.0f);
			glm::mat4 view = glm::mat4(1.0f);
			glm::mat4 projection = glm::mat4(1.0f);
			model = glm::rotate(model, (float)glfwGetTime() * glm::radians(66.6f), glm::vec3(4.04f, 4.2f, 1.3f));
			view = glm::translate(view, coords);
			projection = glm::perspective(glm::radians(56.6f), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
			unsigned int modelLoc = glGetUniformLocation(shader, "model");
			unsigned int viewLoc = glGetUniformLocation(shader, "view");
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
			glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, &projection[0][0]);
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		};

		while (!glfwWindowShouldClose(window)) {
			glfwMakeContextCurrent(window);
			glEnable(GL_DEPTH_TEST);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			renderer.render(func, glm::vec3(2.2f, -1.5f, -7.0f));
			renderer2.render(func, glm::vec3(-2.2f, -1.5f, -7.0f));
			renderer3.render(func, glm::vec3(0.0f, 1.5f, -7.0f));
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
