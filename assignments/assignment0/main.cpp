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

ew::Camera camera;
ew::CameraController cameraController;
ew::Transform monkeyTransform;

int texSlot = 0;

//Global state
int screenWidth = 1080;
int screenHeight = 720;
float prevFrameTime;
float deltaTime;

void resetCamera(ew::Camera* camera, ew::CameraController* controller) {
	camera->position = glm::vec3(0, 0, 5.0f);
	camera->target = glm::vec3(0);
	controller->yaw = controller->pitch = 0;
}

void func(ew::Shader& shader, ew::Model &model, GLFWwindow *window, GLuint &waterTexture, ew::Mesh planeMesh, float time)
{
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
	glBindTexture(GL_TEXTURE_2D, waterTexture);

	shader.use();
	shader.setMat4("transformModel", glm::mat4(1.0f));
	shader.setMat4("viewProjection", camera.projectionMatrix() * camera.viewMatrix());

	shader.setVec3("cameraPos", camera.position);
	shader.setVec3("water_color", debug.color);
	shader.setInt("texture", 0);
	shader.setFloat("tiling", debug.tiling);
	shader.setFloat("time", time);
	shader.setFloat("b1", debug.b1);
	shader.setFloat("b2", debug.b2);

	cameraController.move(window, &camera, deltaTime);
	planeMesh.draw();

	//model.draw(); //Draws monkey model using current shader

}

int main() {
	GLFWwindow* window = initWindow("Assignment 0", screenWidth, screenHeight);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

	//textures
	GLuint brickTexture = ew::loadTexture("assets/brick_color.jpg");
	GLuint waterTexture = ew::loadTexture("assets/water.png");
	GLuint stonesTexture = ew::loadTexture("assets/PavingStones/PavingStones138_1K-JPG_Color.jpg");
	GLuint stonesNorms = ew::loadTexture("assets/PavingStones/PavingStones138_1K-JPG_NormalGL.jpg");

	//cache
	ew::Shader shader = ew::Shader("assets/water.vert", "assets/water.frag");
	ew::Model monkeyModel = ew::Model("assets/suzanne.fbx");
	ew::Mesh plane = ew::createPlane(50.0f, 50.0f, 100.0f);

	//camera
	camera.position = glm::vec3(0.0f, 0.0f, 5.0f);
	camera.target = glm::vec3(0.0f, 0.0f, 0.0f); //looks at center
	camera.aspectRatio = (float)screenWidth / screenHeight;
	camera.fov = 60.0f; //Vertical fov in degrees

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		float time = (float)glfwGetTime();
		deltaTime = time - prevFrameTime;
		prevFrameTime = time;

		func(shader, monkeyModel, window, waterTexture, plane, time);

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
	ImGui::ColorEdit3("debug", &debug.color.x);
	ImGui::SliderFloat("tiling", &debug.tiling, 1.0f, 10.0f);
	ImGui::SliderFloat("b1", &debug.b1, 0.0f, 1.0f);
	ImGui::SliderFloat("b2", &debug.b2, 0.0f, 1.0f);
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

