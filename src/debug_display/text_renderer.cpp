#include "text_renderer.h"

text_renderer::text_renderer(std::string font_fullpath, uint8_t font_size, SDL_Renderer* renderer) {
    _renderer = renderer; 
    _font_size = font_size;

    if(!TTF_WasInit() && TTF_Init() != 0) {
        std::cout << "TTF_Init unsuccessful: " << TTF_GetError() << std::endl;
        exit(EXIT_FAILURE);
    }

    _font = TTF_OpenFont(font_fullpath.c_str(), _font_size);

    if (_font == NULL) {
        std::cout << "Error - font not found, error code: " << TTF_GetError() << std::endl;    	 
        exit(EXIT_FAILURE);
    }   

    update_atlas(); // build the glyph atlas for future use

    // establish a texture for use when rendering
    //_text_texture = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, _font_size, _font_size); // init as one character for now, to be grown later
}

text_renderer::~text_renderer() {
    if (TTF_WasInit()) {  // prevent the handler from deleting ths font twice
	    TTF_CloseFont(_font);
        TTF_Quit();
    } 

    //SDL_DestroyTexture(_text_texture);
    SDL_DestroyTexture(_atlas_texture);
    SDL_FreeSurface(_surface);
}

void text_renderer::update_atlas(void) {
    // render each pixel as white with full alpha, so that we can adjust the colour later with a colour mod
    _colour.r = 255;
    _colour.g = 255;
    _colour.b = 255;
    _colour.a = 255;

    // size our atlas rect according to the font size
    _atlas_rect.x = 0; 
	_atlas_rect.y = 0; 		
	_atlas_rect.w = (128 - 32) * _font_size;   // create one large string of letters, 
	_atlas_rect.h = _font_size;  

    char glyphs[128 - 32]; 

    for (char c = 32; c < 128 - 1; c++) {
        glyphs[c - 32] = c;
    }

    _surface = TTF_RenderText_Solid(_font, glyphs, _colour); // convert message into a surface, just once then the surface is used for other reasons
    _atlas_texture = SDL_CreateTextureFromSurface(_renderer, _surface); 
}

void text_renderer::draw_char(char c, uint16_t x_pos, uint16_t y_pos) {
    // we created our glyph table by generating the ascii table from character 32 onwards, simply find it's offset
    uint8_t char_lookup = c - 32;
    
    // find out which character to pull from the atlas
    SDL_Rect char_rect;
    char_rect.x = _font_size * char_lookup + 1;
    char_rect.y = 0;
    char_rect.w = _font_size - 1;
    char_rect.h = _font_size;

    // create the rect to position to the screen
    SDL_Rect positioning_rect; 
    positioning_rect.x = x_pos; 
    positioning_rect.y = y_pos;
    positioning_rect.w = char_rect.w;
    positioning_rect.h = char_rect.h;

    // render it within the surface
    SDL_RenderCopy(_renderer, _atlas_texture, &char_rect, &positioning_rect);
}

void text_renderer::draw_text(std::string text, uint16_t x_pos, uint16_t y_pos) {
    unsigned int length = text.length();

    /*SDL_Rect positioning_rect; 
    positioning_rect.x = x_pos; 
    positioning_rect.y = y_pos;
    positioning_rect.w = _font_size;
    positioning_rect.h = _font_size; // init as one character for now   

    SDL_SetRenderTarget(_renderer, _text_texture); 
    SDL_RenderClear(_renderer);

    for (uint16_t i = 0; i < length; i++) {
        // we created our glyph table by generating the ascii table from character 32 onwards, simply find it's offset
        uint8_t char_lookup = text[i] - 32;
        
        // find out which character to pull from the atlas
        SDL_Rect char_rect;
        char_rect.x = _font_size * char_lookup + 1;
        char_rect.y = 0;
        char_rect.w = _font_size - 1;
        char_rect.h = _font_size;

        // update the rect to position to the screen. This does not support new lines yet, so will not increase the height
        positioning_rect.w += char_rect.w;
        SDL_RenderCopy(_renderer, _atlas_texture, &char_rect, &positioning_rect);
    }
    // render it within the surface
    SDL_SetRenderTarget(_renderer, NULL);
    SDL_RenderCopy(_renderer, _atlas_texture, NULL, &positioning_rect);*/
    for (uint16_t i = 0; i < length; i++) {
        draw_char(text[i], x_pos + (i * _font_size), y_pos);
    }
}

void text_renderer::debug_draw_atlas(uint16_t x_pos, uint16_t y_pos) {
    // TEMP - paint the entire atlast to screen
    _atlas_rect.x = x_pos;
    _atlas_rect.y = y_pos;
    SDL_RenderCopy(_renderer, _atlas_texture, NULL, &_atlas_rect); // copy any new textures to the renderer
}

void text_renderer::set_colour(SDL_Color colour) {
    _colour = colour;

    if (SDL_SetTextureColorMod(_atlas_texture, _colour.r, _colour.g, _colour.b) != 0) {
        std::cout << "Error - could not change colour mod: " << SDL_GetError() << std::endl;    	 
        exit(EXIT_FAILURE);        
    }  
}
