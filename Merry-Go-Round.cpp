// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <windows.h>
#include <math.h>
// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace glm;
#include <common/shader.hpp>
#include <common/texture.hpp>
#include <common/objloader.hpp>

// ����ȸ���ӵ�
float rotSpeed = 0.0f;
// �������Ʒ��ݺ��ӵ�
float upDownSpeed = 0.5f;
// ȸ����� ����
float gOrientation = 0.0f;
// �� �� ���Ʒ� �������߰��� ���� ����
float upAndDown1 = -2.5f;
float upAndDown2 = 0.0f;
bool updownFlag1 = true;
bool updownFlag2 = false;

// space�� �������� Ȯ��
bool spaceFlag = false;

//���� ��ġ
vec3 lightPos(0.0f, 0.0f, 0.0f);

// ī�޶� �̵��� ���� ��������
// �ʱ� ���� ����
mat4 View;
vec3 cameraPos = vec3(27, 20, -13);
float cameraHorizontalAngle = -1.13f;
float cameraVerticalAngle = -0.6f;

// space �Է½� ȸ���� ���� �Լ�
void spin();
// ī�޶� �̵��� ���� �Լ�
void cameraMove();

int main(void)
{
	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(1024, 768, "Merry-Go-Round", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glewExperimental = true;

	// Initialize GLEW
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// dark gray background
	glClearColor(0.2f, 0.2f, 0.2f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	GLuint programID = LoadShaders("TransformVertexShader.vertexshader", "TextureFragmentShader.fragmentshader");

	// �ʿ��� �ؽ��ĺҷ�����
	GLuint Texture_tail = loadBMP_custom("texture/tail.bmp");
	GLuint Texture_body = loadBMP_custom("texture/body.bmp");
	GLuint Texture_sit = loadBMP_custom("texture/sit.bmp");
	GLuint Texture_leg = loadBMP_custom("texture/leg.bmp");
	GLuint Texture_neck = loadBMP_custom("texture/neck.bmp");
	GLuint Texture_head = loadBMP_custom("texture/head.bmp");
	GLuint Texture_nose = loadBMP_custom("texture/nose.bmp");

	// ���̴����� �� ��������
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");
	GLuint TextureCheckID = glGetUniformLocation(programID, "textureCheck");
	GLuint TextureID = glGetUniformLocation(programID, "myTextureSampler");
	GLint modelMatrixID = glGetUniformLocation(programID, "model");
	GLint viewMatrixID = glGetUniformLocation(programID, "view");
	GLint projMatrixID = glGetUniformLocation(programID, "projection");
	GLint lightColorID = glGetUniformLocation(programID, "lightColor");
	GLint lightPosID = glGetUniformLocation(programID, "lightPos");
	GLint viewPosID = glGetUniformLocation(programID, "viewPos");

	// ť�� ������Ʈ
	std::vector<vec3> cubeVertices;
	std::vector<vec2> cubeUvs;
	std::vector<vec3> cubeNormals;
	bool res = loadOBJ("cube.obj", cubeVertices, cubeUvs, cubeNormals);
	// ť�� ��������
	GLuint cubeVertexbuffer;
	glGenBuffers(1, &cubeVertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, cubeVertices.size() * sizeof(vec3), &cubeVertices[0], GL_STATIC_DRAW);
	//ť�� UV����
	GLuint cubeUvbuffer;
	glGenBuffers(1, &cubeUvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, cubeUvbuffer);
	glBufferData(GL_ARRAY_BUFFER, cubeUvs.size() * sizeof(vec2), &cubeUvs[0], GL_STATIC_DRAW);
	//ť�� �븻����
	GLuint cubeNormalbuffer;
	glGenBuffers(1, &cubeNormalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, cubeNormalbuffer);
	glBufferData(GL_ARRAY_BUFFER, cubeNormals.size() * sizeof(vec3), &cubeNormals[0], GL_STATIC_DRAW);

	// ���� ������Ʈ
	std::vector<vec3> cylinderVertices;
	std::vector<vec2> cylinderUvs;
	std::vector<vec3> cylinderNormals;
	bool res2 = loadOBJ("cylinder.obj", cylinderVertices, cylinderUvs, cylinderNormals);
	// ���� ��������
	GLuint cylinderVertexbuffer;
	glGenBuffers(1, &cylinderVertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, cylinderVertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, cylinderVertices.size() * sizeof(vec3), &cylinderVertices[0], GL_STATIC_DRAW);
	// ���� UV����
	GLuint cylinderUvbuffer;
	glGenBuffers(1, &cylinderUvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, cylinderUvbuffer);
	glBufferData(GL_ARRAY_BUFFER, cylinderUvs.size() * sizeof(vec2), &cylinderUvs[0], GL_STATIC_DRAW);
	// ���� �븻����
	GLuint cylinderNormalbuffer;
	glGenBuffers(1, &cylinderNormalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, cylinderNormalbuffer);
	glBufferData(GL_ARRAY_BUFFER, cylinderNormals.size() * sizeof(vec3), &cylinderNormals[0], GL_STATIC_DRAW);

	// �� ������Ʈ
	std::vector<vec3> sphereVertices;
	std::vector<vec2> sphereUvs;
	std::vector<vec3> sphereNormals;
	bool res3 = loadOBJ("sphere.obj", sphereVertices, sphereUvs, sphereNormals);
	// �� ��������
	GLuint sphereVertexbuffer;
	glGenBuffers(1, &sphereVertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, sphereVertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sphereVertices.size() * sizeof(vec3), &sphereVertices[0], GL_STATIC_DRAW);
	// �� Uv����
	GLuint sphereUvbuffer;
	glGenBuffers(1, &sphereUvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, sphereUvbuffer);
	glBufferData(GL_ARRAY_BUFFER, sphereUvs.size() * sizeof(vec2), &sphereUvs[0], GL_STATIC_DRAW);
	// �� �븻����
	GLuint sphereNormalbuffer;
	glGenBuffers(1, &sphereNormalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, sphereNormalbuffer);
	glBufferData(GL_ARRAY_BUFFER, sphereNormals.size() * sizeof(vec3), &sphereNormals[0], GL_STATIC_DRAW);

	// �ﰢ�� ������Ʈ
	static const GLfloat triangleVertexbuffer_data[] = {
		0.0f, 0.0f, 0.5f,
		0.0f, 2.0f, 0.0f,
		-0.5f, 0.0f, 0.0f,
		-0.5f, 0.0f, 0.0f,
		0.0f, 2.0f, 0.0f,
		0.0f, 0.0f, -0.5f,
		0.0f, 0.0f, -0.5f,
		0.0f, 2.0f, 0.0f,
		0.0f, 0.0f, 0.5f,
		0.0f, 0.0f, -0.5f,
		-0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.5f,
	};
	// �ﰢ�� ��������
	GLuint triangleVertexbuffer;
	glGenBuffers(1, &triangleVertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, triangleVertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertexbuffer_data), triangleVertexbuffer_data, GL_STATIC_DRAW);

	// projection ��Ʈ����
	mat4 Projection = perspective(radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);

	do {
		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// Use our shader
		glUseProgram(programID);

		// spacebar ���������� flag �ٲ��ֱ�
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
			spaceFlag ? spaceFlag = false : spaceFlag = true;
		// ȸ���� ȸ����Ű�� �Լ�
		spin();

		//ī�޶� �̵��Լ�
		cameraMove();


		// Draw center
		mat4 TranslationMatrix_center = translate(mat4(), vec3(0.0f, 0.0f, 0.0f));
		mat4 RotationMatrix_center = eulerAngleYXZ(gOrientation, 0.0f, 0.0f);
		mat4 ScalingMatrix_center = scale(mat4(), vec3(0.05f, 0.05f, 0.05f));

		mat4 Model_center = mat4(1.0f);
		Model_center = TranslationMatrix_center * RotationMatrix_center * ScalingMatrix_center;
		mat4 MVP_center = Projection * View * Model_center;
		// ���̴��� �ʿ� data �����ֱ�
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP_center[0][0]);
		glUniform1i(TextureCheckID, 2);
		glUniform1i(TextureID, 0);


		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, sphereVertexbuffer);
		glVertexAttribPointer(
			0,                  
			3,                  
			GL_FLOAT,          
			GL_FALSE,         
			0,                   
			(void*)0          
		);
		glDrawArrays(GL_TRIANGLES, 0, cylinderVertices.size());

		// �ݺ����� ����Ͽ� 60�� ���� �ϳ��� �񸶿� ������ ��ġ
		for (int i = 0; i < 6; i++) {
			// Draw pillar
			mat4 TranslationMatrix_pillar = translate(mat4(), vec3(0.0f, 0.0f, 12.0f));
			mat4 RotationMatrix_pillar = RotationMatrix_center * eulerAngleYXZ((float)(i * (3.14159f * 60.0 /180.0)), 0.0f, 0.0f);
			mat4 ScalingMatrix_pillar = scale(mat4(), vec3(0.01f, 0.3f, 0.01f));

			//ȸ���ϸ� ���� �񸶿� �Բ� �̵�
			lightPos.x = 12.0 * sin((i * (3.14159f * 60.0 / 180.0)) + gOrientation);
			lightPos.z = 12.0 * cos((i * (3.14159f * 60.0 / 180.0)) + gOrientation);

			// ������ ��� obj Model �������� RotationMatrix_pillar �� ���Ͽ� ���� ���� ȸ���ϵ��� �����
			mat4 Model_pillar =  RotationMatrix_pillar * TranslationMatrix_pillar * ScalingMatrix_pillar;
			mat4 MVP_pillar = Projection * View * Model_pillar;

			glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP_pillar[0][0]);
			glUniform1i(TextureCheckID, 1);
			glUniform1i(TextureID, 0);

			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, cylinderVertexbuffer);
			glVertexAttribPointer(
				0,                  
				3,                  
				GL_FLOAT,          
				GL_FALSE,         
				0,                   
				(void*)0          
			);
			glDrawArrays(GL_TRIANGLES, 0, cylinderVertices.size());

			// Draw Body
			mat4 TranslationMatrix_body;
			// ���Ʒ� �����̴� ������ 2���� ���� �����ư��� ��ġ
			if (i % 2 == 0)
				TranslationMatrix_body = translate(mat4(), vec3(0.0f, upAndDown1, 0.0f)) * TranslationMatrix_pillar;
			else
				TranslationMatrix_body = translate(mat4(), vec3(0.0f, upAndDown2, 0.0f)) * TranslationMatrix_pillar;
			mat4 RotationMatrix_body = eulerAngleYXZ(0.0f, 0.0f, -1.5f);
			mat4 ScalingMatrix_body = scale(mat4(), vec3(0.1f, 0.1f, 0.1f));

			mat4 Model_body = RotationMatrix_pillar * TranslationMatrix_body * RotationMatrix_body * ScalingMatrix_body;
			mat4 MVP_body = Projection * View * Model_body;

			glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP_body[0][0]);
			glUniformMatrix4fv(viewMatrixID, 1, GL_FALSE, &View[0][0]);
			glUniformMatrix4fv(projMatrixID, 1, GL_FALSE, &Projection[0][0]);
			glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, &Model_body[0][0]);

			glUniform1i(TextureCheckID, 0);
			glUniform1i(TextureID, 0);
			// �ѹ��� �����ָ� ��
			glUniform3f(lightColorID, 1.0f, 1.0f, 1.0f);
			glUniform3f(lightPosID, lightPos.x, 15.0f, lightPos.z);
			glUniform3f(viewPosID, cameraPos.x, cameraPos.y, cameraPos.z);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, Texture_body);

			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, cylinderVertexbuffer);
			glVertexAttribPointer(
				0,                  
				3,                  
				GL_FLOAT,          
				GL_FALSE,         
				0,                   
				(void*)0          
			);
			glEnableVertexAttribArray(1);
			glBindBuffer(GL_ARRAY_BUFFER, cylinderUvbuffer);
			glVertexAttribPointer(
				1,                                
				2,                                
				GL_FLOAT,                        
				GL_FALSE,                       
				0,                                 
				(void*)0                        
			);
			glEnableVertexAttribArray(2);
			glBindBuffer(GL_ARRAY_BUFFER, cylinderNormalbuffer);
			glVertexAttribPointer(
				2,
				3,
				GL_FLOAT,
				GL_FALSE,
				0,
				(void*)0
			);
			glDrawArrays(GL_TRIANGLES, 0, cylinderVertices.size());

			// Draw sit
			mat4 TranslationMatrix_sit = translate(mat4(), vec3(-0.5f, 1.0f, 0.0f)) * TranslationMatrix_body;
			mat4 RotationMatrix_sit = eulerAngleYXZ(0.0f, 0.0f, 0.0f) * RotationMatrix_body;
			mat4 ScalingMatrix_sit = scale(mat4(), vec3(0.3f, 1.0f, 0.8f));

			mat4 Model_sit = RotationMatrix_pillar * TranslationMatrix_sit * RotationMatrix_sit * ScalingMatrix_sit;
			mat4 MVP_sit = Projection * View * Model_sit;
			// Send our transformation to the currently bound shader, 
			glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP_sit[0][0]);
			glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, &Model_sit[0][0]);
			glUniform1i(TextureCheckID, 0);
			glUniform1i(TextureID, 0);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, Texture_sit);

			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, cubeVertexbuffer);
			glVertexAttribPointer(
				0,                  
				3,                  
				GL_FLOAT,          
				GL_FALSE,         
				0,                   
				(void*)0          
			);
			glEnableVertexAttribArray(1);
			glBindBuffer(GL_ARRAY_BUFFER, cubeUvbuffer);
			glVertexAttribPointer(
				1,                                
				2,                                
				GL_FLOAT,                        
				GL_FALSE,                       
				0,                                 
				(void*)0                        
			);
			glEnableVertexAttribArray(2);
			glBindBuffer(GL_ARRAY_BUFFER, cubeNormalbuffer);
			glVertexAttribPointer(
				2,
				3,
				GL_FLOAT,
				GL_FALSE,
				0,
				(void*)0
			);
			glDrawArrays(GL_TRIANGLES, 0, cubeVertices.size());

			// Draw neck
			mat4 TranslationMatrix_neck = translate(mat4(), vec3(1.8f, 1.5f, 0.0f)) * TranslationMatrix_body;
			mat4 RotationMatrix_neck = eulerAngleYXZ(0.0f, 0.0f, 1.0f) * RotationMatrix_body;
			mat4 ScalingMatrix_neck = scale(mat4(), vec3(0.5f, 1.2f, 0.5f));

			mat4 Model_neck = RotationMatrix_pillar * TranslationMatrix_neck * RotationMatrix_neck * ScalingMatrix_neck;
			mat4 MVP_neck = Projection * View * Model_neck;

			glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP_neck[0][0]);
			glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, &Model_neck[0][0]);
			glUniform1i(TextureCheckID, 0);
			glUniform1i(TextureID, 0);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, Texture_neck);

			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, cubeVertexbuffer);
			glVertexAttribPointer(
				0,                  
				3,                  
				GL_FLOAT,          
				GL_FALSE,         
				0,                   
				(void*)0          
			);
			glEnableVertexAttribArray(1);
			glBindBuffer(GL_ARRAY_BUFFER, cubeUvbuffer);
			glVertexAttribPointer(
				1,                                
				2,                                
				GL_FLOAT,                        
				GL_FALSE,                       
				0,                                 
				(void*)0                        
			);
			glEnableVertexAttribArray(2);
			glBindBuffer(GL_ARRAY_BUFFER, cubeNormalbuffer);
			glVertexAttribPointer(
				2,
				3,
				GL_FLOAT,
				GL_FALSE,
				0,
				(void*)0
			);
			glDrawArrays(GL_TRIANGLES, 0, cubeVertices.size());

			// Draw head
			mat4 TranslationMatrix_head = translate(mat4(), vec3(0.75f, 1.2f, 0.0f)) * TranslationMatrix_neck;
			mat4 RotationMatrix_head = eulerAngleYXZ(0.0f, 0.0f, 0.0f) * RotationMatrix_neck;
			mat4 ScalingMatrix_head = scale(mat4(), vec3(0.04f, 0.04f, 0.03f));

			mat4 Model_head = RotationMatrix_pillar * TranslationMatrix_head * RotationMatrix_head * ScalingMatrix_head;
			mat4 MVP_head = Projection * View * Model_head;

			glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP_head[0][0]);
			glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, &Model_head[0][0]);
			glUniform1i(TextureCheckID, 0);
			glUniform1i(TextureID, 0);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, Texture_head);

			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, sphereVertexbuffer);
			glVertexAttribPointer(
				0,                  
				3,                  
				GL_FLOAT,          
				GL_FALSE,         
				0,                   
				(void*)0          
			);
			glEnableVertexAttribArray(1);
			glBindBuffer(GL_ARRAY_BUFFER, sphereUvbuffer);
			glVertexAttribPointer(
				1,                                
				2,                                
				GL_FLOAT,                        
				GL_FALSE,                       
				0,                                 
				(void*)0                        
			);
			glEnableVertexAttribArray(2);
			glBindBuffer(GL_ARRAY_BUFFER, sphereNormalbuffer);
			glVertexAttribPointer(
				2,
				3,
				GL_FLOAT,
				GL_FALSE,
				0,
				(void*)0
			);
			glDrawArrays(GL_TRIANGLES, 0, sphereVertices.size());

			// Draw nose
			mat4 TranslationMatrix_nose = translate(mat4(), vec3(0.75f, -0.6f, 0.0f)) * TranslationMatrix_head;
			mat4 RotationMatrix_nose = eulerAngleYXZ(0.0f, 0.0f, 1.2f) * RotationMatrix_head;
			mat4 ScalingMatrix_nose = scale(mat4(), vec3(0.4f, 0.8f, 0.4f));

			mat4 Model_nose = RotationMatrix_pillar * TranslationMatrix_nose * RotationMatrix_nose * ScalingMatrix_nose;
			mat4 MVP_nose = Projection * View * Model_nose;

			glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP_nose[0][0]);
			glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, &Model_nose[0][0]);
			glUniform1i(TextureCheckID, 0);
			glUniform1i(TextureID, 0);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, Texture_nose);

			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, cubeVertexbuffer);
			glVertexAttribPointer(
				0,                  
				3,                  
				GL_FLOAT,          
				GL_FALSE,         
				0,                   
				(void*)0          
			);
			glEnableVertexAttribArray(1);
			glBindBuffer(GL_ARRAY_BUFFER, cubeUvbuffer);
			glVertexAttribPointer(
				1,                                
				2,                                
				GL_FLOAT,                        
				GL_FALSE,                       
				0,                                 
				(void*)0                        
			);
			glEnableVertexAttribArray(2);
			glBindBuffer(GL_ARRAY_BUFFER, cubeNormalbuffer);
			glVertexAttribPointer(
				2,
				3,
				GL_FLOAT,
				GL_FALSE,
				0,
				(void*)0
			);
			glDrawArrays(GL_TRIANGLES, 0, cubeVertices.size());

			// Draw ears
			mat4 TranslationMatrix_ear1 = translate(mat4(), vec3(0.0f, 0.4f, 0.3f)) * TranslationMatrix_head;
			mat4 TranslationMatrix_ear2 = translate(mat4(), vec3(0.0f, 0.4f, -0.3f)) * TranslationMatrix_head;
			mat4 RotationMatrix_ear1 = eulerAngleYXZ(-0.4f, -0.2f, -0.3f) * RotationMatrix_head;
			mat4 RotationMatrix_ear2 = eulerAngleYXZ(0.4f, 0.2f, -0.3f) * RotationMatrix_head;
			mat4 ScalingMatrix_ear = scale(mat4(), vec3(0.5f, 0.5f, 0.5f));

			mat4 Model_ear1 = RotationMatrix_pillar * TranslationMatrix_ear1 * RotationMatrix_ear1 * ScalingMatrix_ear;
			mat4 Model_ear2 = RotationMatrix_pillar * TranslationMatrix_ear2 * RotationMatrix_ear2 * ScalingMatrix_ear;
			mat4 MVP_ear1 = Projection * View * Model_ear1;
			mat4 MVP_ear2 = Projection * View * Model_ear2;
			glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP_ear1[0][0]);

			glUniform1i(TextureCheckID, 1);
			glUniform1i(TextureID, 0);

			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, triangleVertexbuffer);
			glVertexAttribPointer(
				0,                  
				3,                  
				GL_FLOAT,          
				GL_FALSE,         
				0,                   
				(void*)0          
			);
			glDrawArrays(GL_TRIANGLES, 0, 4 * 3);

			glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP_ear2[0][0]);

			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, triangleVertexbuffer);
			glVertexAttribPointer(
				0,                  
				3,                  
				GL_FLOAT,          
				GL_FALSE,         
				0,                   
				(void*)0          
			);
			glDrawArrays(GL_TRIANGLES, 0, 4 * 3);

			// Draw front leg1
			mat4 TranslationMatrix_front_right_leg1 = translate(mat4(), vec3(2.5f, -0.4f, 0.65f)) * TranslationMatrix_body;
			mat4 TranslationMatrix_front_left_leg1 = translate(mat4(), vec3(2.5f, -0.5f, -0.65f)) * TranslationMatrix_body;
			mat4 RotationMatrix_front_right_leg1 = eulerAngleYXZ(0.0f, 0.0f, 0.0f) * RotationMatrix_body;
			mat4 RotationMatrix_front_left_leg1 = eulerAngleYXZ(0.0f, 0.0f, -0.3f) * RotationMatrix_body;
			mat4 ScalingMatrix_front_leg1 = scale(mat4(), vec3(0.03f, 0.05f, 0.03f));

			mat4 Model_front_right_leg1 = RotationMatrix_pillar * TranslationMatrix_front_right_leg1 * RotationMatrix_front_right_leg1 * ScalingMatrix_front_leg1;
			mat4 Model_front_left_leg1 = RotationMatrix_pillar * TranslationMatrix_front_left_leg1 * RotationMatrix_front_left_leg1 * ScalingMatrix_front_leg1;
			mat4 MVP_front_right_leg1 = Projection * View * Model_front_right_leg1;
			mat4 MVP_front_left_leg1 = Projection * View * Model_front_left_leg1;

			glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP_front_right_leg1[0][0]);
			glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, &Model_front_right_leg1[0][0]);
			glUniform1i(TextureCheckID, 0);
			glUniform1i(TextureID, 0);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, Texture_leg);

			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, cylinderVertexbuffer);
			glVertexAttribPointer(
				0,                  
				3,                  
				GL_FLOAT,          
				GL_FALSE,         
				0,                   
				(void*)0          
			);
			glEnableVertexAttribArray(1);
			glBindBuffer(GL_ARRAY_BUFFER, cylinderUvbuffer);
			glVertexAttribPointer(
				1,                                
				2,                                
				GL_FLOAT,                        
				GL_FALSE,                       
				0,                                 
				(void*)0                        
			); 
			glEnableVertexAttribArray(2);
			glBindBuffer(GL_ARRAY_BUFFER, cylinderNormalbuffer);
			glVertexAttribPointer(
				2,
				3,
				GL_FLOAT,
				GL_FALSE,
				0,
				(void*)0
			);
			glDrawArrays(GL_TRIANGLES, 0, cylinderVertices.size());

			glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP_front_left_leg1[0][0]);
			glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, &Model_front_left_leg1[0][0]);
			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, cylinderVertexbuffer);
			glVertexAttribPointer(
				0,                  
				3,                  
				GL_FLOAT,          
				GL_FALSE,         
				0,                   
				(void*)0          
			);
			glEnableVertexAttribArray(1);
			glBindBuffer(GL_ARRAY_BUFFER, cylinderUvbuffer);
			glVertexAttribPointer(
				1,                                
				2,                                
				GL_FLOAT,                        
				GL_FALSE,                       
				0,                                 
				(void*)0                        
			);
			glEnableVertexAttribArray(2);
			glBindBuffer(GL_ARRAY_BUFFER, cylinderNormalbuffer);
			glVertexAttribPointer(
				2,
				3,
				GL_FLOAT,
				GL_FALSE,
				0,
				(void*)0
			);
			glDrawArrays(GL_TRIANGLES, 0, cylinderVertices.size());

			// Draw front leg2
			mat4 TranslationMatrix_front_right_leg2 = translate(mat4(), vec3(0.8f, -1.1f, 0.0f)) * TranslationMatrix_front_right_leg1;
			mat4 TranslationMatrix_front_left_leg2 = translate(mat4(), vec3(0.5f, -1.3f, 0.0f)) * TranslationMatrix_front_left_leg1;
			mat4 RotationMatrix_front_right_leg2 = eulerAngleYXZ(0.0f, 0.0f, -1.8f) * RotationMatrix_front_right_leg1;
			mat4 RotationMatrix_front_left_leg2 = eulerAngleYXZ(0.0f, 0.0f, -2.0f) * RotationMatrix_front_left_leg1;
			mat4 ScalingMatrix_front_leg2 = scale(mat4(), vec3(0.03f, 0.05f, 0.03f));

			mat4 Model_front_right_leg2 = RotationMatrix_pillar * TranslationMatrix_front_right_leg2 * RotationMatrix_front_right_leg2 * ScalingMatrix_front_leg2;
			mat4 Model_front_left_leg2 = RotationMatrix_pillar * TranslationMatrix_front_left_leg2 * RotationMatrix_front_left_leg2 * ScalingMatrix_front_leg2;
			mat4 MVP_front_right_leg2 = Projection * View * Model_front_right_leg2;
			mat4 MVP_front_left_leg2 = Projection * View * Model_front_left_leg2;

			glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP_front_right_leg2[0][0]);
			glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, &Model_front_right_leg2[0][0]);
			glUniform1i(TextureCheckID, 0);
			glUniform1i(TextureID, 0);

			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, cylinderVertexbuffer);
			glVertexAttribPointer(
				0,                  
				3,                  
				GL_FLOAT,          
				GL_FALSE,         
				0,                   
				(void*)0          
			);
			glEnableVertexAttribArray(1);
			glBindBuffer(GL_ARRAY_BUFFER, cylinderUvbuffer);
			glVertexAttribPointer(
				1,                                
				2,                                
				GL_FLOAT,                        
				GL_FALSE,                       
				0,                                 
				(void*)0                        
			);
			glEnableVertexAttribArray(2);
			glBindBuffer(GL_ARRAY_BUFFER, cylinderNormalbuffer);
			glVertexAttribPointer(
				2,
				3,
				GL_FLOAT,
				GL_FALSE,
				0,
				(void*)0
			);
			glDrawArrays(GL_TRIANGLES, 0, cylinderVertices.size());

			glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP_front_left_leg2[0][0]);
			glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, &Model_front_left_leg2[0][0]);

			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, cylinderVertexbuffer);
			glVertexAttribPointer(
				0,                  
				3,                  
				GL_FLOAT,          
				GL_FALSE,         
				0,                   
				(void*)0          
			);
			glEnableVertexAttribArray(1);
			glBindBuffer(GL_ARRAY_BUFFER, cylinderUvbuffer);
			glVertexAttribPointer(
				1,                                
				2,                                
				GL_FLOAT,                        
				GL_FALSE,                       
				0,                                 
				(void*)0                        
			);
			glEnableVertexAttribArray(2);
			glBindBuffer(GL_ARRAY_BUFFER, cylinderNormalbuffer);
			glVertexAttribPointer(
				2,
				3,
				GL_FLOAT,
				GL_FALSE,
				0,
				(void*)0
			);
			glDrawArrays(GL_TRIANGLES, 0, cylinderVertices.size());

			//Draw front leg3
			mat4 TranslationMatrix_front_right_leg3 = translate(mat4(), vec3(-0.4f, -1.3f, 0.0f)) * TranslationMatrix_front_right_leg2;
			mat4 TranslationMatrix_front_left_leg3 = translate(mat4(), vec3(-1.0f, -0.9f, 0.0f)) * TranslationMatrix_front_left_leg2;
			mat4 RotationMatrix_front_right_leg3 = eulerAngleYXZ(0.0f, 0.0f, -0.7f) * RotationMatrix_front_right_leg2;
			mat4 RotationMatrix_front_left_leg3 = eulerAngleYXZ(0.0f, 0.0f, -0.6f) * RotationMatrix_front_left_leg2;
			mat4 ScalingMatrix_front_leg3 = scale(mat4(), vec3(0.03f, 0.013f, 0.03f));

			mat4 Model_front_right_leg3 = RotationMatrix_pillar * TranslationMatrix_front_right_leg3 * RotationMatrix_front_right_leg3 * ScalingMatrix_front_leg3;
			mat4 Model_front_left_leg3 = RotationMatrix_pillar * TranslationMatrix_front_left_leg3 * RotationMatrix_front_left_leg3 * ScalingMatrix_front_leg3;
			mat4 MVP_front_right_leg3 = Projection * View * Model_front_right_leg3;
			mat4 MVP_front_left_leg3 = Projection * View * Model_front_left_leg3;

			glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP_front_right_leg3[0][0]);
			glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, &Model_front_right_leg3[0][0]);
			glUniform1i(TextureCheckID, 3);
			glUniform1i(TextureID, 0);

			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, cylinderVertexbuffer);
			glVertexAttribPointer(
				0,                  
				3,                  
				GL_FLOAT,          
				GL_FALSE,         
				0,                   
				(void*)0          
			);
			glEnableVertexAttribArray(2);
			glBindBuffer(GL_ARRAY_BUFFER, cylinderNormalbuffer);
			glVertexAttribPointer(
				2,
				3,
				GL_FLOAT,
				GL_FALSE,
				0,
				(void*)0
			);
			glDrawArrays(GL_TRIANGLES, 0, cylinderVertices.size());

			glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP_front_left_leg3[0][0]);
			glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, &Model_front_left_leg3[0][0]);

			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, cylinderVertexbuffer);
			glVertexAttribPointer(
				0,                  
				3,                  
				GL_FLOAT,          
				GL_FALSE,         
				0,                   
				(void*)0          
			);
			glEnableVertexAttribArray(2);
			glBindBuffer(GL_ARRAY_BUFFER, cylinderNormalbuffer);
			glVertexAttribPointer(
				2,
				3,
				GL_FLOAT,
				GL_FALSE,
				0,
				(void*)0
			);
			glDrawArrays(GL_TRIANGLES, 0, cylinderVertices.size());

			// Draw back leg1
			mat4 TranslationMatrix_back_right_leg1 = translate(mat4(), vec3(-2.1f, -1.0f, 0.65f)) * TranslationMatrix_body;
			mat4 TranslationMatrix_back_left_leg1 = translate(mat4(), vec3(-2.1f, -1.0f, -0.65f)) * TranslationMatrix_body;
			mat4 RotationMatrix_back_right_leg1 = eulerAngleYXZ(0.0f, 0.0f, 0.7f) * RotationMatrix_body;
			mat4 RotationMatrix_back_left_leg1 = eulerAngleYXZ(0.0f, 0.0f, 0.7f) * RotationMatrix_body;
			mat4 ScalingMatrix_back_leg1 = scale(mat4(), vec3(0.035f, 0.05f, 0.035f));

			mat4 Model_back_right_leg1 = RotationMatrix_pillar * TranslationMatrix_back_right_leg1 * RotationMatrix_back_right_leg1 * ScalingMatrix_back_leg1;
			mat4 Model_back_left_leg1 = RotationMatrix_pillar * TranslationMatrix_back_left_leg1 * RotationMatrix_back_left_leg1 * ScalingMatrix_back_leg1;
			mat4 MVP_back_right_leg1 = Projection * View * Model_back_right_leg1;
			mat4 MVP_back_left_leg1 = Projection * View * Model_back_left_leg1;

			glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP_back_right_leg1[0][0]);
			glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, &Model_back_right_leg1[0][0]);
			glUniform1i(TextureCheckID, 0);
			glUniform1i(TextureID, 0);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, Texture_leg);

			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, cylinderVertexbuffer);
			glVertexAttribPointer(
				0,                  
				3,                  
				GL_FLOAT,          
				GL_FALSE,         
				0,                   
				(void*)0          
			);
			glEnableVertexAttribArray(1);
			glBindBuffer(GL_ARRAY_BUFFER, cylinderUvbuffer);
			glVertexAttribPointer(
				1,                                
				2,                                
				GL_FLOAT,                        
				GL_FALSE,                       
				0,                                 
				(void*)0                        
			);
			glEnableVertexAttribArray(2);
			glBindBuffer(GL_ARRAY_BUFFER, cylinderNormalbuffer);
			glVertexAttribPointer(
				2,
				3,
				GL_FLOAT,
				GL_FALSE,
				0,
				(void*)0
			);
			glDrawArrays(GL_TRIANGLES, 0, cylinderVertices.size());

			glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP_back_left_leg1[0][0]);
			glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, &Model_back_left_leg1[0][0]);

			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, cylinderVertexbuffer);
			glVertexAttribPointer(
				0,                  
				3,                  
				GL_FLOAT,          
				GL_FALSE,         
				0,                   
				(void*)0          
			);
			glEnableVertexAttribArray(1);
			glBindBuffer(GL_ARRAY_BUFFER, cylinderUvbuffer);
			glVertexAttribPointer(
				1,                                
				2,                                
				GL_FLOAT,                        
				GL_FALSE,                       
				0,                                 
				(void*)0                        
			);
			glEnableVertexAttribArray(2);
			glBindBuffer(GL_ARRAY_BUFFER, cylinderNormalbuffer);
			glVertexAttribPointer(
				2,
				3,
				GL_FLOAT,
				GL_FALSE,
				0,
				(void*)0
			);
			glDrawArrays(GL_TRIANGLES, 0, cylinderVertices.size());

			// Draw back leg2
			mat4 TranslationMatrix_back_right_leg2 = translate(mat4(), vec3(-1.1f, -1.4f, 0.0f)) * TranslationMatrix_back_right_leg1;
			mat4 TranslationMatrix_back_left_leg2 = translate(mat4(), vec3(-0.8f, -1.5f, 0.0f)) * TranslationMatrix_back_left_leg1;
			mat4 RotationMatrix_back_right_leg2 = eulerAngleYXZ(0.0f, 0.0f, -6.0f) * RotationMatrix_back_right_leg1;
			mat4 RotationMatrix_back_left_leg2 = eulerAngleYXZ(0.0f, 0.0f, -5.4f) * RotationMatrix_back_left_leg1;
			mat4 ScalingMatrix_back_leg2 = scale(mat4(), vec3(0.03f, 0.045f, 0.03f));

			mat4 Model_back_right_leg2 = RotationMatrix_pillar * TranslationMatrix_back_right_leg2 * RotationMatrix_back_right_leg2 * ScalingMatrix_back_leg2;
			mat4 Model_back_left_leg2 = RotationMatrix_pillar * TranslationMatrix_back_left_leg2 * RotationMatrix_back_left_leg2 * ScalingMatrix_back_leg2;
			mat4 MVP_back_right_leg2 = Projection * View * Model_back_right_leg2;
			mat4 MVP_back_left_leg2 = Projection * View * Model_back_left_leg2;

			glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP_back_right_leg2[0][0]);
			glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, &Model_back_right_leg2[0][0]);
			glUniform1i(TextureCheckID, 0);
			glUniform1i(TextureID, 0);

			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, cylinderVertexbuffer);
			glVertexAttribPointer(
				0,                  
				3,                  
				GL_FLOAT,          
				GL_FALSE,         
				0,                   
				(void*)0          
			);
			glEnableVertexAttribArray(1);
			glBindBuffer(GL_ARRAY_BUFFER, cylinderUvbuffer);
			glVertexAttribPointer(
				1,                                
				2,                                
				GL_FLOAT,                        
				GL_FALSE,                       
				0,                                 
				(void*)0                        
			);
			glEnableVertexAttribArray(2);
			glBindBuffer(GL_ARRAY_BUFFER, cylinderNormalbuffer);
			glVertexAttribPointer(
				2,
				3,
				GL_FLOAT,
				GL_FALSE,
				0,
				(void*)0
			);
			glDrawArrays(GL_TRIANGLES, 0, cylinderVertices.size());

			glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP_back_left_leg2[0][0]);
			glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, &Model_back_left_leg2[0][0]);

			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, cylinderVertexbuffer);
			glVertexAttribPointer(
				0,                  
				3,                  
				GL_FLOAT,          
				GL_FALSE,         
				0,                   
				(void*)0          
			);
			glEnableVertexAttribArray(1);
			glBindBuffer(GL_ARRAY_BUFFER, cylinderUvbuffer);
			glVertexAttribPointer(
				1,                                
				2,                                
				GL_FLOAT,                        
				GL_FALSE,                       
				0,                                 
				(void*)0                        
			);
			glEnableVertexAttribArray(2);
			glBindBuffer(GL_ARRAY_BUFFER, cylinderNormalbuffer);
			glVertexAttribPointer(
				2,
				3,
				GL_FLOAT,
				GL_FALSE,
				0,
				(void*)0
			);
			glDrawArrays(GL_TRIANGLES, 0, cylinderVertices.size());

			//Draw back leg3
			mat4 TranslationMatrix_back_right_leg3 = translate(mat4(), vec3(-0.8f, -0.9f, 0.0f)) * TranslationMatrix_back_right_leg2;
			mat4 TranslationMatrix_back_left_leg3 = translate(mat4(), vec3(-0.2f, -1.1f, 0.0f)) * TranslationMatrix_back_left_leg2;
			mat4 RotationMatrix_back_right_leg3 = eulerAngleYXZ(0.0f, 0.0f, -1.1f) * RotationMatrix_back_right_leg2;
			mat4 RotationMatrix_back_left_leg3 = eulerAngleYXZ(0.0f, 0.0f, -1.1f) * RotationMatrix_back_left_leg2;
			mat4 ScalingMatrix_back_leg3 = scale(mat4(), vec3(0.03f, 0.011f, 0.03f));

			mat4 Model_back_right_leg3 = RotationMatrix_pillar * TranslationMatrix_back_right_leg3 * RotationMatrix_back_right_leg3 * ScalingMatrix_back_leg3;
			mat4 Model_back_left_leg3 = RotationMatrix_pillar * TranslationMatrix_back_left_leg3 * RotationMatrix_back_left_leg3 * ScalingMatrix_back_leg3;
			mat4 MVP_back_right_leg3 = Projection * View * Model_back_right_leg3;
			mat4 MVP_back_left_leg3 = Projection * View * Model_back_left_leg3;

			glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP_back_right_leg3[0][0]);
			glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, &Model_back_right_leg3[0][0]);

			glUniform1i(TextureCheckID, 3);
			glUniform1i(TextureID, 0);

			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, cylinderVertexbuffer);
			glVertexAttribPointer(
				0,                  
				3,                  
				GL_FLOAT,          
				GL_FALSE,         
				0,                   
				(void*)0          
			);
			glEnableVertexAttribArray(2);
			glBindBuffer(GL_ARRAY_BUFFER, cylinderNormalbuffer);
			glVertexAttribPointer(
				2,
				3,
				GL_FLOAT,
				GL_FALSE,
				0,
				(void*)0
			);
			glDrawArrays(GL_TRIANGLES, 0, cylinderVertices.size());

			glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP_back_left_leg3[0][0]);
			glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, &Model_back_left_leg3[0][0]);

			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, cylinderVertexbuffer);
			glVertexAttribPointer(
				0,               
				3,          
				GL_FLOAT,     
				GL_FALSE,        
				0,                
				(void*)0          
			);
			glEnableVertexAttribArray(2);
			glBindBuffer(GL_ARRAY_BUFFER, cylinderNormalbuffer);
			glVertexAttribPointer(
				2,
				3,
				GL_FLOAT,
				GL_FALSE,
				0,
				(void*)0
			);
			glDrawArrays(GL_TRIANGLES, 0, cylinderVertices.size());

			// Draw tail
			mat4 TranslationMatrix_tail = translate(mat4(), vec3(-2.9f, -0.8f, 0.0f)) * TranslationMatrix_body;
			mat4 RotationMatrix_tail = eulerAngleYXZ(0.0f, 0.0f, 1.0f) * RotationMatrix_body;
			mat4 ScalingMatrix_tail = scale(mat4(), vec3(0.03f, 0.1f, 0.03f));

			mat4 Model_tail = RotationMatrix_pillar * TranslationMatrix_tail * RotationMatrix_tail * ScalingMatrix_tail;
			mat4 MVP_tail = Projection * View * Model_tail;

			glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP_tail[0][0]);
			glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, &Model_tail[0][0]);
			glUniform1i(TextureCheckID, 0);
			glUniform1i(TextureID, 0);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, Texture_tail);

			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, cylinderVertexbuffer);
			glVertexAttribPointer(
				0,      
				3,             
				GL_FLOAT,      
				GL_FALSE,      
				0,           
				(void*)0 
			);
			glEnableVertexAttribArray(1);
			glBindBuffer(GL_ARRAY_BUFFER, cylinderUvbuffer);
			glVertexAttribPointer(
				1,                           
				2,                               
				GL_FLOAT,                         
				GL_FALSE,                        
				0,                                
				(void*)0                          
			);
			glEnableVertexAttribArray(2);
			glBindBuffer(GL_ARRAY_BUFFER, cylinderNormalbuffer);
			glVertexAttribPointer(
				2,
				3,
				GL_FLOAT,
				GL_FALSE,
				0,
				(void*)0
			);
			glDrawArrays(GL_TRIANGLES, 0, cylinderVertices.size());
		}

		//Ȱ��ȭ�� attribute �� ��Ȱ��ȭ
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

	//���� �� ��������ش�.
	glDeleteBuffers(1, &cubeVertexbuffer);
	glDeleteBuffers(1, &cylinderVertexbuffer);
	glDeleteBuffers(1, &sphereVertexbuffer);
	glDeleteBuffers(1, &triangleVertexbuffer);

	glDeleteBuffers(1, &cubeUvbuffer);
	glDeleteBuffers(1, &cylinderUvbuffer);
	glDeleteBuffers(1, &sphereUvbuffer);

	glDeleteBuffers(1, &cubeNormalbuffer);
	glDeleteBuffers(1, &cylinderNormalbuffer);
	glDeleteBuffers(1, &sphereNormalbuffer);

	glDeleteProgram(programID);

	glDeleteTextures(1, &Texture_body);
	glDeleteTextures(1, &Texture_tail);
	glDeleteTextures(1, &Texture_sit);
	glDeleteTextures(1, &Texture_leg);
	glDeleteTextures(1, &Texture_neck);
	glDeleteTextures(1, &Texture_head);
	glDeleteTextures(1, &Texture_nose);
	glDeleteVertexArrays(1, &VertexArrayID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

// space�� �Է��� �޾� ȸ���񸶸� ȸ����Ű�ų� ������Ű�� �Լ� + �񸶸� ���Ʒ��� �����̴� �Լ�
void spin() {
	static double lastFrameTime = glfwGetTime();
	double currentTime = glfwGetTime();
	float deltaTime = (float)(currentTime - lastFrameTime);
	// �񸶳��� �ִ��� Ȥ�� �ּ����� ���������� flag�� �ٲپ��ش�
	// 2���� ������ ����� ���� 2���� �÷��׸� ���
	if (upAndDown1 >= 0) {
		updownFlag1 = false;
	}
	else if (upAndDown1 <= -2.5f) {
		updownFlag1 = true;
	}
	if (upAndDown2 >= 0) {
		updownFlag2 = false;
	}
	else if (upAndDown2 <= -2.5f) {
		updownFlag2 = true;
	}
	//when stopped (spacebar pressed)
	if (spaceFlag) {
		if (rotSpeed < 0.4f) {
			rotSpeed += 0.001f;
		}
		else {
			rotSpeed = 0.4f;
		}
		if (upDownSpeed < 2.0f) {
			upDownSpeed += 0.005f;
		}
		else {
			upDownSpeed = 2.0f;
		}
	}
	//when moving (spacebar pressed again)
	else {
		if (rotSpeed > 0.0f)
			rotSpeed -= 0.001f;
		else {
			rotSpeed = 0.0f;
		}
		if (upDownSpeed > 0.0f) {
			upDownSpeed -= 0.005f;
		}
		else {
			upDownSpeed = 0.0f;
		}
	}
	// ȸ���� ���������� ���Ʒ� �����̴� �ӵ��� ���ӵǾ� ���� �ӵ��� �����ϸ� �״�� �����ȴ�
	if (updownFlag1) {
		upAndDown1 += deltaTime * upDownSpeed;
	}
	else {
		upAndDown1 -= deltaTime * upDownSpeed;
	}
	if (updownFlag2) {
		upAndDown2 += deltaTime * upDownSpeed;
	}
	else {
		upAndDown2 -= deltaTime * upDownSpeed;
	}
	gOrientation += 3.14159f * deltaTime * rotSpeed;

	lastFrameTime = currentTime;
}

// Ű���� wasd �Է��� �޾� ī�޶� �����̴� �Լ�
void cameraMove() {
	static double lastFrameTime = glfwGetTime();
	double currentTime = glfwGetTime();
	float deltaTime = (float)(currentTime - lastFrameTime);

	// a, d Ű�� �������� ���� ���� ���������� �����ְ� ���ش�.
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		cameraHorizontalAngle += deltaTime * 1.0f;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		cameraHorizontalAngle += -deltaTime * 1.0f;
	}
	// ī�޶� ���� ���� ���
	vec3 cameraDirection(
		cos(cameraVerticalAngle) * sin(cameraHorizontalAngle),
		cameraVerticalAngle,
		cos(cameraVerticalAngle) * cos(cameraHorizontalAngle)

	);
	// W�� SŰ�� �������� y���̵��� ������ ������ x,z���� �̵���Ų��.
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		cameraPos.x += cameraDirection.x * deltaTime * 20.0f;
		cameraPos.z += cameraDirection.z * deltaTime * 20.0f;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		cameraPos.x -= cameraDirection.x * deltaTime * 20.0f;
		cameraPos.z -= cameraDirection.z * deltaTime * 20.0f;
	}
	// ��ȭ�� view ��Ʈ������ �ݿ��Ѵ�.
	View = lookAt(
		cameraPos,
		cameraPos + cameraDirection,
		vec3(0, 1, 0)
	);
	lastFrameTime = currentTime;
}