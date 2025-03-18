#include <stdio.h>
#include <math.h>

#include <ew/external/glad.h>
#include <ew/shader.h>
#include <ew/model.h>
#include <ew/camera.h>
#include <ew/cameraController.h>
#include <ew/transform.h>
#include <ew/texture.h>
#include <ew/procGen.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
GLFWwindow* initWindow(const char* title, int width, int height);
void drawUI();

struct
{
	glm::vec3 color = {0.0f, 0.31f, 0.85f};
	float tiling = 1.0f;
	float b1 = 0.0f;
	float b2 = 0.0f;
}debug;

struct Material
{
	float Ka = 1.0;
	float Kd = 0.5;
	float Ks = 0.5;
	float Shininess = 128;
}material;

struct Framebuffer
{
	GLuint fbo;
	GLuint color0;
	GLuint color1;
	GLuint color2;
	GLuint depth;
};
Framebuffer framebuffer1, framebuffer2;

struct FullscreenQuad {
	GLuint vao;
	GLuint vbo;
};
FullscreenQuad fullscreen_quad;

static float quad_vertices[] = {
	//pos (x,y) texcoords (u,v)
	-1.0f, 1.0f, 0.0f, 1.0f,
	-1.0f, -1.0f, 0.0f, 0.0f,
	1.0f, -1.0f, 1.0f, 0.0f,

	-1.0f, 1.0f, 0.0f, 1.0f,
	1.0f, -1.0f, 1.0f, 0.0f,
	1.0f, 1.0f, 1.0f, 1.0f,
};

ew::Camera camera;
ew::CameraController cameraController;
ew::Transform monkeyTransform;

int texSlot = 0;

//Global state
int screenWidth = 1080;
int screenHeight = 720;
float prevFrameTime;
float deltaTime;

struct DepthBuffer {
	GLuint fbo;
	GLuint depth;

	void Initialize() {
		//init framebuffer 
		glGenFramebuffers(1, &fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);

		//color
		glGenTextures(1, &depth);
		glBindTexture(GL_TEXTURE_2D, depth);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1024, 1024, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depth, 0);

		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			printf("frame buffer not complete");
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

}depthbuffer;

void resetCamera(ew::Camera* camera, ew::CameraController* controller) {
	camera->position = glm::vec3(0, 0, 5.0f);
	camera->target = glm::vec3(0);
	controller->yaw = controller->pitch = 0;
}

