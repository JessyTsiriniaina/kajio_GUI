#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#define SCREEN_WIDTH 290
#define SCREEN_HEIGHT 350
#define BUTTON_WIDTH 60
#define BUTTON_HEIGHT 43
#define CARD_BUTTON 20

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Texture *buttonTextures[CARD_BUTTON];
SDL_Rect buttonRects[CARD_BUTTON];
char inputBuffer[256] = ""; //to stock user input
bool quit = false;

//________________________________________________________________

char *store[255];
int currentIndex = 0;

void undo(void)
{
	if(currentIndex > 1) strcpy(inputBuffer, store[currentIndex-- - 2]);
	else {strcpy(inputBuffer, ""); currentIndex = 0;}
}

void newElem(char *inpt)
{
	strcat(inputBuffer, inpt);
	store[currentIndex] = malloc(sizeof inputBuffer);
	strcpy(store[currentIndex++], inputBuffer);
}

double res = 0.;

unsigned long long fact(unsigned long n)
{
	if (n == 0)
	{
		return 1;
	}
	else
	{
		return n * fact(n - 1);
	}
}

unsigned long long puissance(unsigned long long base, unsigned long long exp)
{
	unsigned long long res = 1;
	for(unsigned long long i = 1; i <= exp; ++i) res = res * base;
	return res;
}

double eval(double a, double b, char c) {
	
	switch (c)
		{
		case '+':
			res = a + b;
			break;
		case '-':
			res = a - b;
			break;
		case 'x':
			res = a * b;
			break;
		case '/':
			if(b != 0) res = (unsigned long long)a / (unsigned long long)b;
			break;
		case '%':
			res = (unsigned long long)a % (unsigned long long)b;
			break;
        case '!':
			res = fact((unsigned long)a);
			break;
        case '^':
			res = puissance(a, b);
			break;
	}
	return res;
}

//_________________________________________________________________


// texture loader
SDL_Texture* loadTexture(const char* path) {

   IMG_Init(IMG_INIT_PNG);
    SDL_Surface* loadedSurface = IMG_Load(path);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
    SDL_FreeSurface(loadedSurface);
    IMG_Quit();
    return texture;
}

// text rendering
void renderText(const char* text, int x, int y) {
    SDL_Color color = {0xde, 0xde, 0xde, 255};
    TTF_Init();
    TTF_Font *font = TTF_OpenFont("ubuntu.ttf", 24);
    SDL_Surface* surface = TTF_RenderText_Solid(font, text, color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_FreeSurface(surface);

    int texW = 0;
    int texH = 31;
    int output_h = 60;
    SDL_QueryTexture(texture, NULL, NULL, &texW, &texH);

    SDL_Rect input = {x + (output_h - texH)/2, y + (output_h - texH)/2, texW, texH};

    SDL_Texture* fond_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
                                SDL_TEXTUREACCESS_TARGET, SCREEN_WIDTH - x*2, output_h);

    SDL_Rect fond = {x , y, SCREEN_WIDTH - x*2, output_h};
    
    SDL_SetRenderTarget(renderer, fond_texture);
    SDL_SetRenderDrawColor(renderer, 0x33, 0x33, 0x33, 255);
    SDL_RenderFillRect(renderer, NULL);
    
    SDL_SetRenderTarget(renderer, NULL);

    SDL_RenderCopy(renderer, fond_texture, NULL, &fond);
    SDL_RenderCopy(renderer, texture, NULL, &input);

    SDL_DestroyTexture(texture);
    SDL_DestroyTexture(fond_texture);
}

// button click event handler
void handleButtonClick(int x, int y) {

    char *buttonTexts[CARD_BUTTON] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
                                    " - ", " + ", " x ", " / ", " % ", "!", "^", " = ", "", ""};

    for (int i = 0; i < CARD_BUTTON ; ++i) {
        if (x >= buttonRects[i].x && x <= buttonRects[i].x + BUTTON_WIDTH &&
            y >= buttonRects[i].y && y <= buttonRects[i].y + BUTTON_HEIGHT) {
            
            if(i == 19) undo();
            else newElem(buttonTexts[i]);
            
            if(i == 17) {
               
                double a, b;
                char op;

                int n = sscanf(inputBuffer, "%lf %c %lf", &a, &op, &b);
                eval(a, b, op);
                
                char *affiche = malloc(sizeof(char)*256);
                
                if (n <= 2 && op != '!') strcpy(affiche, "Syntax error");
                else if (b != 0 || op != '/') sprintf(affiche, "%.0f", res);
                else strcpy(affiche, "Math error");

                strcpy(inputBuffer, "");
                currentIndex = 0;
                newElem(affiche);

                free(affiche);
                affiche = NULL;
            }

            if(i == 18) 
            {
                strcpy(inputBuffer, "");
                currentIndex = 0;
            }

            break;
        }
    }
}

