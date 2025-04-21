#pragma once
#include <string>
#include <filesystem>
#include <iostream>
#include <unordered_map>
#include "SDL.h"
#include "SDL_image.h"
#include <queue>
#include "glm/glm.hpp"

class ImageDB
{
private:
	 struct UIType {
		std::string image_name;
		float x;
		float y;
		float r = 255.0f;
		float g = 255.0f;
		float b = 255.0f;
		float a = 255.0f;
		float sorting_order = 0.0f;
	};

	 struct ImageType {
		std::string image_name;
		float x;
		float y;
		float rotation_degrees = 0.0f;
		float scale_x = 1.0f;
		float scale_y = 1.0f;
		float pivot_x = 0.5f;
		float pivot_y = 0.5f;
		float r = 255.0f;
		float g = 255.0f;
		float b = 255.0f;
		float a = 255.0f;
		float sorting_order = 0.0f;
	};

	 struct ImagePixels {
		float x;
		float y;
		float r;
		float g;
		float b;
		float a;
	};

	static std::unordered_map <std::string, SDL_Texture*> images;
	static SDL_Renderer* renderer;
	static std::vector<UIType> ui_images;
	static std::vector<ImageType> scene_images;
	static std::vector<ImagePixels> pixel_images;
	static int frameWidth;
	static int frameHeight;
	static glm::vec2 camera;
	static float zoomFactor;

public:
	static void setRenderer(SDL_Renderer* render) { renderer = render; }
	static void setHeight(int val) { frameHeight = val; }
	static void setWidth(int val) { frameWidth = val; }
	static void setCamera(float x, float y) { camera = { x, y }; }
	static float getPositionX() { return camera.x; }
	static float getPositionY() { return camera.y; }
	static float getZoom() { return zoomFactor; }
	static void setZoom(float val) { zoomFactor = val; }
	static void LoadImage(const std::string&);
	static SDL_Texture* GetImage(const std::string&);
	static void DrawUI(const std::string& image_name, float x, float y);
	static void DrawUIEx(const std::string& image_name, float x, float y, float r, float g, float b, float a, float sorting_order);
	static void Draw(const std::string& image_name, float x, float y);
	static void DrawEx(const std::string& image_name, float x, float y, float rotation_degrees, float scale_x, float scale_y, float pivot_x, float pivot_y, float r, float g, float b, float a, float sorting_order);
	static void DrawPixel(float x, float y, float r, float g, float b, float a);
	static void RenderAll();
	static void CreateDefaultParticleTextureWithName(const std::string& name);

};

