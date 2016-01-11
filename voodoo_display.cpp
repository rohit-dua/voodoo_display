/*
   Vodoo Display - Tempest for Data transfer
   Copyright (C) 2015  Rohit Dua

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

*/
#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <SDL2/SDL.h>

double CARRIER = 27000000;
int HTOTAL;
double PIXEL_CLOCK;

using namespace std;


namespace patch
{
  template < typename T > std::string to_string( const T& n  )
  {
    std::ostringstream stm ;
    stm << n ;
    return stm.str() ;
  }
}


namespace SDL{
  int resx, resy;
  //The window we'll be rendering to 
  SDL_Window* gWindow = NULL; 
  SDL_Renderer* renderer;
  SDL_Texture* texture;
  Uint32* pixels = NULL;
  bool bgcolor = 0, SDL_quit=0;
  Uint32 white=0, black=0, grey=0;
  SDL_Thread *mouse_thread;
  bool init0 () {
    //Initialization flag
    bool success = true;
    // Declare display mode structure to be filled in.
    SDL_DisplayMode current;
    //Initialize SDL 
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) 
    { 
      printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
      success = false; 
    } 
    else 
    { 
      int should_be_zero = SDL_GetCurrentDisplayMode(0, &current);
      if(should_be_zero == 0){
        resx = current.w;
        resy = current.h;
      }    
      //Create window 
      gWindow = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, resx, resy, SDL_WINDOW_FULLSCREEN_DESKTOP );
      //gWindow = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 10, 10, SDL_WINDOW_BORDERLESS);

      if( gWindow == NULL ) 
      { 
        printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
        success = false; 
      }
      else 
      { 
        renderer = SDL_CreateRenderer(gWindow, -1, 0);
        texture = SDL_CreateTexture(renderer,SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, resx, resy);
        white = SDL_MapRGB(SDL_GetWindowSurface(gWindow)->format,0xff,0xff,0xff);
        black = SDL_MapRGB(SDL_GetWindowSurface(gWindow)->format,0x00,0x00,0x00);
        grey = SDL_MapRGB(SDL_GetWindowSurface(gWindow)->format,0x80,0x80,0x80);
        pixels = new Uint32[resx * resy];
      } 
    }
    return success; 
  }

  void toggle_bgcolor() {
    if(bgcolor==1){
      SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
      bgcolor = 0;
    }
    else{
      SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
      bgcolor = 1;
    }
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
  }

  void load(){
    SDL_UpdateTexture(texture, NULL, pixels, resx * sizeof(Uint32));
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
  }

  int pixel_carrier_fill(double freq, Uint32 color=white)
  {
    memset(pixels, black, resx * resy * sizeof(Uint32));
    int x, y;
    double t;
    double ftfp2 = freq / PIXEL_CLOCK * 2.0;
    double fcfp = CARRIER / PIXEL_CLOCK * 2.0;
    t = 0;
    for (y = 0; y < resy; y++) {
      if (int(t*ftfp2)%2) {
        t+=resx;
      }else{
        for (x = 0; x < resx; x++) {
          if (int(t*fcfp)%2)
            //printf(" %d ",int(t*fcfp));
            pixels[y*resx+x] = color;
          t++;
        }
      }
      t += HTOTAL - resx;
    }
    return 0;
  }

  void close0() { 
    //Destroy window 
    SDL_DestroyWindow( gWindow ); 
    gWindow = NULL; 
    delete[] pixels;
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
  }
}


int mouse_event(void* ptr)
{
  using namespace SDL;
  SDL_Event event;
  while(true){
    while(SDL_PollEvent(&event)){
      if (event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEMOTION){
        SDL_quit = 1;
        return 0;
      }
    } 
  }
}


double note_frequency(const char note[2]){
  double freq = 0.0;
  switch(tolower(note[0])){
    case 'c': if(note[1]=='#')
                freq =  554.37;
              else
                freq = 523.25;
              break;
    case 'd': if(note[1]=='#')
                freq = 622.25;
              else
                freq = 587.33;
              break;
    case 'e': freq = 659.26;
              break;
    case 'f': if(note[1]=='#')
                freq = 739.99;
              else
                freq = 698.46;
              break;
    case 'g': if(note[1]=='#')
                freq = 830.61;
              else
                freq = 783.99;
              break;
    case 'a': if(note[1]=='#')
                freq = 932.33;
              else
                freq = 880;
              break;
    case 'b': freq = 987.77;
              break;
    default: freq = 0.0;
  }
  return freq;
}


