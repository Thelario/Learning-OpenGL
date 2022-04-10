#include <GL/glew.h> // We need to include glew.h before any OpenGL .h file
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#define ASSERT(x) if (!(x)) __debugbreak(); // Compiler intrinsic function
#define GLCall(x) GLClearErrors();\
    x;\
    ASSERT(GLLogCall(#x, __FILE__, __LINE__)) 

// #x turns the function name into a string
// __FILE__ indicates the file from which the function was called
// __LINE__ indicates the line of the function that was called

static void GLClearErrors()
{
    while (glGetError() != GL_NO_ERROR);
}

static bool GLLogCall(const char* function, const char* file, int line)
{
    while (GLenum error = glGetError())
    {
        std::cout << "[OpenGL Error] (" << error << "): " << function << " " << file << ":" << line << std::endl;
        return false;
    }
    return true;
}


struct ShaderProgramSource
{
    std::string VertexSource;
    std::string FragmentSource;
};

static ShaderProgramSource ParseShader(const std::string& filepath)
{
    std::ifstream stream(filepath);

    enum class ShaderType { NONE = -1, VERTEX = 0, FRAGMENT = 1 };

    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;

    while (getline(stream, line))
    {
        if (line.find("#shader") != std::string::npos)
        {
            if (line.find("vertex") != std::string::npos)
            {
                type = ShaderType::VERTEX;
            }
            else if (line.find("fragment") != std::string::npos)
            {
                type = ShaderType::FRAGMENT;
            }
        }
        else
        {
            ss[(int)type] << line << '\n';
        }
    }

    return { ss[0].str(), ss[1].str() };
}

static unsigned int CompileShader(unsigned int type, const std::string& source)
{
    // glCreateShader creates an empty shader object and returns a non-zero value by which it can be referenced. 
    // A shader object is used to maintain the source code strings that define a shader.
    unsigned int id = glCreateShader(type);

    const char* src = source.c_str(); // This is a pointer to the beginning of the string data

    // glShaderSource sets the source code in shader to the source code in the array of strings specified by string.
    // Any source code previously stored in the shader object is completely replaced.The number of strings in the array is specified by count.
    // If length is NULL, each string is assumed to be null terminated.
    // If length is a value other than NULL, it points to an array containing a string length for each of the corresponding elements of string.
    glShaderSource(id, 1, &src, nullptr);

    // glCompileShader compiles the source code strings that have been stored in the shader object specified by shader.
    glCompileShader(id);

    // The next code is used for checking any possible error in the compilation of the shader
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) // The shader has not compiled successfully
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char)); // alloca is a function that allocates memory dynamically
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!" << std::endl;
        std::cout << &message << std::endl;
        glDeleteShader(id);
        return 0;
    }

    return id;
}

// This function will provide OpenGL with the source code of the vertex shader and the fragment shader.
// And we want OpenGL to compile that program and link them together, and give a unique identifier for that shader back.
static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    // glCreateProgram creates an empty program object and returns a non-zero value by which it can be referenced. 
    // A program object is an object to which shader objects can be attached. This provides a mechanism to specify the shader objects 
    // that will be linked to create a program. It also provides a means for checking the compatibility of the shaders that will 
    // be used to create a program (for instance, checking the compatibility between a vertex shader and a fragment shader). 
    // When no longer needed as part of a program object, shader objects can be detached.
    unsigned int program = glCreateProgram();

    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    // In order to create a complete shader program, there must be a way to specify the list of things that will be linked together.
    // Program objects provide this mechanism. Shaders that are to be linked together in a program object must first be attached to that program object.
    // glAttachShader attaches the shader object specified by shader to the program object specified by program.This indicates that shader will 
    // be included in link operations that will be performed on program.
    GLCall(glAttachShader(program, vs));
    GLCall(glAttachShader(program, fs));

    // glLinkProgram links the program object specified by program. 
    // If any shader objects of type GL_VERTEX_SHADER are attached to program, they will be used to create an executable that will run on the programmable vertex processor. 
    // If any shader objects of type GL_GEOMETRY_SHADER are attached to program, they will be used to create an executable that will run on the programmable geometry processor. 
    // If any shader objects of type GL_FRAGMENT_SHADER are attached to program, they will be used to create an executable that will run on the programmable fragment processor.
    GLCall(glLinkProgram(program));

    // glValidateProgram checks to see whether the executables contained in program can execute given the current OpenGL state. 
    // The information generated by the validation process will be stored in program's information log. 
    // The validation information may consist of an empty string, or it may be a string containing information about how the current program object 
    // interacts with the rest of current OpenGL state. This provides a way for OpenGL implementers to convey more information about why the current 
    // program is inefficient, suboptimal, failing to execute, and so on.
    GLCall(glValidateProgram(program));

    GLCall(glDeleteShader(vs));
    GLCall(glDeleteShader(fs));

    return program;
}

