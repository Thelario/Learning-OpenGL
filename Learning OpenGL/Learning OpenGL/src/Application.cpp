#include <GL/glew.h> // We need to include glew.h before any OpenGL .h file
#include <GLFW/glfw3.h>
#include <iostream>

int main(void)
{

#pragma region Initializing GLFW & GLEW

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

#pragma endregion

#pragma region Vertex Buffer

    /* CREATING A VERTEX BUFFER THAT CONTAINS THE DATA TO DRAW A TRIANGLE */

    /* These are going to be the vertices of the triangle. */
    float positions[6] = {
        -0.5f, -0.5f,
         0.0f,  0.5f,
         0.5f, -0.5f
    };

    /* In this case, we are returning 1 buffer object into buffer. */
    unsigned int buffer;
    glGenBuffers(1, &buffer); 
    
    /* Once we have generated a buffer, we need to select it, and that is done with glBindBuffer. */
    /* In this case, we are binding the buffer into GL_ARRAY_BUFFER, which simply means that the */
    /* purpose of the buffer will be of being an array of Vertex Attributes. */
    glBindBuffer(GL_ARRAY_BUFFER, buffer); 

    /* We now need to give data to the buffer. This can be done either by directly giving data to the buffer */
    /* or we can give it nothing and later pass the data. We are going to provide data straighaway. */
    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), &positions, GL_STATIC_DRAW);

#pragma endregion

#pragma region Main Loop

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

#pragma region Drawing 

        /* We can draw in two different ways: */
        /* 1) glDrawArrays: we use this if we do not have an IndexBuffer. */
        glDrawArrays(GL_TRIANGLES, 0, 3);

        /* 2) glDrawElements: we use this if we have an IndexBuffer. */
        /* glDrawElements(); */

        /* How does OpenGL know that it has to draw the triangle specified before? */
        /* OpenGL will draw whatever is currently bound. In our case, we called to */
        /* glBindBuffer before, and haven't bound anything else, so it will draw the */
        /* triangle that has been passed to the buffer. */
        /* If before having called to glDrawArrays we had bound a different buffer, then */
        /* OpenGL would draw what is contained in that buffer, and not the triangle we want to draw.*/

#pragma endregion

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;

#pragma endregion
}