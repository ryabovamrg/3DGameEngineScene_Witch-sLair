#include "ModelLoader.h"
#include <fstream>
#include <string>
#include <vector>
#include <stdexcept>
#include <stdio.h>

#include <cstdlib>
#include <sstream>
#include <iostream>
#include <iomanip>

#include <unordered_map>

namespace ryabomar {
	/* spilts string into array of strings by delimeter*/
	std::vector<std::string> splitString(const std::string& string, const char delimiter = ' ') {

		std::vector<std::string> splits;

		size_t cur_pos = 0, prev_pos = 0;
		cur_pos = string.find(delimiter);
		while (cur_pos != std::string::npos) {
			splits.push_back(string.substr(prev_pos, cur_pos - prev_pos));
			prev_pos = cur_pos + 1;
			cur_pos = string.find(delimiter, prev_pos);
		}
		splits.push_back(string.substr(prev_pos, cur_pos - prev_pos));
		return splits;
	}


	pgr::MeshData ModelLoader::loadMesh(const char* pathToFile) {
		// string copy
		std::string path(pathToFile);

		bool smooth = false;

		struct V { float x, y, z; };	//struct for vertex
		struct VT { float x, y; };		//struct for texcoord
		struct VN { float x, y, z; };	//struct for normal


		//struct for face
		struct F {
			struct facePoint {
				// idx of vert, idx of normal, idx of texcoord
				unsigned v_idx, vn_idx, vt_idx;
			};

			facePoint point_a, point_b, point_c;
		};

		std::vector<V>  readVs;
		std::vector<VT> readVTs;
		std::vector<VN> readVNs;
		std::vector<F>  readFs;


		// read file --------------------------------------------------------------------------
		std::ifstream file(path, std::ios::in);
		if (!file.is_open()) throw std::runtime_error(std::string("could not open file: ") + path);

		std::string line;

		//read file line by line
		while (std::getline(file, line)) {

			if (line[0] == '#') continue; // skip comment


			// spilt line into vector of strings
			std::vector<std::string> splits = splitString(line);

			// splits[0] contains type, splits[1+] contain data (depending on type)

			if (splits[0] == "v") {
				//in type == v --> await 3 floats
				// stof = string to float
				readVs.push_back({ std::stof(splits[1]), std::stof(splits[2]), std::stof(splits[3]) });
			}

			if (splits[0] == "vt") {
				//in type == vt --> await 2 floats
				readVTs.push_back({ std::stof(splits[1]), std::stof(splits[2]) });
			}

			if (splits[0] == "vn") {
				//in type == vn --> await 3 floats
				readVNs.push_back({ std::stof(splits[1]), std::stof(splits[2]), std::stof(splits[3]) });
			}

			if (splits[0] == "f") {

				// f 2/1/1 1/2/1 3/3/1
				// in type == vt --> 
				// splits[1] = 2/1/1
				// splits[2] = 1/2/1
				// splits[3] = 3/3/1


				// we need to split again with divider '/'
				std::vector<std::string> point_a = splitString(splits[1], '/');
				std::vector<std::string> point_b = splitString(splits[2], '/');
				std::vector<std::string> point_c = splitString(splits[3], '/');

				// our structure for one face 
				F face;

				// 1st point in face, point a
				face.point_a.v_idx = (unsigned)std::stoi(point_a[0]);
				face.point_a.vt_idx = (unsigned)std::stoi(point_a[1]);
				face.point_a.vn_idx = (unsigned)std::stoi(point_a[2]);

				// 2nd point in face, point b
				face.point_b.v_idx = (unsigned)std::stoi(point_b[0]);
				face.point_b.vt_idx = (unsigned)std::stoi(point_b[1]);
				face.point_b.vn_idx = (unsigned)std::stoi(point_b[2]);

				// 3rd point in face, point c
				face.point_c.v_idx = (unsigned)std::stoi(point_c[0]);
				face.point_c.vt_idx = (unsigned)std::stoi(point_c[1]);
				face.point_c.vn_idx = (unsigned)std::stoi(point_c[2]);

				//pushing the final face to the vector of faces
				readFs.push_back(face);
			}

			// should smooth normals or not
			if (splits[0] == "s") {
				if (splits[1] == "on")
					smooth = true;
				if (splits[1] == "off")
					smooth = false;
			}
		}
		file.close();
		//-------------------------------------------------------------------------------------


		// convert to MeshData ----------------------------------------------------------------

		// one line of future VBO
		struct meshData_Vertex { float vert_x, vert_y, vert_z, norm_x, norm_y, norm_z, texcoord_x, texcoord_y; };
		// one line of future EBO
		struct meshData_Triangle { unsigned a, b, c; };

		std::vector<meshData_Vertex>    verticies;
		std::vector<meshData_Triangle>  triangles;

		//smooth		KEY = old index from readFs to pos in readVs     VALUE = vector of idxs from triangles to verticies
		std::unordered_map < unsigned, std::vector<unsigned>> vert_to_smooth;


		// f 2/1/1 1/2/1 3/3/1
		for (F face : readFs) {

			meshData_Vertex vertex[3];

			//position
			vertex[0].vert_x = readVs[face.point_a.v_idx - 1].x; // - 1 because indexes in .obj start from 1
			vertex[0].vert_y = readVs[face.point_a.v_idx - 1].y;
			vertex[0].vert_z = readVs[face.point_a.v_idx - 1].z;

			vertex[1].vert_x = readVs[face.point_b.v_idx - 1].x;
			vertex[1].vert_y = readVs[face.point_b.v_idx - 1].y;
			vertex[1].vert_z = readVs[face.point_b.v_idx - 1].z;

			vertex[2].vert_x = readVs[face.point_c.v_idx - 1].x;
			vertex[2].vert_y = readVs[face.point_c.v_idx - 1].y;
			vertex[2].vert_z = readVs[face.point_c.v_idx - 1].z;

			//normals
			if (!readVNs.empty()) {
				vertex[0].norm_x = readVNs[face.point_a.vn_idx - 1].x;
				vertex[0].norm_y = readVNs[face.point_a.vn_idx - 1].y;
				vertex[0].norm_z = readVNs[face.point_a.vn_idx - 1].z;

				vertex[1].norm_x = readVNs[face.point_b.vn_idx - 1].x;
				vertex[1].norm_y = readVNs[face.point_b.vn_idx - 1].y;
				vertex[1].norm_z = readVNs[face.point_b.vn_idx - 1].z;

				vertex[2].norm_x = readVNs[face.point_c.vn_idx - 1].x;
				vertex[2].norm_y = readVNs[face.point_c.vn_idx - 1].y;
				vertex[2].norm_z = readVNs[face.point_c.vn_idx - 1].z;
			}

			//texcoords
			if (!readVTs.empty()) {

				vertex[0].texcoord_x = readVTs[face.point_a.vt_idx - 1].x;
				vertex[0].texcoord_y = readVTs[face.point_a.vt_idx - 1].y;

				vertex[1].texcoord_x = readVTs[face.point_b.vt_idx - 1].x;
				vertex[1].texcoord_y = readVTs[face.point_b.vt_idx - 1].y;

				vertex[2].texcoord_x = readVTs[face.point_c.vt_idx - 1].x;
				vertex[2].texcoord_y = readVTs[face.point_c.vt_idx - 1].y;
			}

			verticies.push_back(vertex[0]);
			verticies.push_back(vertex[1]);
			verticies.push_back(vertex[2]);

			unsigned idx = verticies.size();

			meshData_Triangle tr;
			tr.a = idx - 3;
			tr.b = idx - 2;
			tr.c = idx - 1;

			vert_to_smooth[face.point_a.v_idx].push_back(tr.a);
			vert_to_smooth[face.point_b.v_idx].push_back(tr.b);
			vert_to_smooth[face.point_c.v_idx].push_back(tr.c);

			triangles.push_back(tr);
		}
		// ------------------------------------------------------------------------------------


		// smooth normals ---------------------------------------------------------------------
		// loop through all verticex and make their normals averege
		if (smooth) for (auto vert : vert_to_smooth) {
			glm::vec3 smooth_normal = glm::vec3(0.0f);

			for (unsigned idx : vert.second) {
				smooth_normal.x += verticies[idx].norm_x;
				smooth_normal.y += verticies[idx].norm_y;
				smooth_normal.z += verticies[idx].norm_z;
			}

			smooth_normal = glm::normalize(smooth_normal);

			for (unsigned idx : vert.second) {
				verticies[idx].norm_x = smooth_normal.x;
				verticies[idx].norm_y = smooth_normal.y;
				verticies[idx].norm_z = smooth_normal.z;
			}
		}
		// ------------------------------------------------------------------------------------


		// construct MeshData -----------------------------------------------------------------
		float* _verticesInterleaved = (float*)malloc(sizeof(meshData_Vertex) * verticies.size());
		unsigned* _triangles = (unsigned*)malloc(sizeof(meshData_Triangle) * triangles.size());

		// copy			TO						FROM								SIZE IN BITES
		std::memcpy(_verticesInterleaved, (float*)verticies.data(), sizeof(meshData_Vertex) * verticies.size());
		std::memcpy(_triangles, (unsigned*)triangles.data(), sizeof(meshData_Triangle) * triangles.size());

		pgr::MeshData meshData = { (unsigned)verticies.size(), (unsigned)triangles.size(), 8, _verticesInterleaved, _triangles };
		// ------------------------------------------------------------------------------------

		return meshData;
	}

