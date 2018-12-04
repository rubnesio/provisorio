#include <Windows.h>
#define STB_IMAGE_IMPLEMENTATION
#include "Shader.h"
#include "stb_image.h"
#include <stdio.h>
#include <GL/glew.h> /* include GLEW and new version of GL on Windows */
#include <GLFW/glfw3.h> /* GLFW helper library */
#include <GLM/glm.hpp>
#include <GLM/vec2.hpp>
#include <GLM/vec3.hpp>
#include <GLM/vec4.hpp>
#include <GLM/mat4x4.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>
#include <Soil.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void processInput(GLFWwindow *window, glm::vec3& position, glm::vec3& rotation, glm::vec3& scale);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void conversao(double &x, double &y);
void gerarCurva();

// settings
const unsigned int SCR_WIDTH = 860;
const unsigned int SCR_HEIGHT = 640;
boolean des = false;
boolean fim = false;
vector<GLfloat>* vert = new vector<GLfloat>();
vector<GLint>* indi = new vector<GLint>();
vector<GLint>* curva = new vector<GLint>();
unsigned int VBO, VAO, VBO2, VAO2;

int main()
{
	
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

														 // glfw window creation
														 // --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

	glfwMakeContextCurrent(window);
	glewExperimental = GL_TRUE;
	glewInit();

	glfwSetMouseButtonCallback(window, mouse_button_callback);

	int framebufferWidth = 0;
	int framebufferHeight = 0;

	glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	const char* vertexShaderSource =
		"#version 410 core\n"
		"layout(location = 0) in vec3 aPos;"
		"layout (location = 1) in vec3 aTexCoord;"
		"layout (location = 2) in vec3 aNormal;"
		"out vec3 ourPos;"
		"out vec3 TexCoord;"
		"out vec3 ourNormal;"
		"uniform mat4 ModelMatrix;"
		"uniform mat4 ViewMatrix;"
		"uniform mat4 ProjectionMatrix;"
		"void main() {"
		"   ourPos = vec4(ModelMatrix * vec4(aPos, 1.f)).xyz;"
		"   TexCoord = aTexCoord;"
		"   ourNormal = mat3(ModelMatrix) * aNormal;"
		"   gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(aPos, 1.f);"
		"}";

	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);

	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	GLint sucess;

	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &sucess);
	if (!sucess) {
		cout << "Error Vertex";
	}

	const char* fragmentShaderSource =
		"#version 410 core\n"
		"in vec3 ourPos;"
		"in vec3 TexCoord;"
		"in vec3 ourNormal;"
		"out vec4 FragColor;"
		"uniform sampler2D texture1;"
		"uniform sampler2D texture2;"
		"uniform vec3 lightPos0;"
		"uniform vec3 cameraPos;"
		"uniform vec3 kambiente;"
		"uniform vec3 kdifusao;"
		"uniform vec3 kespecular;"
		"uniform vec3 shiny;"
		"void main()	{"
		"   vec3 ambientLight = kambiente;"			//ambiente
		"   vec3 posToLightDirVec = normalize(lightPos0 - ourPos);"	//difusa
		"   vec3 diffuseColor = kdifusao;"
		"   float diffuse = clamp(dot(posToLightDirVec, ourNormal), 0, 1);"
		"   vec3 diffuseFinal = diffuseColor * diffuse;"
		"   vec3 lightToPosDirVec = normalize(ourPos - lightPos0);"		//especular
		"   vec3 reflectDirVec = normalize(reflect(lightToPosDirVec, normalize(ourNormal)));"
		"   vec3 posToViewDirVec = normalize(cameraPos - ourPos );"
		"   float specularConstant = pow(max(dot(posToViewDirVec, reflectDirVec), 0), shiny.x);"
		"   vec3 specularFinal = kespecular * specularConstant;"
		"   FragColor =vec4(TexCoord ,1.0f) * (vec4(ambientLight, 1.f) + vec4(diffuseFinal, 1.0f) + vec4(specularFinal, 1.f));"	//saida do resultado
		"}";

	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &sucess);
	if (!sucess) {
		cout << "Error Fragment";
	}

	int shaderProgram;
	shaderProgram = glCreateProgram();

	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);



	
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenVertexArrays(2, &VAO2);
	glGenBuffers(2, &VBO2);
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	

	glm::vec3 position(0.f);
	glm::vec3 rotation(0.f);
	glm::vec3 scale(1.f);

	glm::mat4 ModelMatrix(1.f);
	ModelMatrix = glm::translate(ModelMatrix, position);
	ModelMatrix = glm::rotate(ModelMatrix, glm::radians(rotation.x), glm::vec3(1.f, 0.f, 0.f));
	ModelMatrix = glm::rotate(ModelMatrix, glm::radians(rotation.y), glm::vec3(0.f, 1.f, 0.f));
	ModelMatrix = glm::rotate(ModelMatrix, glm::radians(rotation.z), glm::vec3(0.f, 0.f, 1.f));
	ModelMatrix = glm::scale(ModelMatrix, scale);

	glm::vec3 camPosition(0.f, 0.f, 1.f);
	glm::vec3 worldUp(0.f, 1.f, 0.f);
	glm::vec3 camFront(0.f, 0.f, -1.f);

	glm::mat4 ViewMatrix(1.f);
	ViewMatrix = glm::lookAt(camPosition, camPosition + camFront, worldUp);

	float fov = 90.f;
	float nearPlane = 0.1f;
	float farPlane = 1000.f;
	glm::mat4 ProjectionMatrix(1.f);

	//ProjectionMatrix = glm::perspective(glm::radians(fov), static_cast<float>(framebufferWidth) / framebufferHeight, nearPlane, farPlane);

	ProjectionMatrix = glm::ortho(0.0f, 860.0f, 640.0f, 0.0f);

	glm::vec3 lightPos0(0.0f, 0.f, 1.f);
	glm::vec3 kambiente(0.5, 0.5, 0.5);
	glm::vec3 kdifusao(1.0, 1.0, 1.0);
	glm::vec3 kespecular(1.0, 1.0, 1.0);
	glm::vec3 shiny(300.1000, 0.0, 0.0);

	glUseProgram(shaderProgram);

	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "ModelMatrix"), 1, GL_FALSE, glm::value_ptr(ModelMatrix));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "ViewMatrix"), 1, GL_FALSE, glm::value_ptr(ViewMatrix));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "ProjectionMatrix"), 1, GL_FALSE, glm::value_ptr(ProjectionMatrix));

	glUniform3fv(glGetUniformLocation(shaderProgram, "lightPos0"), 1, glm::value_ptr(lightPos0));
	glUniform3fv(glGetUniformLocation(shaderProgram, "kambiente"), 1, glm::value_ptr(kambiente));
	glUniform3fv(glGetUniformLocation(shaderProgram, "kdifusao"), 1, glm::value_ptr(kdifusao));
	glUniform3fv(glGetUniformLocation(shaderProgram, "kespecular"), 1, glm::value_ptr(kespecular));
	glUniform3fv(glGetUniformLocation(shaderProgram, "cameraPos"), 1, glm::value_ptr(camPosition));
	glUniform3fv(glGetUniformLocation(shaderProgram, "shiny"), 1, glm::value_ptr(shiny));

	glUseProgram(0);

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// input
		// -----
		processInput(window);
		processInput(window, position, rotation, scale);

		// render
		// ------
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// render the triangle
		//ourShader->use();
		glUseProgram(shaderProgram);

		ModelMatrix = glm::mat4(1.f);
		ModelMatrix = glm::translate(ModelMatrix, position);
		ModelMatrix = glm::rotate(ModelMatrix, glm::radians(rotation.x), glm::vec3(1.f, 0.f, 0.f));
		ModelMatrix = glm::rotate(ModelMatrix, glm::radians(rotation.y), glm::vec3(0.f, 1.f, 0.f));
		ModelMatrix = glm::rotate(ModelMatrix, glm::radians(rotation.z), glm::vec3(0.f, 0.f, 1.f));
		ModelMatrix = glm::scale(ModelMatrix, scale);

		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "ModelMatrix"), 1, GL_FALSE, glm::value_ptr(ModelMatrix));

		glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);

		ProjectionMatrix = glm::mat4(1.f);
		ProjectionMatrix = glm::perspective(glm::radians(fov), static_cast<float>(framebufferWidth) / framebufferHeight, nearPlane, farPlane);

		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "ProjectionMatrix"), 1, GL_FALSE, glm::value_ptr(ProjectionMatrix));
				
		if (des) {
			glBindVertexArray(VAO);
			glPointSize(15);
			glDrawArrays(GL_POINTS, 0, vert->size() / 9);
		}
		if (fim) {
			//cout << "teste";
			glBindVertexArray(VAO2);
			glDrawArrays(GL_LINE_STRIP, 0, curva->size() / 9);
			//glDrawArrays();
		}
		
		

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
		glBindVertexArray(0);
		glUseProgram(0);
		
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(2, &VAO2);
	glDeleteBuffers(2, &VBO2);

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	//int i;
	cout << "teste";
	//cin >> i;
	return 0;
}

