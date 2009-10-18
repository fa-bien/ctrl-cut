/*
 * cups_epilog.h
 *
 *  Created on: 19.09.2009
 *      Author: amir
 */

#ifndef CUPS_EPILOG_H_
#define CUPS_EPILOG_H_

/** Default for debug mode. */
#define DEBUG (1)

/** Number of bytes in the bitmap header. */
#define BITMAP_HEADER_NBYTES (54)

/** Basename for files generated by the program. */
#define FILE_BASENAME "epilog"

/** Number of characters allowable for a filename. */
#define FILENAME_NCHARS (128)

/** Maximum allowable hostname characters. */
#define HOSTNAME_NCHARS (1024)

/** Temporary directory to store files. */
#define TMP_DIRECTORY "/tmp"

#define PRINTER_NAME "LaserCutter"

#define GTKLP_CONF_DIR "/home/amir/dev/Epilog/gtklpconf/"

class CupsEpilog {
public:
	CupsEpilog();
	virtual ~CupsEpilog();

int big_to_little_endian(uint8_t *position, int bytes);
void range_checks(void);
bool execute_gtklp(char *printer_name,char *filename_cups, char *gtkconfdir);
bool execute_ghostscript(char *filename_bitmap, char *filename_eps,
                                char *filename_vector,
                                char *bmp_mode, int resolution,
                                int height, int width);
bool generate_raster(printer_config *pjob, laser_config *lconf, FILE *bitmap_file);
bool generate_vector(printer_config *pjob, laser_config *lconf, FILE *vector_file);
bool generate_pjl(printer_config *pjob, laser_config *lconf, FILE *bitmap_file, FILE *vector_file);
bool ps_to_eps(laser_config *lconf, FILE *ps_file, FILE *eps_file);
bool uri_parameters(printer_config pjob);
bool process_uri_options(laser_config *lconf, char *queue_options);
FILE* save_cups_file(FILE *file_cups_in, char* filename_cups_save);

};

#endif
