#define SDL_MAIN_HANDLED
//#include <SDL.h>
#include <cstdlib>
#include <iostream>
#include <ctime>
#include <cmath>
#include "HGL.h"

struct Frame {
	int subscreen;
	int index_frame;
};

using namespace std;

void InitGraphEngine(SDL_Renderer* renderer, SDL_Window* window, int w, int h);
void DestroyGraphEngine(SDL_Renderer* renderer, SDL_Window* window);

void DrawRandomPoints(SDL_Renderer*,const int &);
void DrawRandomLine(SDL_Renderer*, const int&);
void DrawRandomCircles(SDL_Renderer* , const int&);

void DrawInnerCircleAnimated(SDL_Renderer* renderer, int32_t centreX, int32_t centreY, int32_t radius);
void DrawCircle(SDL_Renderer* renderer, int32_t centreX, int32_t centreY, int32_t radius);
void DrawFilledCircle(SDL_Renderer* renderer, int32_t centreX, int32_t centreY, int32_t radius);


void DrawSolution(SDL_Renderer* renderer, const Soluzione & S);
void DrawRoute(SDL_Renderer* renderer, Rotta& R);
void DrawVector(SDL_Renderer* renderer, const vector<Nodo> & V);
void DrawNode(SDL_Renderer* renderer, const Nodo& v);

void DrawSolutionSized(SDL_Renderer* renderer, const Soluzione& S, const int& lamba, const int& index_frame);
void DrawRouteSized(SDL_Renderer* renderer, Rotta& R, const Frame& Screen);
void DrawVectorSized(SDL_Renderer* renderer, const vector<Nodo>& V, const Frame& Screen);
void DrawNodeSized(SDL_Renderer* renderer, const Nodo& v, const Frame & Screen);
void DrawFrameSized(SDL_Renderer*, const int& subscreen);

void ClearRenderSized(SDL_Renderer*, const Frame& Screen);

