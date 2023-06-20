#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <stdlib.h>

/*
#version 460 core: 这是版本指令，指定OpenGL的版本为4.6，使用核心配置。
layout (location=0) out vec3 color;: 这行代码定义了一个输出变量color，它是一个三维向量。

gl_Position = vec4(pos[gl_VertexID], 0.0, 1.0);: 这行代码将顶点的位置赋值给内置变量gl_Position，通过索引gl_VertexID访问顶点的位置坐标。
color = col[gl_VertexID];: 这行代码将顶点的颜色赋值给输出变量color，通过索引gl_VertexID访问顶点的颜色信息。
 */
static const char* shaderCodeVertex = R"(
#version 460 core
layout (location=0) out vec3 color;
const vec2 pos[3] = vec2[3](
	vec2(-0.6, -0.4),
	vec2( 0.6, -0.4),
	vec2( 0.0,  0.6)
);
const vec3 col[3] = vec3[3](
	vec3( 1.0, 0.0, 0.0 ),
	vec3( 0.0, 1.0, 0.0 ),
	vec3( 0.0, 0.0, 1.0 )
);
void main()
{
	gl_Position = vec4(pos[gl_VertexID], 0.0, 1.0);
	color = col[gl_VertexID];
}
)";
/*
片段着色器中包含以下内容：
#version 460 core: 这是版本指令，指定OpenGL的版本为4.6，使用核心配置。
layout (location=0) in vec3 color;: 这行代码定义了一个输入变量color，它是一个三维向量。
layout (location=0) out vec4 out_FragColor;: 这行代码定义了一个输出变量out_FragColor，它是一个四维向量。
void main() { ... }: 这是片段着色器的主函数。
out_FragColor = vec4(color, 1.0);: 这行代码将输入变量color作为颜色值，赋值给输出变量out_FragColor。通过vec4函数将
颜色值转为四维向量，其中最后一个分量（alpha通道）设置为1.0，表示完全不透明。
 */
//顶点着色器负责计算顶点的位置和颜色等属性，而片段着色器负责对每个像素进行颜色计算。两者配合完成了整个渲染过程。
static const char* shaderCodeFragment = R"(
#version 460 core
layout (location=0) in vec3 color;
layout (location=0) out vec4 out_FragColor;
void main()
{
	out_FragColor = vec4(color, 1.0);
};
)";

