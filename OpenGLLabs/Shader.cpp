#include "Shader.hpp"

auto Shader::compile_shader(unsigned int type, const std::string& source) -> unsigned int {
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

auto Shader::create_shader(const std::string& vertex_shader, const std::string& fragment_shader) -> unsigned int {
	unsigned int program = glCreateProgram();
	unsigned int vs = compile_shader(GL_VERTEX_SHADER, vertex_shader);
	unsigned int fs = compile_shader(GL_FRAGMENT_SHADER, fragment_shader);

	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}

Shader::Shader(const std::string& vertex_shader_path, const std::string& fragment_shader_path) {
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

auto Shader::get_id() const -> unsigned int {
	return _id;
}

auto Shader::select() const -> void {
	glUseProgram(_id);
}

Shader::operator unsigned int() const {
	return _id;
}
