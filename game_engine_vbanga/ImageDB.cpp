#include "ImageDB.h"
#include "Helper.h"
#include "TextDB.h"


std::unordered_map <std::string, SDL_Texture*> ImageDB::images;
SDL_Renderer* ImageDB::renderer;

std::vector<ImageDB::UIType> ImageDB::ui_images;
std::vector<ImageDB::ImageType> ImageDB::scene_images;
std::vector<ImageDB::ImagePixels> ImageDB::pixel_images;

int ImageDB::frameWidth;
int ImageDB::frameHeight;

glm::vec2 ImageDB::camera;
float ImageDB::zoomFactor = 1.0f;


void ImageDB::LoadImage(const std::string& imgName) {
    if (images.count(imgName) != 0) {
        //Now I can call it without checking each actor - done internally
        return;
    }

    if (!std::filesystem::exists("resources/images/" + imgName + ".png")) {
        std::cout << "error: missing image " << imgName;
        exit(0);
    }
    //Make sure that there isn't already a value for that imgName
    images.insert({imgName, IMG_LoadTexture(renderer, ("resources/images/" + imgName + ".png").c_str()) });
}

SDL_Texture* ImageDB::GetImage(const std::string& imgName) {
    if (images.count(imgName) == 0) {
        return nullptr;
    }
    return images[imgName];
}

void ImageDB::DrawUI(const std::string& image_name, float x, float y) {
    LoadImage(image_name);
    UIType draw;
    draw.image_name = image_name;
    draw.x = x;
    draw.y = y;
    ui_images.push_back(draw);
}

void ImageDB::DrawUIEx(const std::string& image_name, float x, float y, float r, float g, float b, float a, float sorting_order) {
    LoadImage(image_name);
    UIType draw;
    draw.image_name = image_name;
    draw.x = x;
    draw.y = y;
    draw.r = r;
    draw.g = g;
    draw.b = b;
    draw.a = a;
    draw.sorting_order = sorting_order;
    ui_images.push_back(draw);
}

void ImageDB::Draw(const std::string& image_name, float x, float y) {
    LoadImage(image_name);
    ImageType draw;
    draw.image_name = image_name;
    draw.x = x;
    draw.y = y;
    draw.scale_x = 1.0f;
    draw.scale_y = 1.0f;
    draw.rotation_degrees = 0.0f;
    draw.pivot_x = 0.5f;
    draw.pivot_y = 0.5f;
    scene_images.push_back(draw);
}

void ImageDB::DrawEx(const std::string& image_name, float x, float y, float rotation_degrees, float scale_x, float scale_y, float pivot_x, float pivot_y, float r, float g, float b, float a, float sorting_order) {
    LoadImage(image_name);
    ImageType draw;
    draw.image_name = image_name;
    draw.x = x;
    draw.y = y;
    draw.rotation_degrees = rotation_degrees;
    draw.scale_x = scale_x;
    draw.scale_y = scale_y;
    draw.pivot_x = pivot_x;
    draw.pivot_y = pivot_y;
    draw.r = r;
    draw.g = g;
    draw.b = b;
    draw.a = a;
    draw.sorting_order = sorting_order;
    scene_images.push_back(draw);
}

void ImageDB::DrawPixel(float x, float y, float r, float g, float b, float a) {
    ImagePixels pixel;
    pixel.x = x;
    pixel.y = y;
    pixel.r = r;
    pixel.g = g;
    pixel.b = b;
    pixel.a = a;
    pixel_images.push_back(pixel);
}

void ImageDB::RenderAll() {
    std::stable_sort(scene_images.begin(), scene_images.end(), [](const ImageType& a, const ImageType& b) {
        return a.sorting_order < b.sorting_order;
        });
    float UNIT_TO_PIXEL = 100;
    SDL_RenderSetScale(renderer, zoomFactor, zoomFactor);
    for (ImageType& img : scene_images) {
        SDL_Texture* texture = GetImage(img.image_name);
        if (!texture) continue;

        glm::vec2 targetPos = glm::vec2(img.x, img.y) - camera;

        int flip_mode = SDL_FLIP_NONE;
        if (img.scale_x < 0) {
            flip_mode |= SDL_FLIP_HORIZONTAL;
        }
        if (img.scale_y < 0) {
            flip_mode |= SDL_FLIP_VERTICAL;
        }

        float tex_w, tex_h;
        Helper::SDL_QueryTexture(texture, &tex_w, &tex_h);

        SDL_FRect dst;
        dst.w = tex_w * glm::abs(img.scale_x);
        dst.h = tex_h * glm::abs(img.scale_y);


        dst.x = (frameWidth * 0.5f * (1.0f / zoomFactor)) + ((targetPos.x * UNIT_TO_PIXEL - (img.pivot_x * dst.w)));
        dst.y = (frameHeight * 0.5f * (1.0f / zoomFactor)) + ((targetPos.y * UNIT_TO_PIXEL - (img.pivot_y * dst.h)));

        SDL_FPoint pivot = { img.pivot_x * dst.w, img.pivot_y * dst.h };
        SDL_SetTextureColorMod(texture, img.r, img.g, img.b);
        SDL_SetTextureAlphaMod(texture, img.a);

        Helper::SDL_RenderCopyEx(-1, "scene", renderer, texture, nullptr, &dst, img.rotation_degrees, &pivot, SDL_FLIP_NONE);
        SDL_RenderSetScale(renderer, zoomFactor, zoomFactor);

        SDL_SetTextureColorMod(texture, 255, 255, 255);
        SDL_SetTextureAlphaMod(texture, 255);
    }
    SDL_RenderSetScale(renderer, 1, 1);

    std::stable_sort(ui_images.begin(), ui_images.end(), [](const UIType& a, const UIType& b) {
        return a.sorting_order < b.sorting_order;
        });
    for (UIType& ui : ui_images) {
        SDL_Texture* texture = GetImage(ui.image_name);
        if (!texture) continue;

        float tex_w, tex_h;
        Helper::SDL_QueryTexture(texture, &tex_w, &tex_h);

        SDL_FRect dst;
        dst.x = ui.x;
        dst.y = ui.y;
        dst.w = tex_w;
        dst.h = tex_h;

        SDL_SetTextureColorMod(texture, ui.r, ui.g, ui.b);
        SDL_SetTextureAlphaMod(texture, ui.a);

        Helper::SDL_RenderCopy(renderer, texture, nullptr, &dst);

        SDL_SetTextureColorMod(texture, 255, 255, 255);
        SDL_SetTextureAlphaMod(texture, 255);
    }

    TextDB::showText();

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    for (ImagePixels& px : pixel_images) {
        SDL_SetRenderDrawColor(renderer,
            px.r,
            px.g,
            px.b,
            px.a);
        SDL_RenderDrawPoint(renderer, px.x, px.y);
    }
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

    scene_images.clear();
    ui_images.clear();
    pixel_images.clear();
}

void ImageDB::CreateDefaultParticleTextureWithName(const std::string& name)
{
    if (images.find(name) != images.end()) {
        return;
    }

    SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormat(0, 8, 8, 32, SDL_PIXELFORMAT_RGBA8888);

    Uint32 white_color = SDL_MapRGBA(surface->format, 255, 255, 255, 255);
    SDL_FillRect(surface, NULL, white_color);

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    images[name] = texture;
}
