#include "DrawableNode.h"
#include <algorithm>


namespace ryabomar {

	DrawableNode::DrawableNode(GLuint shaderProgram, pgr::MeshData mesh, GLuint texture, PhongMaterial material,
		glm::vec3 position, glm::vec3 rotationAngles, glm::vec3 scalingFactors)
		: texture(texture), shaderProgram(shaderProgram), material(material)
	{
		this->isActive = true;
		this->position = position;
		this->rotation = rotationAngles;
		this->scale = scalingFactors;
		nVertices = mesh.nVertices;
		nTriangles = mesh.nTriangles;


		GLuint VBO, EBO;

		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		// reserve 1 slot for VBO
		glGenBuffers(1, &VBO);
		//select VBO
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		//send data to VBO
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh.nAttribsPerVertex * mesh.nVertices, mesh.verticesInterleaved, GL_STATIC_DRAW);

		glGenBuffers(1, &EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 3 * mesh.nTriangles, mesh.triangles, GL_STATIC_DRAW);

		/* layouts */
		GLint positionLoc = glGetAttribLocation(shaderProgram, "position");
		glEnableVertexAttribArray(positionLoc);
		glVertexAttribPointer(positionLoc, 3, GL_FLOAT, GL_FALSE, (sizeof(float) * 8), 0);
		//line size   offset inside one line

		GLint normalLoc = glGetAttribLocation(shaderProgram, "normal");
		glEnableVertexAttribArray(normalLoc);
		glVertexAttribPointer(normalLoc, 3, GL_FLOAT, GL_FALSE, (sizeof(float) * 8), (void*)(sizeof(float) * 3));

		GLint texcoordLoc = glGetAttribLocation(shaderProgram, "texcoord");
		glEnableVertexAttribArray(texcoordLoc);
		glVertexAttribPointer(texcoordLoc, 2, GL_FLOAT, GL_FALSE, (sizeof(float) * 8), (void*)(sizeof(float) * 6));

