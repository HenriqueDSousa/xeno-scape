#include "VertexArray.h"
#include <GL/glew.h>

VertexArray::VertexArray(const float* verts, unsigned int numVerts, const unsigned int* indices,
						 unsigned int numIndices)
: mNumVerts(numVerts)
, mNumIndices(numIndices)
, mVertexBuffer(0)
, mIndexBuffer(0)
, mVertexArray(0)
{
	// Create VAO
	glGenVertexArrays(1, &mVertexArray);
	glBindVertexArray(mVertexArray);

	// Create VBO
	glGenBuffers(1, &mVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, numVerts * 4 * sizeof(float), verts, GL_STATIC_DRAW);

	// Create EBO
	glGenBuffers(1, &mIndexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(GLuint), indices, GL_STATIC_DRAW);

	// Vertex attributes
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0,                  // location = 0
		2,                  // 2 floats (x, y)
		GL_FLOAT,
		GL_FALSE,
		4 * sizeof(float),  // stride: 4 floats
		(void*)0 // offset
	);

	// Texture attributes
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(
		1,                                      // location = 1
		2,                                      // 2 floats (u, v)
		GL_FLOAT,
		GL_FALSE,
		4 * sizeof(float),                      // stride: 4 floats
		(void*) (2 * sizeof(float)) // offset
	);
}

VertexArray::~VertexArray()
{
	glDeleteBuffers(1, &mVertexBuffer);
	glDeleteBuffers(1, &mIndexBuffer);
	glDeleteVertexArrays(1, &mVertexArray);
}

void VertexArray::SetActive() const
{
	glBindVertexArray(mVertexArray);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);
}