	//!!!!!!!!!!!!!!!



	/*
	Ns 900.000000
	Ka 1.000000 1.000000 1.000000  // ambient
	Kd 0.800000 0.800000 0.800000  // diffuse
	Ks 0.000000 0.000000 0.000000  // specular
	Ke 0.000000 0.000000 0.000000  // specular exponent
	Ni 1.450000					   // emition
	d 1.000000					   // Specifies the dissolve for the current material. 1.0 is fully opaque.
	illum 1						   // illumination model
	map_Kd -s 0.100000 0.100000 0.150000 textures\\a09df4cb10936bf2675a0fcdeb0248cd.jpg
	*/

	/* SHININESS??? WHERE ARE YOU*/


	PhongMaterial ModelLoader::loadMaterial(const char* pathToFile) {
		PhongMaterial loadedMaterial;
		loadedMaterial.textureScale = glm::vec3(1.0f);

		std::string line;
		std::ifstream myfile(pathToFile);
		if (myfile.is_open())
		{
			for (std::string line; std::getline(myfile, line); )
			{
				std::istringstream in(line);
				std::string type;
				in >> type;

				if (type == "map_Kd") {
					std::string param;
					in >> param;
					if (param == "-s") {
						float x, y, z;
						in >> x >> y >> z;
						loadedMaterial.textureScale = glm::vec3(x, y, z);

					}
				}

				if (type == "Ns")
				{

					float x;
					in >> x;
					loadedMaterial.shininess = x;
				}
				if (type == "Ka")
				{

					float x, y, z;
					in >> x >> y >> z;
					loadedMaterial.ambient = glm::vec3(x, y, z);
				}
				if (type == "Kd")
				{

					float x, y, z;
					in >> x >> y >> z;
					loadedMaterial.diffuse = glm::vec3(x, y, z);
				}
				if (type == "Ks")
				{

					float x, y, z;
					in >> x >> y >> z;
					loadedMaterial.specular = glm::vec3(x, y, z);
				}
				if (type == "Ke")
				{
					float x, y, z;
					in >> x >> y >> z;
					loadedMaterial.specularExponent = glm::vec3(x, y, z);
				}
			}
		}

		//loadedMaterial.shininess = 0.5f;

		return loadedMaterial;

	}


	/*
	pgr::MeshData ModelLoader::makeTerrainMeshFromHightmap(const char* pathToHightmap) {
		struct Triangle { unsigned a, b, c; };
		struct Vertex { float v_x, v_y, v_z, n_x, n_y, n_z, t_x, t_y; };



		pgr::MeshData md;
		return md;
	}

	*/

}