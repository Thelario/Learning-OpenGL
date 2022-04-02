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
    /* Vertices may contain way more data than just the position. */
    /* When talking about the position, we will be talking about Vertex Position, not only about Vertex. */
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

    /* Once we have bound the buffer, we need to tell OpenGL how the layout of the data is going to be. */
    /* This means whether the vertices are going to be in 2D or in 3D, and how to treat them. */

    /* We first need to enable the Vertex Attribute. */
    glEnableVertexAttribArray(0);

    /* Then we need to actually create the Vertex Attribute. */
    /* 1) index */
    /* 2) size = 2 because we are in 2D, and each vertex will have two floats. */
    /* 3) type = GL_FLOAT because we are working with floats. */
    /* 5) stride = the amount of bytes between each vertex, which is the size of the vertex position plus the size of the texture coordinate plus the size of the normal. */
    /* 6) pointer = pointer into the actual attribute. */
    /*    The offset for the position will be 0, the offset for the texture coordinate will be 12, and the offset for the normal will be 20. */
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);

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