void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window, glm::vec3& position, glm::vec3& rotation, glm::vec3& scale)
{
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		position.z += 0.01f;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		position.z -= 0.01f;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		position.x += 0.01f;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		position.x -= 0.01f;
	}
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
		rotation.y -= 1.f;
	}
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
		rotation.y += 1.f;
	}
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
		rotation.x -= 1.f;
	}
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
		rotation.x += 1.f;
	}
	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
		scale += 0.1f;
	}
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
		scale -= 0.1f;
	}
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
		fim = TRUE;
		gerarCurva();
		glBindVertexArray(VAO2);

		glBindBuffer(GL_ARRAY_BUFFER, VBO2);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*curva->size(), &curva->at(0), GL_STATIC_DRAW);

		// position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);
		// color attribute
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
		glEnableVertexAttribArray(2);



		// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
		// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
		//glBindVertexArray(0);
	}
}

void conversao(double &x, double &y) {
	if (x > (SCR_WIDTH / 2)) {
		x = x - (SCR_WIDTH / 2);
		x = x / (SCR_WIDTH / 2);
		x = x + (x*0.35);
	}
	else if (x == (SCR_WIDTH / 2)) {
		x = 0;
	}
	else {
		x = (-(((SCR_WIDTH / 2) - x) / (SCR_WIDTH / 2)));
		x = x+(x*0.35);
	}

	if (y > (SCR_HEIGHT / 2)) {
		y = y - (SCR_HEIGHT / 2);
		y = y / (SCR_HEIGHT / 2);
		y = y * (-1);
	}
	else if (y == (SCR_HEIGHT / 2)) {
		y = 0;
	}
	else {
		y = -(((SCR_HEIGHT / 2) - y) / (SCR_HEIGHT / 2));
		y = y * (-1);
	}
}


