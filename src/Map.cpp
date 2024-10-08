//std
#include <omp.h>
#include <cmath>
#include <cstdio>
#include <chrono>
#include <filesystem>

//tensegrity
#include "Tensegrity/inc/Map.hpp"
#include "Tensegrity/inc/Solver.hpp"

//constructors
Map::Map(void) : 
	m_force(0.00e+00), m_mode(0), m_mesh_angle(100), m_mesh_radius(100), 
	m_data_state(nullptr), m_data_cables(nullptr), m_data_energy(nullptr)
{
	m_base.solver()->log(false);
}

//destructor
Map::~Map(void)
{
	delete[] m_data_state;
	delete[] m_data_cables;
	delete[] m_data_energy;
}

//data
Tensegrity& Map::base(void)
{
	return m_base;
}

double Map::force(void) const
{
	return m_force;
}
double Map::force(double force)
{
	return m_force = force;
}

double Map::tension(void) const
{
	return m_base.tension();
}
double Map::tension(double tension)
{
	return m_base.tension(tension);
}

uint32_t Map::mode(void) const
{
	return m_mode;
}
uint32_t Map::mode(uint32_t mode)
{
	return m_mode = mode;
}

uint32_t Map::cables(uint32_t cables)
{
	return m_base.cables(cables);
}
uint32_t Map::cables(void) const
{
	return m_base.cables();
}

uint16_t Map::mesh_angle(void) const
{
	return m_mesh_angle;
}
uint16_t Map::mesh_angle(uint32_t mesh_angle)
{
	return m_mesh_angle = mesh_angle;
}

uint16_t Map::mesh_radius(void) const
{
	return m_mesh_radius;
}
uint16_t Map::mesh_radius(uint32_t mesh_radius)
{
	return m_mesh_radius = mesh_radius;
}

//solve
void Map::solve(void)
{
	//data
	bool test;
	const double Pr = m_force;
	using namespace std::chrono;
	const uint32_t na = m_mesh_angle;
	const uint32_t nr = m_mesh_radius;
	const uint32_t nc = m_base.cables();
	const uint32_t nt = omp_get_max_threads();
	const double Ht = m_base.height_total();
	const double dp = m_base.plate_diameter();
	const double dc = m_base.cables_diameter();
	Tensegrity* tensegrities = new Tensegrity[nt];
	const high_resolution_clock::time_point t1 = high_resolution_clock::now();
	//setup
	for(uint32_t i = 0; i < nt; i++)
	{
		tensegrities[i] = m_base;
		tensegrities[i].add_load({0, 0, -Pr}, {});
	}
	//solve
	setup();
	for(uint32_t i = 0; i < nr; i++)
	{
		test = true;
		#pragma omp parallel for
		for(int32_t j = 0; j < int32_t(na); j++)
		{
			//data
			const double t = 2 * M_PI * j / na;
			const double r = dp / 2 * (i + 1) / nr;
			Tensegrity& tensegrity = tensegrities[omp_get_thread_num()];
			//solve
			tensegrity.load_position(0, {r * cos(t), r * sin(t), Ht});
			if(m_mode == 1) tensegrity.load(0, {+Pr * cos(t), +Pr * sin(t), 0});
			if(m_mode == 2) tensegrity.load(0, {-Pr * sin(t), +Pr * cos(t), 0});
			tensegrity.solver()->solve();
			if(!tensegrity.solver()->equilibrium()) test = false;
			//save
			m_data_state[8 * (i * na + j) + 0] = r;
			m_data_state[8 * (i * na + j) + 1] = t;
			m_data_energy[3 * (i * na + j) + 0] = r;
			m_data_energy[3 * (i * na + j) + 1] = t;
			m_data_cables[(nc + 3) * (i * na + j) + 0] = r;
			m_data_cables[(nc + 3) * (i * na + j) + 1] = t;
			const math::vec3 ud = tensegrity.solver()->state(true);
			const math::vec3 rd = math::quat(tensegrity.solver()->state(true) + 3).pseudo();
			for(uint32_t k = 0; k < 3; k++)
			{
				m_data_state[8 * (i * na + j) + k + 2] = ud[k];
				m_data_state[8 * (i * na + j) + k + 5] = rd[k];
			}
			for(uint32_t k = 0; k <= nc; k++)
			{
				m_data_cables[(nc + 3) * (i * na + j) + k + 2] = tensegrity.cable_force(k);
			}
			m_data_energy[3 * (i * na + j) + 2] = tensegrity.internal_energy();
		}
		if(!test) break;
		printf("mode: %d cables: %d radius: %d\n", m_mode, m_base.cables(), i);
	}
	if(test)
	{
		write_state();
		write_cables();
		write_energy();
	}
	const high_resolution_clock::time_point t2 = high_resolution_clock::now();
	const double td = (double) duration_cast<milliseconds>(t2 - t1).count();
	printf("Solution computed in %lf seconds\n", td / 1e3);
}