string create_data_packet(string data){
  unsigned long checksum = 0;
  string morseCode[] = {"01", "1000", "1010", "100", "0", "0010", 
    "110", "0000", "00", "0111", "101", "0100", "11", "10", "111", 
    "0110", "1101", "010", "000", "1", "001", "0001", "011", "1001", 
    "1011", "1100"};
  char ch;
  string morseWord = "";
  for(unsigned int i=0; i < data.length(); i++)
  {
    ch = data[i];
    if(ch == ' '){
      morseWord += "0101";
    }
    else{
      ch = toupper(ch);
      morseWord += morseCode[ch - 'A'];
    }
    morseWord += " ";
  }
  unsigned int len = morseWord.length();
  string packet;
  packet = morseWord; //+ patch::to_string(len);// + std::stoi(checksum); 
  return packet;
}


int generate_display(string data){
  // 1 = b = 987.77
  // 0 = c = 523.25
  // delim = f# = 739.99
  using namespace SDL;
  int data_index = 0;
  double freq;
  string text_data = create_data_packet(data);
  //printf("Text: %s", text_data.c_str());
  int text_data_size = sizeof(text_data)/sizeof(text_data[0]);
  while(SDL_quit == 0 && data_index < text_data_size){
    if (text_data[data_index] == '1')
      freq = 987.77;
    else if (text_data[data_index] == '0')
      freq = 739.99;
    else if (text_data[data_index] == ' ')
      freq = 523.25;
    else{
      printf("Invalid Text");
      break;
    }
    pixel_carrier_fill(freq);
    load();
    data_index++;
    SDL_Delay(100);
    pixel_carrier_fill(880);
    load();
    SDL_Delay(100);
  }
  return 0;
}


void play_song(char song_file[]){
  std::ifstream file(song_file);
  std::string line;
  double freq;
  while (std::getline(file, line) && SDL::SDL_quit==0){
    if(line[0] == '/' || line[0] == '\n'){
      continue;
    }
    else{
      istringstream iss(line);
      std::string out;
      while(getline(iss, out, ' ') && SDL::SDL_quit==0){
        freq = note_frequency(out.c_str());
        SDL::pixel_carrier_fill(freq);
        SDL::load();
        SDL_Delay(200);
      }
      SDL_Delay(200);
    }
  }
}


int main (int argc, char **argv)
{
  using namespace SDL;
  bool song_play = false;
  char *song_file;
  if(argc>1){
    song_file = argv[1];
    song_play = true;
  }
  bool data = 0;
  FILE *fp;
  char var[70];
  fp = popen("xvidtune -show", "r");
  fgets(var, sizeof(var), fp);
  pclose(fp);
  char * out = strtok(var, " ");
  int index = 0;
  while (out != NULL && index < 6) {
    switch(index){
      case 1: PIXEL_CLOCK = atof(out)*1000000;
              break;
      case 5: HTOTAL = atoi(out);
              break;
    }
    index++;
    out = strtok(NULL, " ");
  }
  if( !init0() )
  {
    printf( "Failed to initialize!\n" );
  }
  else
  {
    printf("PIXEL CLOCK: %fHz\nHTOTAL: %d\nAM Carrier Frequency: %fHz\nResolution: %dx%d\n", PIXEL_CLOCK, HTOTAL, CARRIER,SDL::resx, SDL::resy);
    mouse_thread = SDL_CreateThread(mouse_event, "Mouse Click Detect Thread", (void*)NULL);
    if(song_play)
      while(SDL_quit==0)
        play_song(song_file);
    else{ 
      while(SDL_quit==0){
        generate_display("ROHIT DUA");
        //SDL_quit  = 1;
      }
    }
  }
  //Free resources and close SDL
  close0(); 
  return 0;
}
