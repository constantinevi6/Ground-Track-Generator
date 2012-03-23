#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gtg.h"
#include "gtgutil.h"
#include "gtgtrace.h"

#include "gtgshp.h"

struct configuration cfg;

int main(int argc, char *argv[])
{
	int opt = 0;
	int longIndex = 0;
	
	/* All attributes turned off by default */
	FlagAllAttributes(false);
	
	/* Initialize default configuration */
	cfg.start = NULL; /* NULL start implies epoch start time */
	cfg.end = NULL;
	cfg.unit = minutes;
	cfg.interval = 1.0;
	cfg.steps = 100;
	cfg.tleText = NULL;
	cfg.tlePath = NULL;
	cfg.shpPath = NULL;
	cfg.features = point;
	cfg.verbose = 0;
	cfg.split = 0;
	
	/* Suppress getopt_long from printing its own error/warning messages */
	opterr = 0;

	/* Expected arguments for getopt_long */
	const char *optString = "a:d:e:f:?i:l:o:s:n:t:u:v";
	const struct option longOpts[] = {
			{"attributes", required_argument, NULL, 'a'},
			{"end", required_argument, NULL, 'e'},
			{"features", required_argument, NULL, 'f'},
			{"help", no_argument, NULL, '?'},
			{"input", required_argument, NULL, 'i'},
			{"interval", required_argument, NULL, 'l'},
			{"output", required_argument, NULL, 'o'},
			{"split", no_argument, NULL, 'd'},
			{"start", required_argument, NULL, 's'},
			{"steps", required_argument, NULL, 'n'},
			{"tle", required_argument, NULL, 't'},
			{"unit", required_argument, NULL, 'u'},
			{"verbose", no_argument, &cfg.verbose, 1},
			{"version", no_argument, NULL, 'v'},
			{NULL, no_argument, NULL, 0}
	};
	
	/* Store command line arguments and perform some preliminary validation */
	while(-1 != (opt = getopt_long_only(argc, argv, optString, longOpts, &longIndex))) {
		switch(opt) {
			
			case 'd':
				/* Split line segments that cross the 180th meridian/dateline */
				cfg.split = 1;
				break;
			
			case 'a':
				/* Attributes */
				if (0 == strcmp("all", optarg)) {
					FlagAllAttributes(true);
				} else {
				
					/* first attribute argument is required */
					if (not EnableAttribute(optarg)) {
						Fail("invalid attribute: %s\n", optarg);
					}
					
					/* subsequent attribute arguments, if present, are optional */
					/* if a subsequent argument doesn't look like an attribute, */
					/* just return control to getopt to handle as another opt */
					while (optind < argc) {
						if (EnableAttribute(argv[optind])) {
							optind++;
						} else {
							break;
						}
					}
				}				
				break;
			
			case 's':
				/* Start */
				if(NULL != cfg.start) {
					Fail("start already specified: %s\n", cfg.start);
				}
				cfg.start = optarg;
				break;
			
			case 'e':
				/* End */
				if (NULL != cfg.end) {
					Fail("end already specified: %s\n", cfg.end);
				}
				cfg.end = optarg;
				break;
			
			case 'u':
				/* Interval units */
				/* Accepted argument values: seconds, minutes, hours, days */
				if (0 == strcmp("seconds", optarg)) {
					cfg.unit = seconds;
				} else if (0 == strcmp("minutes", optarg)) {
					cfg.unit = minutes;
				} else if (0 == strcmp("hours", optarg)) {
					cfg.unit = hours;
				} else if (0 == strcmp("days", optarg)) {
					cfg.unit = days;
				} else {
					Fail("invalid unit: %s (should be seconds, minutes, hours, or days)\n", optarg);
				}
				break;
			
			case 'l':
				/* Interval length */
				/* Argument format: positive floating point number */
				if (1 != sscanf(optarg, "%lf", &cfg.interval)) {
					Fail("cannot parse interval: %s (should be positive number)\n", optarg);
				}
				if (cfg.interval <= 0.0) {
					Fail("invalid interval: %s (should be positive number)\n", cfg.interval);
				}
				break;
			
			case 'n':
				/* Steps */
				/* Argument format: integer >= 1 */
				if (1 != sscanf(optarg, "%d", &cfg.steps)) {
					Fail("cannot parse steps: %s (should be positive integer)\n", optarg);
				}
				if (cfg.steps <= 0) {
					Fail("invalid steps: %s (should be positive integer)\n", cfg.steps);
				}
				break;
			
			case 't':
				/* TLE text */
				/* Argument format: text to be parsed for TLE, ala runtest */
				if (NULL != cfg.tleText) {
					Fail("TLE text already specified\n");
				}
				if (NULL != cfg.tlePath) {
					Fail("TLE file already specified: %s (cannot use --tle and --input)\n", cfg.tlePath);
				}
				cfg.tleText = optarg;
				break;
			
			case 'i':
				/* Input file */
				/* Argument format: path to file to read for TLE, ala runtest */
				if (NULL != cfg.tlePath) {
					Fail("TLE file already specified\n");
				}
				if (NULL != cfg.tleText) {
					Fail("TLE text already specified (cannot use --input and --tle)\n");
				}
				cfg.tlePath = optarg;
				break;
			
			case 'o':
				/* Output file */
				/* Argument format: path to output shapefile basename */
				if (NULL != cfg.shpPath) {
					Fail("output shapefile already specified: %s\n", cfg.shpPath);
				}
				cfg.shpPath = optarg;
				break;
			
			case 'f':
				/* Feature type */
				/* Accepted argument values: point, line */
				if (0 == strcmp("point", optarg)) {
					cfg.features = point;
				} else if (0 == strcmp("line", optarg)) {
					cfg.features = line;
				} else {
					Fail("invalid feature type: %s (should be point or line)\n", optarg);
				}
				break;
			
			case 'v':
				/* Version information */
				ShowVersion();
				break;
				
			case 0:
				/* (options that just store val in flag, such as --verbose) */
				/* (exact option indicated by longOpts[longIndex].name) */
				break;
			
			case '?':
			default:
				/* Help / usage OR unrecognized options */
				if ((0 == strcmp("help", longOpts[longIndex].name))
						|| (0 == strcmp("-?", argv[optind - 1]))) {
					ShowHelp();
				} else {
					Fail("unrecognized option: %s (try --help)\n", argv[optind - 1]);
				}
				break;
		}
	}
	
	/* Positional arguments */
	argv += optind;
	argc -= optind;
	
	/* If an end time is specified, use that instead of steps to constrain output */
	if (NULL != cfg.end) {
		cfg.steps = 0;
	}
	
	/* If output shpPath was not specified as an option, take the first
	   remaining argument as shpPath and remove it from the list of remnants */
	if (NULL == cfg.shpPath) {
		if (argc < 1) {
			Fail("no output shapefile specified\n");
		}
		cfg.shpPath = argv[0];
		argv++;
		argc--;
	}
	
	if (argc > 0) {
		Fail("extraneous command line argument: %s\n", argv[0]);
	}
	
	/* to read remaining arguments as attributes */
	/*for (int i = 0; i < argc; i++) {
		if (not EnableAttribute(argv[i])) {
			Fail("invalid attribute: %s\n", argv[i]);
		}
	}*/
		
	StartGroundTrack();
	
	return EXIT_SUCCESS;
}
