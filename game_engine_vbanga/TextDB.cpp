#include "TextDB.h"
#include "Helper.h"

//TTF_Font* TextDB::currentFont;
std::unordered_map<std::string, std::unordered_map<int, TTF_Font*>> TextDB::fonts;
SDL_Renderer* TextDB::renderer = nullptr;
std::queue<TextDB::TextType> TextDB::displayText;

void TextDB::LoadFont(const std::string& font_name, const int& font_size) {
    std::string font_path = "resources/fonts/" + font_name + ".ttf";
    if (!std::filesystem::exists(font_path)) {
        std::cout << "error: font " << font_name << " missing";
        exit(0);
    }
    
    fonts[font_name][font_size] = TTF_OpenFont(font_path.c_str(), font_size);
}

void TextDB::DrawText(std::string str_content, float x, float y, std::string font_name, int font_size, int r, int g, int b, int a) {
    if (str_content == "") {
        return;
    }


    if (fonts.count(font_name) != 0 && fonts[font_name].count(font_size) != 0) {
        //Already loaded, we chillin
    }
    else {
        LoadFont(font_name, font_size);
    }

    TTF_Font* currentFont = fonts[font_name][font_size];
    SDL_Color color;
    color.r = r;
    color.g = g;
    color.b = b;
    color.a = a;

    SDL_Surface* text_surface = TTF_RenderText_Solid(currentFont, str_content.c_str(),color);
    SDL_Texture* text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
    float tempWidth = static_cast<float>(text_surface->w);
    float tempHeight = static_cast<float>(text_surface->h);
    SDL_FRect text_rect = { x, y, tempWidth, tempHeight };
    SDL_FPoint center;
    center.x = text_rect.w / 2.0f;
    center.y = text_rect.h / 2.0f;
    displayText.push({ text_texture,text_rect,center });
    SDL_FreeSurface(text_surface);
}

void TextDB::showText() {
    while (!displayText.empty()) {
        TextType& value = displayText.front();
        Helper::SDL_RenderCopyEx(-1, "text", renderer, value.texture, nullptr, &value.rect, 0.0, &value.center, SDL_FLIP_NONE);
        SDL_DestroyTexture(value.texture);
        displayText.pop();
   }
}
