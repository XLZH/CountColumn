/* This eidtion is used to calculate the frequence of random seq in the fastq file
 * (1) First, You need to provide the fastq file to be calculated!
 * (2) Second, You should specify the column (which should short than seq length) to be calculate!
 *
 * NOTE: Compile the C source code with : gcc CountCol.2.c -std=c99 -o CountCol.2
 * Version : 2
 * Modify : 2016-10-16
 */

#define __STDC_WANT_LIB_EXT1__ 1
#include <stdio.h>
#include <linux/limits.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <stdbool.h>

#define A 65
#define T 84
#define C 67
#define G 71

#define MAX_READLEN 250
#define MAX_BUFROW 100000

typedef struct {
    int a[MAX_READLEN];
    int t[MAX_READLEN];
    int c[MAX_READLEN];
    int g[MAX_READLEN];
    int n[MAX_READLEN];
    long int TotalRowCount;
} RESULT;

typedef struct {
	bool help;
	int col;
	char infile[PATH_MAX];
} ARGS;

char TemBuff[MAX_BUFROW][MAX_READLEN] = {'\0'};
/* Used to content the reads form file */

void Usage (void);
/* Show usage information */

void ParseArgs (ARGS*, int, char*[]);
/* Parse the args for the program */

void Calculate (RESULT*, char (*)[], int, const int);
/* Calculate the content of ATGC in column */

void PrintInfo (RESULT*, const int);
/* Print the result to stander stdout */


int main (int argc, char *argv[])
{
	ARGS* Args = (ARGS*)malloc(sizeof(ARGS));

	ParseArgs (Args, argc, argv);
	if (Args->help || argc < 5) { Usage (); exit (1); }

	RESULT Result;
	memset(&Result,0,sizeof(RESULT));
	time_t start, end;
	FILE* Infile = NULL;
	bool fgetstatus = true;	

	if (!(Infile = fopen(Args->infile, "r"))) {
		fprintf(stderr, "error: Failed to open inputfile %s\n", Args->infile);
		exit (2);
	}
	setvbuf(Infile, NULL, _IOFBF, BUFSIZ);

	time (&start); // Start the timer
	printf ("Start to anylizing ...\n");

	while (fgetstatus)
	{
		int rowcount = 0;
		char Trash[MAX_READLEN] = {'\0'};

		for (int i=0; i < MAX_BUFROW; ++i)
		{
			if ( (i + 3) % 4 == 0 ) {
				if (fgets(TemBuff[rowcount], MAX_READLEN, Infile))
					rowcount ++;
				else { fgetstatus = false; break; }
			}
			else {
				if (fgets(Trash, MAX_READLEN, Infile))
					continue;
				else { fgetstatus = false; break; }
			}
		}

		Calculate (&Result, TemBuff, rowcount, Args->col);
		Result.TotalRowCount += rowcount;
	}

	PrintInfo (&Result, Args->col); // putout the result to stander stdout!
	time (&end); // End the timer
	printf("The time consum is %.2lf(s)\n", difftime(end,start));

	free(Args);
	Infile = NULL;

	return 0;
}


void Usage (void)
{
	char* Usage = 
	"This Program will calculate the percent of ATGC in eatch column!\n" \
	"Usage: ./CountCol -i <Infile(fastq file)> -c <colNumber> \n" \
	"		[-h]		This will print helpInfo. \n" \
	"		[-i]		[Required] Inputfile (Fastq file) \n" \
	"		[-c]		[Required] The column num(numeric) to be calculated!\n";

	fprintf(stderr,"%s",Usage);
}


void ParseArgs (ARGS* Args, int argc, char *argv[])
{
	int ch;
	opterr = 0;

	while ((ch = getopt(argc, argv, "hi:c:")) != -1) 
	{
		switch (ch) {
			case 'h':
				Args->help = true; break;
			case 'i':
				strcpy(Args->infile, optarg); break;
			case 'c':
				if ((Args->col = atoi(optarg)) >= MAX_READLEN) {
					Args->help = true;
					fprintf(stderr, "error: Column exceed the MAX_READLEN\n");
				}
				break;
			case '?':
				Args->help = true;
				fprintf(stderr, "error: Option error occour!.\n");
		}
	}	
}


void Calculate (RESULT* Result, char (*TemBuff)[MAX_READLEN],
			 int rowcount, const int colcount)
{

	for (int colindex=0; colindex < colcount; ++colindex)
	{
		char TemCol[MAX_BUFROW + 1] = {'\0'};
		char* pTemCol = TemCol;

		for (int i=0; i < rowcount; ++i)
			*(pTemCol + i) = *( *(TemBuff + i) + colindex );

		for (pTemCol; *pTemCol != '\0'; ++pTemCol)
		{
            switch (*pTemCol) {
                case A:
                    ++Result->a[colindex]; break;
                case T:
                    ++Result->t[colindex]; break;
                case C:
                    ++Result->c[colindex]; break;
                case G:
                    ++Result->g[colindex]; break;
                default: 
                    ++Result->n[colindex];
            }
		}
	}
}

void PrintInfo (RESULT* Result, const int colcount)
{
    int* start = &Result->a[0];
    int* current = start;
    char Tag[5] = {'A', 'T', 'C', 'G', 'N'};

    for (int i=0; i < 5; ++i) // Go through the structer by its storage mode!
    {
        printf("%c\t", Tag[i]);

        for (start; current < start + colcount; ++current)
            printf("%.2f\t",(float)(*current) / Result->TotalRowCount * 100);

        printf("\n");
        current = start += MAX_READLEN;
    }
}