int main( void )
{
	//这是一个GLFW的错误回调，用了一个lambda输出错误内容
	//stderr是标准错误流，用于输出程序运行时的错误信息。与stdout（标准输出）不同，stderr通常不会被重定向，而是直接输出到控制台或终端。
	//在C语言中，通过使用stderr流，我们可以将错误信息输出到标准错误输出，以便用户(或开发者)能够及时地看到错误信息、调试程序或记录错误日志。
	glfwSetErrorCallback(
		[]( int error, const char* description )
		{
			fprintf( stderr, "Error: %s\n", description );
		}
	);

	//初始化GLFW
	if ( !glfwInit() )
		exit( EXIT_FAILURE );

	//下一步是告诉GLFW我们想要使用哪个版本的OpenGL。我们使用的是OpenGL 4.6
	/*
	GLFW_CONTEXT_VERSION_MAJOR:
	这个属性用于指定OpenGL的主版本号。在这段代码中，设置为4，表示使用OpenGL 4.x版本。

	GLFW_CONTEXT_VERSION_MINOR:
	这个属性用于指定OpenGL的次版本号。在这段代码中，设置为6，表示使用OpenGL 4.6版本。

	GLFW_OPENGL_PROFILE:
	这个属性用于指定OpenGL的配置文件类型。在这段代码中，设置为GLFW_OPENGL_CORE_PROFILE，表示使用核心配置文件。
	核心配置文件只包含了OpenGL的核心功能特性，而没有包含已经被废弃的旧版特性。
	*/
	glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
	glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 6 );
	glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );

	//创建一个窗口，1024*768，没有设置窗口的共享上下文和监视器
	GLFWwindow* window = glfwCreateWindow( 1024, 768, "Simple example", nullptr, nullptr );
	//如果窗口创建失败，就调用glfwTerminate函数终止GLFW库的使用，然后使用exit函数退出程序。
	if ( !window )
	{
		glfwTerminate();
		exit( EXIT_FAILURE );
	}
	
	//设置键盘回调函数。当有键盘事件发生时，会调用指定的Lambda表达式作为键盘回调函数。
	//通过if语句判断按下的键是否是ESC键（键码为GLFW_KEY_ESCAPE）且动作为按下（action为GLFW_PRESS）。
	glfwSetKeyCallback(
		window,
		[]( GLFWwindow* window, int key, int scancode, int action, int mods )
		{
			if ( key == GLFW_KEY_ESCAPE && action == GLFW_PRESS )
				glfwSetWindowShouldClose( window, GLFW_TRUE );
		}
	);

	/*
	glfwMakeContextCurrent(window):
	这个函数将指定的窗口设置为当前的上下文。这意味着后续的OpenGL函数调用都是针对这个窗口的。

	gladLoadGL(glfwGetProcAddress):
	这个函数用来加载OpenGL函数指针。它需要传入一个函数指针获取器的函数指针，这里使用了glfwGetProcAddress函数作为获取器。

	glfwSwapInterval(1):
	这个函数设置垂直同步（VSync）的间隔。参数1表示启用垂直同步，即将帧率限制为显示器的刷新率。如果参数为0，则禁用垂直同步。
	 */
	glfwMakeContextCurrent( window );
	gladLoadGL( glfwGetProcAddress );
	glfwSwapInterval( 1 );

	//在OpenGL中，GLuint是一个无符号整数类型，用于表示着色器（shader）、
	//纹理（texture）、缓冲区（buffer）等对象的唯一标识符。它通常作为OpenGL对象的句柄（handle）使用。

	//这两个着色器都应该被编译并链接到一个着色器程序。我们是这样做的:
	//1.顶点着色器
	const GLuint shaderVertex = glCreateShader( GL_VERTEX_SHADER );
	glShaderSource( shaderVertex, 1, &shaderCodeVertex, nullptr );
	glCompileShader( shaderVertex );
	//2.片断着色器
	const GLuint shaderFragment = glCreateShader( GL_FRAGMENT_SHADER );
	glShaderSource( shaderFragment, 1, &shaderCodeFragment, nullptr );
	glCompileShader( shaderFragment );

	//使用glAttachShader函数将顶点着色器和片段着色器附加到着色器程序中。
	const GLuint program = glCreateProgram();
	glAttachShader( program, shaderVertex );
	glAttachShader( program, shaderFragment );
	//使用glLinkProgram函数链接着色器程序
	glLinkProgram( program );
	//使用glUseProgram函数激活着色器程序，使其成为当前的渲染状态
	glUseProgram( program );

	//顶点数组对象
	GLuint vao;
	glCreateVertexArrays( 1, &vao );
	glBindVertexArray( vao );

	//清除颜色，设置全白不透明
	//注意，这段代码只是设置了清除颜色，它并没有立即生效。需要在渲染循环中的合适位置调用glClear(GL_COLOR_BUFFER_BIT)来清除颜色缓冲区，
	//并触发使用glClearColor设置的清除颜色。这样才能真正将指定的颜色呈现到屏幕上。
	glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );

	while ( !glfwWindowShouldClose( window ) )
	{
		//通过从GLFW读取当前宽度和高度并相应地更新OpenGL视口来实现可调整大小的窗口
		int width, height;
		glfwGetFramebufferSize( window, &width, &height );
		glViewport( 0, 0, width, height );
		
		//这个函数通常会在渲染循环的开始位置调用，以便在每一帧开始绘制之前先清除颜色缓冲区，以准备绘制新的内容。
		//需要注意的是，glClear只会清除指定的缓冲区，而不会填充新的颜色或数据。在使用glClear之前，通常需要先设置
		//清除颜色 glClearColor，以指定要清除的颜色。然后，使用glDrawArrays或其他绘制函数来绘制图形，并使用glSwapBuffers或
		//其他相关的函数将绘制结果显示到屏幕上。
		glClear( GL_COLOR_BUFFER_BIT );
		
		/*
			glDrawArrays(GL_TRIANGLES, 0, 3);
			glDrawArrays是一个OpenGL函数，用于绘制图元。
			GL_TRIANGLES是一个枚举常量，表示绘制三角形图元。
			0是起始顶点索引，表示从顶点数组的第0个顶点开始绘制。
			3是顶点数量，表示需要绘制3个顶点。
		*/
		glDrawArrays( GL_TRIANGLES, 0, 3 );
		
		//交换前后缓冲区，即将当前帧的渲染结果显示到屏幕上。
		glfwSwapBuffers( window );

		//glfwPollEvents()会检查窗口是否有未处理的事件，比如键盘输入、鼠标移动等。如果有未处理的事件，它会触发相应的回调函数来处理这些事件。
		glfwPollEvents();
	}

	glDeleteProgram( program );
	glDeleteShader( shaderFragment );
	glDeleteShader( shaderVertex );
	glDeleteVertexArrays( 1, &vao );

	glfwDestroyWindow( window );
	glfwTerminate();

	return 0;
}