void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		
		conversao(xpos, ypos);
		
		float x = xpos;
		float y = ypos;
	

		vert->push_back(x);
		vert->push_back(y);
		vert->push_back(0.0);
		vert->push_back(1.0);
		vert->push_back(1.0);
		vert->push_back(1.0);
		vert->push_back(0.0);
		vert->push_back(0.0);
		vert->push_back(1.0);
		
		des = true;
		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*vert->size(), &vert->at(0), GL_STATIC_DRAW);

		// position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		// color attribute
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);

		//glBindVertexArray(0);
	}

}

void gerarCurva() {
	vector<float>* aux = new vector<float>();
	//cout << vert->at(0) << " " << vert->at(18);
	for (int i = 0; i < vert->size()/9; i++) {
		aux->push_back(vert->at(9 * i + 0));
		aux->push_back(vert->at(9 * i + 1));
		aux->push_back(vert->at(9 * i + 2));
	}
	//cout << aux->at(0) << " " << aux->at(6) << " ";

	aux->push_back(vert->at(0));
	aux->push_back(vert->at(1));
	aux->push_back(vert->at(2));
	for (int i = 0; i < ((aux->size()/3) - 3); i++)  {
		for (float t = 0; t <= 1; t += 0.05) {
			GLfloat x = (((-1 * pow(t, 3) + 3 * pow(t, 2) - 3 * t + 1)*aux->at(3 * i) +
				(3 * pow(t, 3) - 6 * pow(t, 2) + 0 * t + 4)*aux->at(3 * (i + 1)) +
				(-3 * pow(t, 3) + 3 * pow(t, 2) + 3 * t + 1)*aux->at(3 * (i + 2)) +
				(1 * pow(t, 3) + 0 * pow(t, 2) + 0 * t + 0)*aux->at(3 * (i + 3))) / 6);

			GLfloat y = (((-1 * pow(t, 3) + 3 * pow(t, 2) - 3 * t + 1)*aux->at((3 * i)+1) +
				(3 * pow(t, 3) - 6 * pow(t, 2) + 0 * t + 4)*aux->at((3 * (i + 1))+1) +
				(-3 * pow(t, 3) + 3 * pow(t, 2) + 3 * t + 1)*aux->at((3 * (i + 2))+1) +
				(1 * pow(t, 3) + 0 * pow(t, 2) + 0 * t + 0)*aux->at((3 * (i + 3)))+1) / 6);

		/*	GLfloat z = (((-1 * pow(t, 3) + 3 * pow(t, 2) - 3 * t + 1)*aux->at((3 * i) + 2) +
				(3 * pow(t, 3) - 6 * pow(t, 2) + 0 * t + 4)*aux->at((3 * (i + 1)) + 2) +
				(-3 * pow(t, 3) + 3 * pow(t, 2) + 3 * t + 1)*aux->at((3 * (i + 2)) + 2) +
				(1 * pow(t, 3) + 0 * pow(t, 2) + 0 * t + 0)*aux->at((3 * (i + 3))) + 2) / 6);*/
			cout << x << " " << y << "\n"; 
			curva->push_back(x);
			curva->push_back(y);
			curva->push_back(0.0); 
			curva->push_back(1.0);
			curva->push_back(1.0);
			curva->push_back(1.0);
			curva->push_back(0.0);
			curva->push_back(0.0);
			curva->push_back(1.0);
		}
	}

	
}