void func(ew::Shader& shader, ew::Shader& geoShader, ew::Model &model, GLFWwindow *window, float time)
{	
	// bind fbuffer
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer1.fbo);
	glViewport(0, 0, 800, 600);
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//clear default buffer
	glClearColor(0.6f, 0.8f, 0.92f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	//1. pipeline definion
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);

	//2. gfx pass (what goes onto screen)
	//RENDER
	glClearColor(0.6f, 0.8f, 0.92f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//rotate model around Y
	monkeyTransform.rotation = glm::rotate(monkeyTransform.rotation, deltaTime, glm::vec3(0.0, 1.0, 0.0));

	cameraController.move(window, &camera, deltaTime);

	//bind brick to tex unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, framebuffer1.color0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, framebuffer1.color1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, framebuffer1.color2);


	shader.use();
	shader.setInt("_NormalMap", texSlot);

	for (int i = 0; i < 9; i++)
	{
		for (int j = 0; j < 9; j++)
		{
			shader.setMat4("transformModel", glm::translate(glm::vec3(i * 2.0f, 0, j * 2.0f)));
			model.draw(); //Draws monkey model using current shader
		}
	}
	
	shader.setMat4("viewProjection", camera.projectionMatrix() * camera.viewMatrix());
	shader.setVec3("cameraPos", camera.position);
	shader.setVec3("water_color", debug.color);
	shader.setInt("texture", 0);

	//Material
	shader.setFloat("_Material.Ka", material.Ka);
	shader.setFloat("_Material.Kd", material.Kd);
	shader.setFloat("_Material.Ks", material.Ks);
	shader.setFloat("_Material.Shininess", material.Shininess);

	cameraController.move(window, &camera, deltaTime);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

int main() {
	GLFWwindow* window = initWindow("Assignment 0", screenWidth, screenHeight);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

	//textures
	GLuint brickTexture = ew::loadTexture("assets/brick_color.jpg");
	GLuint waterTexture = ew::loadTexture("assets/water.png");
	GLuint stonesTexture = ew::loadTexture("assets/PavingStones/PavingStones138_1K-JPG_Color.jpg");
	GLuint stonesNorms = ew::loadTexture("assets/PavingStones/PavingStones138_1K-JPG_NormalGL.jpg");

	ew::Shader shader_fullscreen = ew::Shader("assets/fullscreen.vert", "assets/fullscreen.frag");

	//cache
	ew::Shader shader = ew::Shader("assets/lit.vert", "assets/lit.frag");
	ew::Shader geo_shader = ew::Shader("assets/geo.vert", "assets/geo.frag");
	ew::Model monkeyModel = ew::Model("assets/suzanne.fbx");
	ew::Mesh plane = ew::createPlane(50.0f, 50.0f, 100.0f);

	depthbuffer.Initialize();

	//camera
	camera.position = glm::vec3(0.0f, 0.0f, 5.0f);
	camera.target = glm::vec3(0.0f, 0.0f, 0.0f); //looks at center
	camera.aspectRatio = (float)screenWidth / screenHeight;
	camera.fov = 60.0f; //Vertical fov in degrees

	//initialize fullscreen quad
	glGenVertexArrays(1, &fullscreen_quad.vao);
	glGenBuffers(1, &fullscreen_quad.vbo);

	//bind vao to vbo
	glBindVertexArray(fullscreen_quad.vao);
	glBindBuffer(GL_ARRAY_BUFFER, fullscreen_quad.vbo);

	//buffer data to vbo
	glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), &quad_vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0); //positions
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)(sizeof(float) * 2));
	glBindVertexArray(0);

	//init framebuffer
	glGenFramebuffers(1, &framebuffer1.fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer1.fbo);

	//color
	glGenTextures(1, &framebuffer1.color0);
	glBindTexture(GL_TEXTURE_2D, framebuffer1.color0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebuffer1.color0, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		float time = (float)glfwGetTime();
		deltaTime = time - prevFrameTime;
		prevFrameTime = time;

		//clear default buffer
		glClearColor(0.6f, 0.8f, 0.92f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		func(shader, geo_shader, monkeyModel, window, time);

		//render fullscreen quad
		shader_fullscreen.use();
		shader_fullscreen.setInt("texture0", 0);
		glViewport(0, 0, screenWidth, screenHeight);
		glBindVertexArray(fullscreen_quad.vao);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, framebuffer2.color0);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);

		drawUI();

		glfwSwapBuffers(window);
	}
	printf("Shutting down...");
}

void drawUI() {
	ImGui_ImplGlfw_NewFrame();
	ImGui_ImplOpenGL3_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Settings");
	if (ImGui::Button("Reset Camera"))
	{
		resetCamera(&camera, &cameraController);
	}
	if (ImGui::CollapsingHeader("Material"))
	{
		ImGui::SliderFloat("AmbientK", &material.Ka, 0.0f, 1.0f);
		ImGui::SliderFloat("DiffuseK", &material.Kd, 0.0f, 1.0f);
		ImGui::SliderFloat("SpecularK", &material.Ks, 0.0f, 1.0f);
		ImGui::SliderFloat("Shininess", &material.Shininess, 2.0f, 1024.0f);
	}
	ImGui::Image((ImTextureID)(intptr_t)framebuffer1.color0, ImVec2(800, 600));
	ImGui::Image((ImTextureID)(intptr_t)framebuffer2.color0, ImVec2(800, 600));
	ImGui::Text("Add Controls Here!");
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	screenWidth = width;
	screenHeight = height;
}

/// <summary>
/// Initializes GLFW, GLAD, and IMGUI
/// </summary>
/// <param name="title">Window title</param>
/// <param name="width">Window width</param>
/// <param name="height">Window height</param>
/// <returns>Returns window handle on success or null on fail</returns>
GLFWwindow* initWindow(const char* title, int width, int height) {
	printf("Initializing...");
	if (!glfwInit()) {
		printf("GLFW failed to init!");
		return nullptr;
	}

	GLFWwindow* window = glfwCreateWindow(width, height, title, NULL, NULL);
	if (window == NULL) {
		printf("GLFW failed to create window");
		return nullptr;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGL(glfwGetProcAddress)) {
		printf("GLAD Failed to load GL headers");
		return nullptr;
	}

	//Initialize ImGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();

	return window;
}

