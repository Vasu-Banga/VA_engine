#pragma once
#include <string>
#include <iostream>
#include <unordered_map>
#include "SDL.h"
#include "SDL_ttf.h"
#include <filesystem>
#include <queue>


class TextDB {
private:
    //static TTF_Font* currentFont;
    struct TextType {
        SDL_Texture* texture;
        SDL_FRect rect;
        SDL_FPoint center;
    };
    static std::unordered_map<std::string, std::unordered_map<int, TTF_Font*>> fonts;
    static SDL_Renderer* renderer;
    static std::queue<TextType> displayText;
public:
    static void setRenderer(SDL_Renderer* render) { renderer = render; }
    static void LoadFont(const std::string& font_name, const int& font_size);
    static void DrawText(std::string str_content, float x, float y, std::string font_name, int font_size, int r, int g, int b, int a);
    static void showText();
};