//setup
void Map::setup(void)
{
	//data
	const uint32_t na = m_mesh_angle;
	const uint32_t nr = m_mesh_radius;
	const uint32_t nc = m_base.cables();
	//memory
	delete[] m_data_state;
	delete[] m_data_cables;
	delete[] m_data_energy;
	m_data_state = new double[8 * na * nr];
	m_data_energy = new double[3 * na * nr];
	m_data_cables = new double[(nc + 3) * na * nr];
	//folder
	char path[200];
	this->path(path);
	std::filesystem::create_directory(path);
}
void Map::write_state(void) const
{
	//data
	const uint32_t na = m_mesh_angle;
	const uint32_t nr = m_mesh_radius;
	const uint32_t nc = m_base.cables();
	char folder_path[200], file_path[200];
	//path
	path(folder_path);
	sprintf(file_path, "%s/state.txt", folder_path);
	//file
	FILE* file = fopen(file_path, "w");
	//write
	for(uint32_t i = 0; i < nr; i++)
	{
		for(uint32_t j = 0; j <= na; j++)
		{
			for(uint32_t k = 0; k < 8; k++)
			{
				fprintf(file, "%+.6e ", m_data_state[8 * (i * na + j % na) + k]);
			}
			fprintf(file, "\n");
		}
		fprintf(file, "\n");
	}
	//close
	fclose(file);
}
void Map::write_cables(void) const
{
	//data
	const uint32_t na = m_mesh_angle;
	const uint32_t nr = m_mesh_radius;
	const uint32_t nc = m_base.cables();
	char folder_path[200], file_path[200];
	//path
	path(folder_path);
	sprintf(file_path, "%s/cables.txt", folder_path);
	//file
	FILE* file = fopen(file_path, "w");
	//write
	for(uint32_t i = 0; i < nr; i++)
	{
		for(uint32_t j = 0; j <= na; j++)
		{
			for(uint32_t k = 0; k < nc + 3; k++)
			{
				fprintf(file, "%+.6e ", m_data_cables[(nc + 3) * (i * na + j % na) + k]);
			}
			fprintf(file, "\n");
		}
		fprintf(file, "\n");
	}
	//close
	fclose(file);
}
void Map::write_energy(void) const
{
	//data
	const uint32_t na = m_mesh_angle;
	const uint32_t nr = m_mesh_radius;
	const uint32_t nc = m_base.cables();
	char folder_path[200], file_path[200];
	//path
	path(folder_path);
	sprintf(file_path, "%s/energy.txt", folder_path);
	//file
	FILE* file = fopen(file_path, "w");
	//write
	for(uint32_t i = 0; i < nr; i++)
	{
		for(uint32_t j = 0; j <= na; j++)
		{
			for(uint32_t k = 0; k < 3; k++)
			{
				fprintf(file, "%+.6e ", m_data_energy[3 * (i * na + j % na) + k]);
			}
			fprintf(file, "\n");
		}
		fprintf(file, "\n");
	}
	//close
	fclose(file);
}

//path
void Map::path(char* path) const
{
	const uint32_t na = m_mesh_angle;
	const uint32_t nr = m_mesh_radius;
	const uint32_t nc = m_base.cables();
	sprintf(path, "data/maps/%d-%d-%d-%d-%d", m_mode, nc, na, nr, tension() > m_force / 2);
}