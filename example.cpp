#define RGE_IMPL
#define RGE_USE_STB_IMAGE_WRITE
#define RGE_USE_STB_IMAGE
#define SYS_OPENGL_1_0
#include <rge/rge.hpp>

#include <iostream>
#include <fstream>

typedef struct model_t {
	std::vector<rge::vec3> vertices;
	std::vector<int> triangles;
	std::vector<rge::vec3> normals;
	std::vector<rge::vec2> uvs;
} model_t;

static std::vector<std::string> str_split(std::string& s, char sep) {
	std::vector<std::string> output;
	std::string::size_type prev_pos = 0, pos = 0;
	while((pos = s.find(sep, pos)) != std::string::npos) {
		std::string substring(s.substr(prev_pos, pos - prev_pos));
		output.push_back(substring);
		prev_pos = ++pos;
	}
	output.push_back(s.substr(prev_pos, pos - prev_pos));

	return output;
}

static model_t* load_obj(const char* fname) {
	std::ifstream fp_in;
	fp_in.open(fname, std::ios::in);
	if(!fp_in.is_open()) {
		std::cout << "Error opening mesh file" << std::endl;
		return nullptr;
	}

	model_t* mdl = new model_t();

	std::vector<rge::vec3> vertices;
	std::vector<rge::vec3> normals;
	std::vector<rge::vec2> uvs;

	std::string line;
	std::vector<std::string> comps;
	std::vector<std::string> face_comps;
	while(std::getline(fp_in, line)) {
		comps.clear();
		comps = str_split(line, ' ');
		if(comps.size() == 0)
			continue;

		if(comps[0] == "v") {
			if(comps.size() >= 4)
				vertices.push_back(rge::vec3(std::stof(comps[1]), std::stof(comps[2]), std::stof(comps[3])));
		} else if(comps[0] == "vn") {
			if(comps.size() >= 4)
				normals.push_back(rge::vec3(std::stof(comps[1]), std::stof(comps[2]), std::stof(comps[3])));
		} else if(comps[0] == "vt") {
			if(comps.size() >= 3)
				uvs.push_back(rge::vec2(std::stof(comps[1]), std::stof(comps[2])));
		} else if(comps[0] == "f") {
			for(int i = 1; i < 4; i++) {
				face_comps.clear();
				face_comps = str_split(comps[i], '/');
				int vi = 0, vti = 0, vni = 0;

				if(face_comps.size() > 0) {
					vi = std::stoi(face_comps[0]);
				} else if(face_comps.size() > 1) {
					vti = std::stoi(face_comps[1]);
				} else if(face_comps.size() > 2) {
					vni = std::stoi(face_comps[2]);
				}

				if(vti == 0) vti = vi;
				if(vni == 0) vni = vi;

				mdl->triangles.push_back(vertices.size());
				mdl->vertices.push_back(vertices[vi]);
				mdl->uvs.push_back(uvs[vti]);
				mdl->normals.push_back(normals[vni]);
			}
		}
	}

	vertices.clear();
	normals.clear();
	uvs.clear();
	comps.clear();
	face_comps.clear();
	fp_in.close();
	std::cout << " File successfully read." << std::endl;

	return mdl;
}

class game : public rge::engine {

private:
    ptr(rge::material) material;
	rge::renderer* renderer;
    rge::camera* camera;
	model_t* model;

	float counter;

public:
    game() : rge::engine() {
        material = alloc(rge::material)();
		camera = new rge::camera();
		model = nullptr;
		counter = 0;
    }

    void on_init() override {
		renderer = get_renderer();

		//model = load_obj("tests/cube.obj");

        // camera->set_perspective(60, 1.6F, 1.0F, 1000.0F);
		camera->set_orthographic(-16, 16, 10, -10, 0.0F, 100.0F);
		camera->transform->position = rge::vec3(0, 0, -10);
		camera->transform->rotation = rge::quaternion::identity();

        // renderer->set_target(render);
        renderer->set_camera(camera);
		renderer->set_ambience(rge::color(0.2F, 0.2F, 0.2F));

		material->diffuse = rge::color(1, 0, 1);
        material->texture = rge::texture::read_from_disk("tests/test.bmp");
    }

	void on_update(float delta_time) override {
		counter += delta_time;
		if(counter > 3.14F)
			counter = 0;

		//camera->transform->position = rge::vec3(5 * cosf(counter), 0, -5 * sinf(counter));
		//camera->transform->rotation = rge::quaternion::yaw_pitch_roll(-counter, 0, 0);
	}

    void on_render() override {
		renderer->clear(rge::color(0.8F, 0.4F, 0.4F));
		
		renderer->draw(*material->texture, rge::rect(0, 0, 16, 16));

		if(model) {
			renderer->draw(
				rge::mat4::trs(rge::vec3(0, 0, 0), rge::quaternion::yaw_pitch_roll(0, 0, 0), rge::vec3(1, 1, 1)),
				model->vertices,
				model->triangles,
				model->normals,
				model->uvs,
				*material
			);
		}
    }
};


int main(int argc, char** argv) {
    game* gm = new game();
	gm->create(true); // Multithreading off.

    while(gm->get_is_running()) {
        std::string cmd;
		std::cout << "> ";
        std::getline(std::cin, cmd);
        
        if(!gm->get_is_running())
            break;

        if(gm->command(cmd) == rge::FAIL) {
            rge::log::error("Unknown command!");
        }
    }

	gm->wait_for_exit();
	delete gm;
    return 0;
}