int main(void)
{

    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    /* This function sets the swap interval for the current OpenGL or OpenGL ES context, i.e. the number of screen updates */
    /* to wait from the time glfwSwapBuffers was called before swapping the buffers and returning. */
    /* This is sometimes called vertical synchronization, vertical retrace synchronization or just vsync.*/
    glfwSwapInterval(1);

    /* Initializing GLEW */
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        /* Problem: glewInit failed, something is seriously wrong. */
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
    }

    /* We are printing the version for both OpenGL and GLEW */
    fprintf(stdout, "Status: Using OpenGL %s\n", glGetString(GL_VERSION));
    fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));

    /* CREATING A VERTEX BUFFER THAT CONTAINS THE DATA TO DRAW A TRIANGLE */

    /* These are going to be the vertices of the triangle. */
    /* Vertices may contain way more data than just the position. */
    /* When talking about the position, we will be talking about Vertex Position, not only about Vertex. */
    float positions[8] = {
        -0.5f, -0.5f,
         0.5f, -0.5f,
         0.5f,  0.5f,
        -0.5f, 0.5f
    };

    /* These indices are the ones that we will be using to draw the rectangle, indicating the different vertices. */
    unsigned int indices[6] = {
        0, 1, 2,
        2, 3, 0
    };

    /* In this case, we are returning 1 buffer object into buffer. */
    unsigned int buffer;
    GLCall(glGenBuffers(1, &buffer));
    
    /* Once we have generated a buffer, we need to select it, and that is done with glBindBuffer. */
    /* In this case, we are binding the buffer into GL_ARRAY_BUFFER, which simply means that the */
    /* purpose of the buffer will be of being an array of Vertex Attributes. */
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, buffer));

    /* Once we have bound the buffer, we need to tell OpenGL how the layout of the data is going to be. */
    /* This means whether the vertices are going to be in 2D or in 3D, and how to treat them. */

    /* We first need to enable the Vertex Attribute. */
    GLCall(glEnableVertexAttribArray(0));

    /* Then we need to actually create the Vertex Attribute. */
    /* 1) index */
    /* 2) size = 2 because we are in 2D, and each vertex will have two floats. */
    /* 3) type = GL_FLOAT because we are working with floats. */
    /* 5) stride = the amount of bytes between each vertex, which is the size of the vertex position plus the size of the texture coordinate plus the size of the normal. */
    /* 6) pointer = pointer into the actual attribute. */
    /*    The offset for the position will be 0, the offset for the texture coordinate will be 12, and the offset for the normal will be 20. */
    GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0));

    /* We now need to give data to the buffer. This can be done either by directly giving data to the buffer */
    /* or we can give it nothing and later pass the data. We are going to provide data straighaway. */
    GLCall(glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(float), positions, GL_STATIC_DRAW));

    /* Sending the indices data to the gpu. */
    unsigned int ibo;
    GLCall(glGenBuffers(1, &ibo));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW));

    /* Creating a shader */
    ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");
    unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
    GLCall(glUseProgram(shader)); // glUseProgram installs the program object specified by program as part of current rendering state

    GLCall(int location = glGetUniformLocation(shader, "u_Color"));
    ASSERT(location != -1);
    GLCall(glUniform4f(0, 1.0f, 0.5f, 0.5f, 1.0f));

    float r = 0.0f;
    float increment = 0.05f;

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        /* We are going to change color over time of our rectangle. */
        GLCall(glUniform4f(0, r, 0.5f, 0.5f, 1.0f));

        r += increment;
        if (r >= 1.0f)
            increment *= -1.0f;
        else if (r <= 0.0f)
            increment *= -1.0f;

        /* We can draw in two different ways: */
        /* 1) glDrawArrays: we use this if we do not have an IndexBuffer. */
        // glDrawArrays(GL_TRIANGLES, 0, 3);

        /* 2) glDrawElements: we use this if we have an IndexBuffer. */
        // GLClearErrors();
        // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
        // ASSERT(GLLogCall());

        GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

        /* How does OpenGL know that it has to draw the triangle specified before? */
        /* OpenGL will draw whatever is currently bound. In our case, we called to */
        /* glBindBuffer before, and haven't bound anything else, so it will draw the */
        /* triangle that has been passed to the buffer. */
        /* If before having called to glDrawArrays we had bound a different buffer, then */
        /* OpenGL would draw what is contained in that buffer, and not the triangle we want to draw.*/

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glDeleteProgram(shader);

    glfwTerminate();
    return 0;
}