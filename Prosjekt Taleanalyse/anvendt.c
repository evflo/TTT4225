

#include <libsndfile.h>
#define BUFSIZE 1024

void readFile(char* file){

	int data[BUFSIZE];
	SNDFILE *infile;
	
	SFINFO sfinfo;

	memset(&sfinfo,0,sizeod(sfinfo));

	if(!(infile = sf_open(file,SFM_READ,&sfinfo))){

	}
	
	calloc(sfinfo.frames*sfinfo.channels,sizeof(double));
	count = sf_read_double(infile, buf,sfinfo.frames*sfinfo.channels);
	
}

void main(int argc, char** argv){

	readFile(argv[1]);



}