		//release VAO
		glBindVertexArray(0);
		//release texture
		glBindTexture(GL_TEXTURE_2D, 0);


	}

	void DrawableNode::draw(std::vector <LightNode*>& lights, Camera* camera, int width, int height) {
		//std::cout << "DRAW" << std::endl;
		//glEnable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glBindVertexArray(VAO);
		glBindTexture(GL_TEXTURE_2D, texture);
		glUseProgram(shaderProgram);

		GLuint tex0Loc = glGetUniformLocation(shaderProgram, "texture0");
		glUniform1i(tex0Loc, 0);
		GLuint tex1Loc = glGetUniformLocation(shaderProgram, "texture1");
		glUniform1i(tex1Loc, 1);


		GLuint projectionLoc = glGetUniformLocation(shaderProgram, "projection");
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(camera->getProjection(width, height)));

		GLuint viewLoc = glGetUniformLocation(shaderProgram, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(camera->getView()));

		GLuint modelLoc = glGetUniformLocation(shaderProgram, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(this->getModelTransform()));

		GLuint normalModelLoc = glGetUniformLocation(shaderProgram, "normalModel");
		glUniformMatrix4fv(normalModelLoc, 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(this->getModelTransform()))));

		/* Camera*/
		GLuint camPosLoc = glGetUniformLocation(shaderProgram, "camera.position");
		glUniform3fv(camPosLoc, 1, glm::value_ptr(camera->getGlobalPosition()));

		GLuint camDirLoc = glGetUniformLocation(shaderProgram, "camera.direction");
		glUniform3fv(camDirLoc, 1, glm::value_ptr(camera->getGlobalForward()));

		/*Phong material*/
		GLuint ambientLoc = glGetUniformLocation(shaderProgram, "material.ambient");
		glUniform3fv(ambientLoc, 1, glm::value_ptr(material.ambient));

		GLuint diffuseLoc = glGetUniformLocation(shaderProgram, "material.diffuse");
		glUniform3fv(diffuseLoc, 1, glm::value_ptr(material.diffuse));

		GLuint specularLoc = glGetUniformLocation(shaderProgram, "material.specular");
		glUniform3fv(specularLoc, 1, glm::value_ptr(material.specular));

		GLuint specularExponentLoc = glGetUniformLocation(shaderProgram, "material.specularExponent");
		glUniform3fv(specularExponentLoc, 1, glm::value_ptr(material.specularExponent));

		GLuint texScaleLoc = glGetUniformLocation(shaderProgram, "material.textureScale");
		glUniform3fv(texScaleLoc, 1, glm::value_ptr(material.textureScale));

		GLuint shininessLoc = glGetUniformLocation(shaderProgram, "material.shininess");
		glUniform1f(shininessLoc, material.shininess);

		/*light structure*/
		/*SUN*/
		char strPosition[] = "lights[0].position";
		char strDirection[] = "lights[0].direction";
		char strAmbient[] = "lights[0].ambient";
		char strDiffuse[] = "lights[0].diffuse";
		char strSpecular[] = "lights[0].specular";

		char strCutoff[] = "lights[0].cutOff";
		char strSpotExponent[] = "lights[0].spotExponent";
		char strConstant[] = "lights[0].constant";
		char strLinear[] = "lights[0].linear";
		char strQuadratic[] = "lights[0].quadratic";

		char idx = '0';
		GLuint lightsCountLoc = glGetUniformLocation(shaderProgram, "lightsCount");
		glUniform1i(lightsCountLoc, std::min((int)lights.size(), 10));

		for (int i = 0; i < std::min((int)lights.size(), 10); i++) {
			strPosition[7] = idx + i;
			strDirection[7] = idx + i;
			strAmbient[7] = idx + i;
			strDiffuse[7] = idx + i;
			strSpecular[7] = idx + i;

			strCutoff[7] = idx + i;
			strSpotExponent[7] = idx + i;
			strConstant[7] = idx + i;
			strLinear[7] = idx + i;
			strQuadratic[7] = idx + i;

			GLuint sunPosLoc = glGetUniformLocation(shaderProgram, strPosition);
			glUniform3fv(sunPosLoc, 1, glm::value_ptr(lights[i]->getGlobalPosition()));
			GLuint sunDirLoc = glGetUniformLocation(shaderProgram, strDirection);
			glUniform3fv(sunDirLoc, 1, glm::value_ptr(lights[i]->getGlobalForward()));
			GLuint sunAmbientLoc = glGetUniformLocation(shaderProgram, strAmbient);
			glUniform3fv(sunAmbientLoc, 1, glm::value_ptr(lights[i]->ambient));
			GLuint sunDiffuseLoc = glGetUniformLocation(shaderProgram, strDiffuse);
			glUniform3fv(sunDiffuseLoc, 1, glm::value_ptr(lights[i]->diffuse));
			GLuint sunSpecularLoc = glGetUniformLocation(shaderProgram, strSpecular);
			glUniform3fv(sunSpecularLoc, 1, glm::value_ptr(lights[i]->specular));

			GLuint spotlightCutoffrLoc = glGetUniformLocation(shaderProgram, strCutoff);
			glUniform1f(spotlightCutoffrLoc, lights[i]->cutOff);
			GLuint spotlightExpLoc = glGetUniformLocation(shaderProgram, strSpotExponent);
			glUniform1f(spotlightExpLoc, lights[i]->spotExponent);


			GLuint pointlight1ConstLoc = glGetUniformLocation(shaderProgram, strConstant);
			glUniform1f(pointlight1ConstLoc, (lights[i]->constant));
			GLuint pointlight1LinearLoc = glGetUniformLocation(shaderProgram, strLinear);
			glUniform1f(pointlight1LinearLoc, (lights[i]->linear));
			GLuint pointlight1QuadraticLoc = glGetUniformLocation(shaderProgram, strQuadratic);
			glUniform1f(pointlight1QuadraticLoc, (lights[i]->quadratic));
		}


		/* Time*/
		GLuint deltaTimeLoc = glGetUniformLocation(shaderProgram, "deltaTime");
		glUniform1f(deltaTimeLoc, Parameters::getInstance().deltaTime);
		GLuint elapsedTimeLoc = glGetUniformLocation(shaderProgram, "elapsedTime");
		glUniform1f(elapsedTimeLoc, Parameters::getInstance().timeElapsed);


		glDrawElements(GL_TRIANGLES, nTriangles * 3, GL_UNSIGNED_INT, 0);

		//glBlendFunc(GL_ONE, GL_ZERO);
		glUseProgram(0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindVertexArray(0);
	}
	bool DrawableNode::shouldDraw() {
		return isActive;
	}
}