// Function to initialize SDL and create window/renderer
bool initSDL() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    window = SDL_CreateWindow("Kajio", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                             SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    return true;
}

// load button textures && initialize button positions
bool loadMedia() {

    // Load button textures
    buttonTextures[0] = loadTexture("button/0.png");
    buttonTextures[1] = loadTexture("button/1.png");
    buttonTextures[2] = loadTexture("button/2.png");
    buttonTextures[3] = loadTexture("button/3.png");
    buttonTextures[4] = loadTexture("button/4.png");
    buttonTextures[5] = loadTexture("button/5.png");
    buttonTextures[6] = loadTexture("button/6.png");
    buttonTextures[7] = loadTexture("button/7.png");
    buttonTextures[8] = loadTexture("button/8.png");
    buttonTextures[9] = loadTexture("button/9.png");
    buttonTextures[10] = loadTexture("button/minus.png");
    buttonTextures[11] = loadTexture("button/plus.png");
    buttonTextures[12] = loadTexture("button/mult.png");
    buttonTextures[13] = loadTexture("button/div.png");
    buttonTextures[14] = loadTexture("button/mod.png");
    buttonTextures[15] = loadTexture("button/fact.png");
    buttonTextures[16] = loadTexture("button/pow.png");
    buttonTextures[17] = loadTexture("button/equal.png");
    buttonTextures[18] = loadTexture("button/clear.png");
    buttonTextures[19] = loadTexture("button/del.png");

    // Initialize button positions
    int buttonX = 0;
    int buttonY = 70;
    for (int i = 0; i < CARD_BUTTON; ++i) {
        buttonRects[i].x = buttonX + 10;
        buttonRects[i].y = buttonY + 10;
        buttonRects[i].w = BUTTON_WIDTH;
        buttonRects[i].h = BUTTON_HEIGHT;

        buttonX += BUTTON_WIDTH + 10;
        if (buttonX >= SCREEN_WIDTH - 10) {
            buttonX = 0;
            buttonY += BUTTON_HEIGHT + 10;
        }
    }

    return true;
}

// free resources and quit SDL
void closeSDL() {
    for (int i = 0; i < CARD_BUTTON; ++i) {
        SDL_DestroyTexture(buttonTextures[i]);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main(/*int argc, char* args[]*/void) {
    if (!initSDL()) {
        printf("Failed to initialize SDL!\n");
        return -1;
    }

    if (!loadMedia()) {
        printf("Failed to load media!\n");
        return -1;
    }

    // infinite loop to detect event && handle it
    SDL_Event e;
    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
            else if (e.type == SDL_MOUSEBUTTONDOWN) {
                int x, y;
                SDL_GetMouseState(&x, &y);
                handleButtonClick(x, y);
            }
        }

        // Clear screen
        SDL_SetRenderDrawColor(renderer, 0x44, 0x44, 0x44, 255);
        SDL_RenderClear(renderer);

        // Render buttons
        for (int i = 0; i < CARD_BUTTON; ++i) {
            SDL_SetRenderDrawColor(renderer, 0x55, 0x55, 0x55, 255); 
            SDL_SetRenderTarget(renderer, buttonTextures[i]);
            SDL_RenderFillRect(renderer, &buttonRects[i]);
            SDL_SetRenderTarget(renderer, NULL);
            SDL_RenderCopy(renderer, buttonTextures[i], NULL, &buttonRects[i]);
        }

        // Render input text
        renderText(inputBuffer, 10, 10);
        
        // Update screen
        SDL_RenderPresent(renderer);
    }

    closeSDL();
    return 0;
}
