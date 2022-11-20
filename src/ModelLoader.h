#pragma once
#include "pgr.h"
#include "Types.h"
namespace ryabomar {
	// http://www.opengl-tutorial.org/ru/beginners-tutorials/tutorial-7-model-loading/

	using namespace std;
	/**
	Loader of models' meshes in .obj format and materials in .mtl format
	*/
	class ModelLoader
	{
	public:
		/**
		Loads the mesh from given path to .obj file

		\return MeshData	a stucrure containing vertices and faces from .obj file
		*/
		static pgr::MeshData loadMesh(const char* pathToFile);


		/**
		Loads the materila from given path to .mtl file

		\return PhongMaterial	a stucrure containing material values
		*/
		static PhongMaterial loadMaterial(const char* pathToFile);
	};

}