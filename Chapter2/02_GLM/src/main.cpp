#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <stdio.h>
#include <stdlib.h>

using glm::mat4;
using glm::vec3;

static const char* shaderCodeVertex = R"(
#version 460 core
layout(std140, binding = 0) uniform PerFrameData
{
	uniform mat4 MVP;
	uniform int isWireframe;
};
layout (location=0) out vec3 color;
const vec3 pos[8] = vec3[8](
	vec3(-1.0,-1.0, 1.0),
	vec3( 1.0,-1.0, 1.0),
	vec3( 1.0, 1.0, 1.0),
	vec3(-1.0, 1.0, 1.0),

	vec3(-1.0,-1.0,-1.0),
	vec3( 1.0,-1.0,-1.0),
	vec3( 1.0, 1.0,-1.0),
	vec3(-1.0, 1.0,-1.0)
);
const vec3 col[8] = vec3[8](
	vec3( 1.0, 0.0, 0.0),
	vec3( 0.0, 1.0, 0.0),
	vec3( 0.0, 0.0, 1.0),
	vec3( 1.0, 1.0, 0.0),

	vec3( 1.0, 1.0, 0.0),
	vec3( 0.0, 0.0, 1.0),
	vec3( 0.0, 1.0, 0.0),
	vec3( 1.0, 0.0, 0.0)
);
const int indices[36] = int[36](
	// front
	0, 1, 2, 2, 3, 0,
	// right
	1, 5, 6, 6, 2, 1,
	// back
	7, 6, 5, 5, 4, 7,
	// left
	4, 0, 3, 3, 7, 4,
	// bottom
	4, 5, 1, 1, 0, 4,
	// top
	3, 2, 6, 6, 7, 3
);
void main()
{
	int idx = indices[gl_VertexID];
	gl_Position = MVP * vec4(pos[idx], 1.0);
	color = isWireframe > 0 ? vec3(0.0) : col[idx];
}
)";

static const char* shaderCodeFragment = R"(
#version 460 core
layout (location=0) in vec3 color;
layout (location=0) out vec4 out_FragColor;
void main()
{
	out_FragColor = vec4(color, 1.0);
};
)";

struct PerFrameData
{
	mat4 mvp;
	int isWireframe;
};

int main(void)
{
	glfwSetErrorCallback(
		[](int error, const char* description)
		{
			fprintf(stderr, "Error: %s\n", description);
		}
	);

	if (!glfwInit())
		exit(EXIT_FAILURE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(1024, 768, "Simple example", nullptr, nullptr);
	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwSetKeyCallback(
		window,
		[](GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
				glfwSetWindowShouldClose(window, GLFW_TRUE);
		}
	);

	glfwMakeContextCurrent(window);
	gladLoadGL(glfwGetProcAddress);
	glfwSwapInterval(1);

	const GLuint shaderVertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(shaderVertex, 1, &shaderCodeVertex, nullptr);
	glCompileShader(shaderVertex);

	const GLuint shaderFragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(shaderFragment, 1, &shaderCodeFragment, nullptr);
	glCompileShader(shaderFragment);

	const GLuint program = glCreateProgram();
	glAttachShader(program, shaderVertex);
	glAttachShader(program, shaderFragment);
	glLinkProgram(program);
	glUseProgram(program);

	GLuint vao;
	glCreateVertexArrays(1, &vao);
	glBindVertexArray(vao);

	const GLsizeiptr kBufferSize = sizeof(PerFrameData);

	GLuint perFrameDataBuffer;
	//DSA（Direct State Access） [OpenGL 4.5+]
	//通过调用glCreateBuffers()函数创建一个缓冲区对象，并将其存储在perFrameDataBuffer变量中。这个缓冲区对象将用于存储每帧数据。
	glCreateBuffers(1, &perFrameDataBuffer);
	//为缓冲区对象分配存储空间，并指定大小为kBufferSize，这里假定kBufferSize是一个预定义的常量。传递nullptr作为数据参数，表示暂时不填充数据到缓冲区。
	glNamedBufferStorage(perFrameDataBuffer, kBufferSize, nullptr, GL_DYNAMIC_STORAGE_BIT);
	//将缓冲区对象绑定到统一块绑定点。这里使用绑定点0，表示将UBO(Uniform Buffer Obj)绑定到OpenGL上下文的0号统一块中。这个绑定操作允许我们在
	//着色器中通过统一块绑定点索引来访问缓冲区数据。使用GL_UNIFORM_BUFFER作为目标，表示绑定的是Uniform缓冲区。
	/*void glBindBufferRange(GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size);
	参数说明：
	target：指定绑定的目标类型，可以是GL_UNIFORM_BUFFER（Uniform缓冲区）或GL_SHADER_STORAGE_BUFFER（着色器存储缓冲区）。
	index：指定绑定的统一块绑定点索引。在GLSL中，使用layout(std140, binding = index)来指定统一块的绑定点。
	buffer：要绑定的缓冲区对象的名称。
	offset：缓冲区中的偏移量，指定了绑定的起始位置。
	size：绑定的范围大小，指定了从偏移量开始绑定的字节数。
	*/
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, perFrameDataBuffer, 0, kBufferSize);

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	//开启深度测试
	glEnable(GL_DEPTH_TEST);
	//开启多边形偏移，在实体图像上渲染立方体的线框图像而不出现z-fighting
	//Z-fighting:
	//开启深度测试后OpenGL就不会再去绘制模型被遮挡的部分，这样实现的显示画面更为真实，但
	//是由于深度缓冲区精度的限制，对于深度相差非常小的情况（例如在同一平面上进行两次绘制），OpenGL就
	//不能正确判定两者的深度值，会导致深度测试的结果不可预测，显示出来的现象时交错闪烁的前后两个画面，这种情况称为z-fighting。
	glEnable(GL_POLYGON_OFFSET_LINE);
	//设置多边形偏移量,这行代码设置了多边形偏移量的具体值，其中-1.0f表示偏移量的因子。
	//https://www.cnblogs.com/lovebay/p/15494510.html
	glPolygonOffset(-1.0f, -1.0f);

	while (!glfwWindowShouldClose(window))
	{
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		const float ratio = width / (float)height;

		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// 创建一个单位矩阵，并对其进行平移，平移向量是 (0.0f, 0.0f, -3.5f)。
		// 平移矩阵进行旋转，绕着vec3(1.0f, 1.0f, 1.0f)旋转(float)glfwGetTime()角度
		const mat4 m = glm::rotate(glm::translate(mat4(1.0f), vec3(0.0f, 0.0f, -3.5f)), (float)glfwGetTime(), vec3(1.0f, 1.0f, 1.0f));
		// 透视矩阵
		const mat4 p = glm::perspective(45.0f, ratio, 0.1f, 1000.0f);
		
		PerFrameData perFrameData = { .mvp = p * m, .isWireframe = false };
		//1.第一次draw: 立方体
		glNamedBufferSubData(perFrameDataBuffer, 0, kBufferSize, &perFrameData);
		
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		//2.第二次draw，只画线，画到wireframe为黑色
		perFrameData.isWireframe = true;
		glNamedBufferSubData(perFrameDataBuffer, 0, kBufferSize, &perFrameData);

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteBuffers(1, &perFrameDataBuffer);
	glDeleteProgram(program);
	glDeleteShader(shaderFragment);
	glDeleteShader(shaderVertex);
	glDeleteVertexArrays(1, &vao);

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}
