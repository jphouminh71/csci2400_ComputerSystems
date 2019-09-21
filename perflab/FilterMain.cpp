#include <stdio.h>
#include "cs1300bmp.h"
#include <iostream>
#include <fstream>
#include "Filter.h"

using namespace std;

#include "rdtsc.h"

//
// Forward declare the functions
//
Filter * readFilter(string filename);
double applyFilter(Filter *filter, cs1300bmp *input, cs1300bmp *output);

int
main(int argc, char **argv)
{

  if ( argc < 2) {
    fprintf(stderr,"Usage: %s filter inputfile1 inputfile2 .... \n", argv[0]);
  }

  //
  // Convert to C++ strings to simplify manipulation
  //
  string filtername = argv[1];

  //
  // remove any ".filter" in the filtername
  //
  string filterOutputName = filtername;
  string::size_type loc = filterOutputName.find(".filter");
  if (loc != string::npos) {
    //
    // Remove the ".filter" name, which should occur on all the provided filters
    //
    filterOutputName = filtername.substr(0, loc);
  }

  Filter *filter = readFilter(filtername);

  double sum = 0.0;
  int samples = 0;
    short int inNum;
  for ( inNum = 2; inNum < argc; inNum++) {
    string inputFilename = argv[inNum];
    string outputFilename = "filtered-" + filterOutputName + "-" + inputFilename;
    struct cs1300bmp *input = new struct cs1300bmp;
    struct cs1300bmp *output = new struct cs1300bmp;
    int ok = cs1300bmp_readfile( (char *) inputFilename.c_str(), input);

    if ( ok ) {
      double sample = applyFilter(filter, input, output);
      sum += sample;
      samples++;
      cs1300bmp_writefile((char *) outputFilename.c_str(), output);
    }
    delete input;
    delete output;
  }
  fprintf(stdout, "Average cycles per sample is %f\n", sum / samples);

}

struct Filter *
readFilter(string filename)
{
  ifstream input(filename.c_str());

  if ( ! input.bad() ) {
    int size = 0;
    input >> size;
    Filter *filter = new Filter(size);
    int div;
    input >> div;
    filter -> setDivisor(div);
    
    short int i, j;
    for ( i=0; i < size; i++) {
      for ( j=0; j < size; j++) {
	int value;
	input >> value;
	filter -> set(i,j,value);
      }
    }
    return filter;
  } else {
    cerr << "Bad input in readFilter:" << filename << endl;
    exit(-1);
  }
}


double
applyFilter(struct Filter *filter, cs1300bmp *input, cs1300bmp *output)
{

  long long cycStart, cycStop;

  cycStart = rdtscll();

    
// pull out all repeated calls  to save calculation times and useless referencing. 
  output -> width = input -> width;
  output -> height = input -> height;

short int divideValue = filter->getDivisor();
    short int width = (input->width)-1;
    short int height = (input->height)-1;
    
    
// create 2d array for the filter values so we don't have to keep calculating in the main loop
short int filterArray[3][3];
    short int i, j;
for(i = 0; i < 3; i++){
    for(j = 0; j < 3; j++){
        filterArray[i][j] = filter->get(i, j);
    }
}
    
    
/* unrolling the main loop even further, we can further improve speeds by processing each plane at the same time, didn't do it here
     since we pull out most of the bad referencing and created the matrix for the filter values we do inline programming which
     is a lot faster
     
     
     using short integers save speed? 
     
     plane -> row -> col 
         the way the hardware places memory into the cache so we need it in this order. 
*/  
    short int plane, row, col;
for( plane = 0; plane < 3; plane++) {
    for( row = 1; row < height; row = row + 1) {
      for( col = 1; col < width; col = col + 1) {

	int temp = 0;	
	temp = temp + (input -> color[plane][row - 1][col - 1] * filterArray[0][0] );
    temp = temp + (input -> color[plane][row - 1][col] * filterArray[0][1] );
    temp = temp + (input -> color[plane][row - 1][col + 1] * filterArray[0][2] );
    temp = temp + (input -> color[plane][row][col - 1] * filterArray[1][0] );
    temp = temp + (input -> color[plane][row][col] * filterArray[1][1] );
    temp = temp + (input -> color[plane][row][col + 1] * filterArray[1][2] );
    temp = temp + (input -> color[plane][row + 1][col - 1] * filterArray[2][0] );
    temp = temp + (input -> color[plane][row + 1][col] * filterArray[2][1] );
    temp = temp + (input -> color[plane][row + 1][col + 1] * filterArray[2][2] );
	
	temp = temp / divideValue;

          
/* theres a way to make this faster, do the conditionals in a form where its all in one line,  : <-- using this
  this would be a faster way to implement the conditions because it would save cycles by not having to process the entire conditionals, if the anticedent fails, then it would skip over the rest of the conditional something like that. 
*/
	if ( temp  < 0 ) {
	  temp = 0;
	}

	if ( temp  > 255 ) { 
	  temp = 255;
	}
    output -> color[plane][row][col] = temp;
      }
    }
  }

  cycStop = rdtscll();
  double diff = cycStop - cycStart;
  double diffPerPixel = diff / (output -> width * output -> height);
  fprintf(stderr, "Took %f cycles to process, or %f cycles per pixel\n",
	  diff, diff / (output -> width * output -> height));
  return diffPerPixel;
}
