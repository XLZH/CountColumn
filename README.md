# CountColumn
Calculate the base(ATCGN) percent in specified column for a fastq file

### Compile:
        gcc -std=c99 -O3 -o CountCol CountCol.2.c
        
### Usage:

    ./CountCol -i <Infile(fastq)> -c <column Number>
              [-h]    This will print helpInfo.
              [-i]    [Required] Inputfile(fastq file).              
              [-c]    [Required] The column num(numeric) to be calculated!
