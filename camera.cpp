int main () {
	glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f); //Camera position

	glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f); //Camera direction
	glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraTarget);

	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f); //Right axis (or X axis)
	glm::vec3 cameraRight = glm::normalize(gml::cross(up, cameraDirection));

	glm::vec3 cameraUp = gml::cross(cameraDirection, cameraRight); //Up axis (or Y axis)

	glm::mat4 view;
	view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f),
			   glm::vec3(0.0f, 0.0f, 0.0f),
			   glm::vec3(0.0f, 1.0f, 0.0f));
