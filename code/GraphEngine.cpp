#include "GraphEngine.h"

extern vector<Nodo> V;

int width ;
int height ;
int h_tab;
int w_tab;

Frame Screen;

void InitGraphEngine(SDL_Renderer* renderer, SDL_Window* window, int w, int h) {

        width = w;
        height = h;
        h_tab = h / 4;

}

void DestroyGraphEngine(SDL_Renderer* renderer, SDL_Window* window) {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void DrawRandomPoints(SDL_Renderer* renderer, const int& n) {
	int x = 0;
	int y = 0;

	for (int i = 0; i < n; i++) {

		SDL_SetRenderDrawColor(renderer, rand() % 255, rand() % 255, rand() % 255, SDL_ALPHA_OPAQUE);
		SDL_RenderDrawPoint(renderer, rand() % 400 + 200, rand() % 400 + 200);
	}
}

void DrawRandomLine(SDL_Renderer* renderer, const int& n)
{
	for (int i = 0; i < n; i++) {
		SDL_SetRenderDrawColor(renderer, rand() % 255, rand() % 255, rand() % 255, SDL_ALPHA_OPAQUE);
		SDL_RenderDrawLine(renderer, rand() % 400 + 200, rand() % 400 + 200, rand() % 400 + 200, rand() % 400 + 200);
	}
}

void DrawRandomCircles(SDL_Renderer* renderer, const int& n) {
    for (int i = 0; i < n; i++) {
        SDL_SetRenderDrawColor(renderer, rand() % 255, rand() % 255, rand() % 255, SDL_ALPHA_OPAQUE);
        DrawCircle(renderer, rand() % 400 + 200, rand() % 400 + 200, 20);
    }
}

void DrawCircle(SDL_Renderer* renderer, int32_t centreX, int32_t centreY, int32_t radius)
{
    const int32_t diameter = (radius * 2);

    int32_t x = (radius - 1);
    int32_t y = 0;
    int32_t tx = 1;
    int32_t ty = 1;
    int32_t error = (tx - diameter);

    while (x >= y)
    {
        //  Each of the following renders an octant of the circle
        SDL_RenderDrawPoint(renderer, centreX + x, centreY - y);
        SDL_RenderDrawPoint(renderer, centreX + x, centreY + y);
        SDL_RenderDrawPoint(renderer, centreX - x, centreY - y);
        SDL_RenderDrawPoint(renderer, centreX - x, centreY + y);
        SDL_RenderDrawPoint(renderer, centreX + y, centreY - x);
        SDL_RenderDrawPoint(renderer, centreX + y, centreY + x);
        SDL_RenderDrawPoint(renderer, centreX - y, centreY - x);
        SDL_RenderDrawPoint(renderer, centreX - y, centreY + x);

        if (error <= 0)
        {
            ++y;
            error += ty;
            ty += 2;
        }

        if (error > 0)
        {
            --x;
            tx += 2;
            error += (tx - diameter);
        }
    }

}

void DrawFilledCircle(SDL_Renderer* renderer, int32_t centreX, int32_t centreY, int32_t radius)
{
    //SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);

    for (int w = 0; w < radius * 2; w++)
    {
        for (int h = 0; h < radius * 2; h++)
        {
            int dx = radius - w; // horizontal offset
            int dy = radius - h; // vertical offset
            if ((dx * dx + dy * dy) <= (radius * radius))
            {
                SDL_RenderDrawPoint(renderer, centreX + dx, centreY + dy);
            }
        }
    }

}

void DrawInnerCircleAnimated(SDL_Renderer* renderer, int32_t centreX, int32_t centreY, int32_t radius) {

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    for (int i = 0; i < 50; i++) {
        DrawCircle(renderer, centreX + 400, centreY + 400, radius * i);
        SDL_RenderPresent(renderer);
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    for (int i = 50; i >= 0; i--) {
        DrawCircle(renderer, centreX + 400, centreY + 400, radius * i);
        SDL_RenderPresent(renderer);
    }

}

void DrawVector(SDL_Renderer* renderer, const vector<Nodo>& V) {
    for (int i = 0; i < V.size(); i++)
        DrawNode(renderer, V[i]);
}

void DrawNode(SDL_Renderer* renderer, const Nodo& v) {
    if(v.indice != 0)SDL_SetRenderDrawColor(renderer,255,255, 255, SDL_ALPHA_OPAQUE);
    else SDL_SetRenderDrawColor(renderer, rand() % 255, rand() % 255, rand() % 255, SDL_ALPHA_OPAQUE);
    DrawFilledCircle(renderer, 100+v.x*7, 100+v.y*7, 8);
}

void DrawRoute(SDL_Renderer* renderer, Rotta& R) {

    int x1,x2;
    int y1,y2;
    int cnt = 0;
    int x0, y0;

    std::list<Nodo>::iterator it_double = R.percorso.begin();
    std::list<Nodo>::iterator it = R.percorso.begin();

    x0 = it->x * 7 + 100;
    y0 = it->y * 7 + 100;

    if (R.percorso.size() != 0)it_double++;

    for (it = R.percorso.begin(); it != R.percorso.end(); ++it) {

        if (it_double != R.percorso.end()) {
            x1 = it->x * 7 + 100;
            y1 = it->y * 7 + 100;
            x2 = it_double->x * 7 + 100;
            y2 = it_double->y * 7 + 100;
            it_double++;
            SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
        }
    }

    if(R.percorso.size() > 1)SDL_RenderDrawLine(renderer, x0, y0, x2, y2);


}

void DrawSolution(SDL_Renderer* renderer, const Soluzione& S) {

    int cnt = 0;
    int r = 1;
    int g = 1;
    int b = 1;

    for (int i = 0; i < S.cluster.size(); i++) {
        if (cnt == 0) {
            SDL_SetRenderDrawColor(renderer, r * 100, g*50, b*10, SDL_ALPHA_OPAQUE);
            cnt++;
            r++;
        }
        else if (cnt == 1) {
            SDL_SetRenderDrawColor(renderer, r * 50, g* 100, b * 10, SDL_ALPHA_OPAQUE);
            cnt++;
            g++;
        }
        else {
            SDL_SetRenderDrawColor(renderer, r * 10, g * 50, b * 100, SDL_ALPHA_OPAQUE);
            cnt = 0;
            b++;
        }
        DrawRoute(renderer, *S.cluster[i]);
    }

    DrawVector(renderer, V);

}

void DrawNodeSized(SDL_Renderer* renderer, const Nodo& v, const Frame& Screen) {

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    //if(v.indice == 0) SDL_SetRenderDrawColor(renderer, rand() % 255 , rand() % 255, rand() % 255, SDL_ALPHA_OPAQUE);

    int x_index = Screen.index_frame % Screen.subscreen; //Indice di traslazione su colonne
    int y_index = Screen.index_frame / Screen.subscreen; //Indice di traslazione su righe
    int x = (100 + v.x * 7) / Screen.subscreen + x_index*width / Screen.subscreen;
    int y = (100 + v.y * 7) / Screen.subscreen + y_index*height / Screen.subscreen;

    DrawFilledCircle(renderer, x,y, 8 / Screen.subscreen);
}

void DrawVectorSized(SDL_Renderer* renderer, const vector<Nodo>& V, const Frame& Screen) {
    for (int i = 0; i < V.size(); i++)
        DrawNodeSized(renderer, V[i], Screen);
}

void DrawRouteSized(SDL_Renderer* renderer, Rotta& R, const Frame& Screen) {

    int x_index = Screen.index_frame % Screen.subscreen; //Indice di traslazione su colonne
    int y_index = Screen.index_frame / Screen.subscreen; //Indice di traslazione su righe

    int x1, x2;
    int y1, y2;
    int x0, y0;

    std::list<Nodo>::iterator it_double = R.percorso.begin();
    std::list<Nodo>::iterator it = R.percorso.begin();

    if (R.percorso.size() > 0) {
        x0 = (100 + it->x * 7) / Screen.subscreen + x_index * width / Screen.subscreen;
        y0 = (100 + it->y * 7) / Screen.subscreen + y_index * height / Screen.subscreen;
    }

    if (R.percorso.size() > 0)it_double++;
    for (it = R.percorso.begin(); it != R.percorso.end(); ++it) {

        if (it_double != R.percorso.end()) {
            x1 = (100 + it->x * 7) / Screen.subscreen + x_index * width / Screen.subscreen;
            y1 = (100 + it->y * 7) / Screen.subscreen + y_index * height / Screen.subscreen;
            x2 = (it_double->x * 7 + 100) / Screen.subscreen + x_index * width / Screen.subscreen;
            y2 = (it_double->y * 7 + 100) / Screen.subscreen + y_index * height / Screen.subscreen;
            it_double++;
            SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
        }
    }

    if (R.percorso.size() > 1)SDL_RenderDrawLine(renderer, x0, y0, x2, y2);

}

void DrawSolutionSized(SDL_Renderer* renderer, const Soluzione& S, const int& lambda, const int & index_frame) {
    int subscreen = sqrt(lambda);

    int cnt = 0;
    int r = 1;
    int g = 1;
    int b = 1;

    Screen.subscreen = subscreen;
    Screen.index_frame = index_frame;

    for (int i = 0; i < S.cluster.size(); i++) {
        if (cnt == 0) {
            SDL_SetRenderDrawColor(renderer, r * 100, g * 50, b * 10, SDL_ALPHA_OPAQUE);
            cnt++;
            r++;
        }
        else if (cnt == 1) {
            SDL_SetRenderDrawColor(renderer, r * 50, g * 100, b * 10, SDL_ALPHA_OPAQUE);
            cnt++;
            g++;
        }
        else {
            SDL_SetRenderDrawColor(renderer, r * 10, g * 50, b * 100, SDL_ALPHA_OPAQUE);
            cnt = 0;
            b++;
        }
        DrawRouteSized(renderer, *S.cluster[i], Screen);
    }

    DrawVectorSized(renderer, V, Screen);

}

void DrawFrameSized(SDL_Renderer* renderer, const int& subscreen) {

    width = 800;
    height = 800;
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    for (int i = 0; i <= subscreen; i++) {
        SDL_RenderDrawLine(renderer, i*width/subscreen, 0, i * width / subscreen, height); //LINEE VERTICALI
        SDL_RenderDrawLine(renderer, 0, i*height/subscreen, width, i * height / subscreen); //LINEE ORIZZONTALI
    }

    SDL_RenderDrawLine(renderer,width-1, 0, width-1, height); //LINEE VERTICALI
    //SDL_RenderDrawLine(renderer, 0, i * height / subscreen, width, i * height / subscreen); //LINEE ORIZZONTALI


}

void ClearRenderSized(SDL_Renderer* renderer, const Frame& Screen) {

    SDL_Rect rect;
    int x_index = Screen.index_frame % Screen.subscreen;
    int y_index = Screen.index_frame / Screen.subscreen;

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);

    rect.x = x_index * width / Screen.subscreen+1;
    rect.y = y_index * height / Screen.subscreen+1;
    rect.w = width/ Screen.subscreen-2;
    rect.h = height / Screen.subscreen-1;

    //cout << rect.w << " " << rect.h << endl;
    SDL_RenderFillRect(renderer, &rect);

    SDL_RenderPresent(renderer);
}