#pragma once
#include "Helper.h"
#include "ImageDB.h"
#include <vector>
#include <queue>
#include "glm/glm.hpp"



class ParticleSystem
{
private:
	//Limits
	float emit_angle_min = 0.0f;
	float emit_angle_max = 360.0f;
	float emit_radius_min = 0.0f;
	float emit_radius_max = 0.5f;
	float start_scale_min = 1.0f;
	float start_scale_max = 1.0f;
	float rotation_min = 0.0f;
	float rotation_max = 0.0f;

	//Distributions
	RandomEngine emit_angle_distribution;
	RandomEngine emit_radius_distribution;
	RandomEngine rotation_distribution;
	RandomEngine scale_distribution;
	RandomEngine speed_distribution;
	RandomEngine rotation_speed_distribution;

	//Initial Values
	float x = 0.0f;
	float y = 0.0f;
	int frames_between_bursts = 1;
	int burst_quantity = 1;
	
	std::string particleName = "";
	int local_frame_number = 0;

	// Color
	int start_color_r = 255;
	int start_color_g = 255;
	int start_color_b = 255;
	int start_color_a = 255;

	// Sorting Order
	int sorting_order = 9999;


	//Animation Values
	bool enabled = true;
	bool emissions_enabled = true;
	int duration_frames = 300;
	float start_speed_min = 0.0f;
	float start_speed_max = 0.0f;
	float rotation_speed_min = 0.0f;
	float rotation_speed_max = 0.0f;
	float gravity_scale_x = 0.0f;
	float gravity_scale_y = 0.0f;
	float drag_factor = 1.0f;
	float angular_drag_factor = 1.0f;
	float end_scale = -1.0f;
	int end_color_r = -1, end_color_g = -1, end_color_b = -1, end_color_a = -1;

	//Objects of Arrays
	std::vector<float> starting_x_positions;
	std::vector<float> starting_y_positions;
	std::vector<float> starting_rotations;
	std::vector<float> starting_scales;
	std::vector<float> starting_x_speeds;
	std::vector<float> starting_y_speeds;
	std::vector<float> starting_rotation_speeds;
	std::vector<size_t> lifespans;
	std::queue<size_t> openIndicies;


public:
	//Getters and Setters
	void setX(float val) { x = val; }
	float getX() const { return x; }
	void setY(float val) { y = val; }
	float getY() const { return y; }
	void setFramesBetweenBursts(int val) { if (val <= 1) { frames_between_bursts = 1; return; } frames_between_bursts = val; }
	int getFramesBetweenBursts() const { return frames_between_bursts; }
	void setBurstQuantity(int val) { if (val <= 1) { burst_quantity = 1; return; } burst_quantity = val; }
	int getBurstQuantity() const { return burst_quantity; }
	void setStartScaleMin(float val) { start_scale_min = val; }
	float getStartScaleMin() const { return start_scale_min; }
	void setStartScaleMax(float val) { start_scale_max = val; }
	float getStartScaleMax() const { return start_scale_max; }
	void setRotationMin(float val) { rotation_min = val; }
	float getRotationMin() const { return rotation_min; }
	void setRotationMax(float val) { rotation_max = val; }
	float getRotationMax() const { return rotation_max; }
	void setStartColorR(int val) { start_color_r = val; }
	int getStartColorR() const { return start_color_r; }
	void setStartColorG(int val) { start_color_g = val; }
	int getStartColorG() const { return start_color_g; }
	void setStartColorB(int val) { start_color_b = val; }
	int getStartColorB() const { return start_color_b; }
	void setStartColorA(int val) { start_color_a = val; }
	int getStartColorA() const { return start_color_a; }
	void setSortingOrder(int val) { sorting_order = val; }
	int getSortingOrder() const { return sorting_order; }
	void setEmitRadiusMin(float val) { emit_radius_min = val; }
	float getEmitRadiusMin() const { return emit_radius_min; }
	void setEmitRadiusMax(float val) { emit_radius_max = val; }
	float getEmitRadiusMax() const { return emit_radius_max; }
	void setEmitAngleMin(float val) { emit_angle_min = val; }
	float getEmitAngleMin() const { return emit_angle_min; }
	void setEmitAngleMax(float val) { emit_angle_max = val; }
	float getEmitAngleMax() const { return emit_angle_max; }
	void setParticleName(const std::string& val) { particleName = val; }
	std::string getParticleName() const { return particleName; }
	void setDurationFrames(int val) { duration_frames = glm::max(1, val); }
	int getDurationFrames() const { return duration_frames; }
	void setEnabled(bool val) { enabled = val; }
	bool getEnabled() const { return enabled; }

	void setStartSpeedMin(float val) { start_speed_min = val; }
	float getStartSpeedMin() const { return start_speed_min; }
	void setStartSpeedMax(float val) { start_speed_max = val; }
	float getStartSpeedMax() const { return start_speed_max; }

	void setRotationSpeedMin(float val) { rotation_speed_min = val; }
	float getRotationSpeedMin() const { return rotation_speed_min; }
	void setRotationSpeedMax(float val) { rotation_speed_max = val; }
	float getRotationSpeedMax() const { return rotation_speed_max; }

	void setGravityScaleX(float val) { gravity_scale_x = val; }
	float getGravityScaleX() const { return gravity_scale_x; }
	void setGravityScaleY(float val) { gravity_scale_y = val; }
	float getGravityScaleY() const { return gravity_scale_y; }

	void setDragFactor(float val) { drag_factor = val; }
	float getDragFactor() const { return drag_factor; }

	void setAngularDragFactor(float val) { angular_drag_factor = val; }
	float getAngularDragFactor() const { return angular_drag_factor; }

	void setEndScale(float val) { end_scale = val; }
	float getEndScale() const { return end_scale; }

	void setEndColorR(int val) { end_color_r = val; }
	int getEndColorR() const { return end_color_r; }
	void setEndColorG(int val) { end_color_g = val; }
	int getEndColorG() const { return end_color_g; }
	void setEndColorB(int val) { end_color_b = val; }
	int getEndColorB() const { return end_color_b; }
	void setEndColorA(int val) { end_color_a = val; }
	int getEndColorA() const { return end_color_a; }

	//Functions
	void onStart();
	void onUpdate();
	void createParticle();


	void Play();
	void Stop();
	void Burst();
};

