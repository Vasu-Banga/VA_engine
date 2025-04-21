#include "ParticleSystem.h"

void ParticleSystem::onStart()
{
	//Create default texture here
	if (particleName == "") {
		particleName = "default_particle";
		ImageDB::CreateDefaultParticleTextureWithName(particleName);
	}
	else {
		ImageDB::LoadImage(particleName);
	}
 	emit_angle_distribution = RandomEngine(emit_angle_min, emit_angle_max, 298);
	emit_radius_distribution = RandomEngine(emit_radius_min, emit_radius_max, 404);
	rotation_distribution = RandomEngine(rotation_min, rotation_max, 440);
	scale_distribution = RandomEngine(start_scale_min, start_scale_max, 494);
	speed_distribution = RandomEngine(start_speed_min, start_speed_max, 498);
	rotation_speed_distribution = RandomEngine(rotation_speed_min, rotation_speed_max, 305);
}

void ParticleSystem::onUpdate()
{
	if (local_frame_number % frames_between_bursts == 0 && emissions_enabled) {
		for (size_t i = 0; i < burst_quantity; i++) {
			createParticle();
		}
	}
	for (size_t i = 0; i < starting_x_positions.size(); i++) {
		if (lifespans[i] < duration_frames) {
			//Update values!
			float percent = (float) lifespans[i] / (float) duration_frames;
			float& velocityX = starting_x_speeds[i];
			float& velocityY = starting_y_speeds[i];
			float& angularVel = starting_rotation_speeds[i];
			float& posX = starting_x_positions[i];
			float& posY = starting_y_positions[i];
			float& rot = starting_rotations[i];
			float scale = starting_scales[i];
			float r = start_color_r;
			float g = start_color_g;
			float b = start_color_b;
			float a = start_color_a;

			velocityX += gravity_scale_x;
			velocityY += gravity_scale_y;

			velocityX *= drag_factor;
			velocityY *= drag_factor;
			angularVel *= angular_drag_factor;

			if (end_scale >= 0.0f) {
				scale = glm::mix(starting_scales[i], end_scale, percent);
			}

			if (end_color_r >= 0.0f) {
				r = glm::mix(r, (float)end_color_r, percent);
			}
			if (end_color_g >= 0.0f) {
				g = glm::mix(r, (float)end_color_g, percent);
			}
			if (end_color_b >= 0.0f) {
				b = glm::mix(r, (float)end_color_b, percent);
			}
			if (end_color_a >= 0.0f) {
				a = glm::mix(r, (float)end_color_a, percent);
			}



			posX += velocityX;
			posY += velocityY;
			rot += angularVel;
			ImageDB::DrawEx(particleName, posX, posY, rot, scale, scale, 0.5f, 0.5f, r, g, b, a, sorting_order);
		}
		else if (lifespans[i] == duration_frames) {
			//Add it to the queue
			openIndicies.push(i);
		}
		lifespans[i]++;
	}
	local_frame_number++;
}

void ParticleSystem::createParticle()
{
	float angle_radians = glm::radians(emit_angle_distribution.Sample());
	float radius = emit_radius_distribution.Sample();
	float scale = scale_distribution.Sample();
	float rotation = rotation_distribution.Sample();
	float speed = speed_distribution.Sample();
	float rotation_speed = rotation_speed_distribution.Sample();

	float cos_angle = glm::cos(angle_radians);
	float sin_angle = glm::sin(angle_radians);

	if (openIndicies.empty()) {
		starting_x_positions.push_back(x + cos_angle * radius);
		starting_y_positions.push_back(y + sin_angle * radius);
		starting_x_speeds.push_back(cos_angle * speed);
		starting_y_speeds.push_back(sin_angle * speed);
		starting_rotation_speeds.push_back(rotation_speed);
		starting_scales.push_back(scale);
		starting_rotations.push_back(rotation);
		lifespans.push_back(0);
	}
	else {
		size_t index = openIndicies.front();
		openIndicies.pop();
		starting_x_positions[index] = x + cos_angle * radius;
		starting_y_positions[index] = y + sin_angle * radius;
		starting_x_speeds[index] = cos_angle * speed;
		starting_y_speeds[index] = sin_angle * speed;
		starting_rotation_speeds[index] = rotation_speed;
		starting_scales[index] = scale;
		starting_rotations[index] = rotation;
		lifespans[index] = 0;
	}
	
}

void ParticleSystem::Play() {
	emissions_enabled = true;
}
void ParticleSystem::Stop() {
	emissions_enabled = false;
}
void ParticleSystem::Burst() {
	for (size_t i = 0; i < burst_quantity; i++) {
		createParticle();
	}
}