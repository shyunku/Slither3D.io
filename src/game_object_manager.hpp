#pragma once

#include "log_manager.hpp"
#include "world_border.hpp"
#include "objvert.hpp"

#include <unordered_map>
#include <utility>

extern CommandConsole console;
extern GameEventLogger gevent;
extern Player* player;

using ovprop = ObjectVertexProperty;
typedef unordered_map<uint, Worm> worm_map;
typedef unordered_map<uint, Prey> prey_map;

typedef class in_game_object_manager_
{
private:
	ObjectVertexProperty large_sphere_vertex_property;
	ObjectVertexProperty small_sphere_vertex_property;
	ObjectVertexProperty circle_vertex_property;
	uint worm_id = 0U;
	uint prey_id = 0U;
public:
	//vector<Star> stars;
	unordered_map<uint, Worm> worms;
	unordered_map<uint, Prey> preys;
	WorldBorder world_border = WorldBorder();

	in_game_object_manager_();
	in_game_object_manager_(ovprop large_sphere_vertex_property, ovprop small_sphere_vertex_propery, ovprop circle_vertex_property);
	void render_all();
	void update_all(float time_tick);
	void detect_collision_worms();
	void magnet_prey();
	void detect_collision_worm_prey();
	int push_new_worm_pair();
	void push_new_prey_rand_pos();
	void push_new_prey_pos(vec3 pos);
	pair<uint, uint> push_new_worm_pairs(uint num);
	void push_player_worm_pair();
	void remove_worm(uint id);
	void remove_worms(uint num);
	void print_alive_worms();
	Worm* get_worm_with_id(uint id);
}InGameObjectManager;