#include "FileParser.h"
#include "util/Logger.h"
#include "util/Eps.h"
#include <util/Measurement.h>
#include <cups/cups.h>
#include <cups/file.h>

#ifdef USE_GHOSTSCRIPT_API
#include <sstream>
#include <ghostscript/iapi.h>
#include <ghostscript/ierrors.h>
#include <ghostscript/gdevdsp.h>
#include "boost/format.hpp"
using boost::str;
using boost::format;
#else
#ifndef __APPLE__
#define GS_EXECUTABLE "/usr/bin/gs"
#else
#define GS_EXECUTABLE "/opt/local/bin/gs"
#endif
#endif

using std::string;

PostscriptParser *PostscriptParser::inst = NULL;

#ifdef USE_GHOSTSCRIPT_API
std::stringstream vectorbuffer;
static int GSDLLCALL
gsdll_stdout(void *, const char *str, int len)
{
  vectorbuffer.write(str, len);
  return len;
}
#endif

/**
 * Execute ghostscript feeding it an ecapsulated postscript file which
 * is then converted into a bitmap image. As a byproduct, output of
 * the ghostscript process is redirected to the global stringstream
 * vectorbuffer, which will contain instructions on how to perform a
 * vector cut of lines within the postscript.
 *
 * @param filename_bitmap the filename to use for the resulting bitmap file.
 * @param filename_eps the filename to read in encapsulated postscript from.
 * @param rasterdriver a string which contains the ghostscript driver to use (ppmraw, display, nullpage)
 * @param resolution the encapsulated postscript resolution.
 * @param height the postscript height in points per inch.
 * @param width the postscript width in points per inch.
 *
 * @return Return true if the execution of ghostscript succeeds, false
 * otherwise.
 */
#ifdef USE_GHOSTSCRIPT_API
static int display_open(void *, void *)
{
  LOG_DEBUG("display_open()");
  return 0;
}

static int display_preclose(void *, void *)
{
  LOG_DEBUG("display_preclose()");
  return 0;
}

static int display_close(void *, void *)
{
  LOG_DEBUG("display_close()");
  return 0;
}

static int display_presize(void *, void *, int width, int height, 
                           int /*raster*/, unsigned int /*fmt*/)
{
  LOG_DEBUG(str(format("display_presize(%d,%d)") % width % height));
  return 0;
}

/*
  Called by ghostscript when a page is resized. width, height is in pixels, rowstride is in bytes.
  fmt is the same bitmask sent as the -dDisplayFormat parameter.
*/   
static int display_size(void *, void *, int width, int height, 
                        int rowstride, unsigned int /*fmt*/, unsigned char *pimage)
{
  LOG_DEBUG(str(format("display_size(%d,%d)") % width % height));
  //  fprintf(stderr, "    pimage: %p\n", pimage);

  //FIXME unused
  //int color = fmt & DISPLAY_COLORS_MASK;
  //int depth = fmt & DISPLAY_DEPTH_MASK;

  // FIXME: Verify format

  PostscriptParser::instance()->createImage(width, height, pimage, rowstride);

  return 0;
}
   
static int display_sync(void *, void *)
{
  LOG_DEBUG("display_sync()");

  return 0;
}

/*!
  Called by ghostscript when a page has finished rendering.
  NB! The memory buffer will be cleared to background color by ghostscript after
  this function has returned.
*/
static int display_page(void *, void *, int /*copies*/, int /*flush*/)
{
  LOG_DEBUG("display_page()");

  PostscriptParser::instance()->copyPage();

  return 0;
}

#if 0
static int display_update(void *handle, void *device, 
                          int x, int y, int w, int h)
{
  LOG_DEBUG(str(format("display_update([ %d , %d , %d , % d])") % x % y % w % h));
  return 0;
}

static void *display_memalloc(void *handle, void *device, unsigned long size)
{
  return 0;
}

static int display_memfree(void *handle, void *device, void *mem)
{
  return 0;
}
#endif

static int display_separation(void *, void *,
                              int /*comp_num*/, const char */*name*/,
                              unsigned short /*c*/, unsigned short /*m*/,
                              unsigned short /*y*/, unsigned short /*k*/)
{
  LOG_DEBUG("display_separation()");
  return 0;
}

// callback structure for display device
display_callback display_callbacks = { 
    sizeof(display_callback),
    DISPLAY_VERSION_MAJOR,
    DISPLAY_VERSION_MINOR,
    display_open,
    display_preclose,
    display_close,
    display_presize,
    display_size,
    display_sync,
    display_page,
    NULL, // display_update,
    NULL, // display_memalloc,
    NULL, // display_memfree,
    display_separation
};

bool PostscriptParser::execute_ghostscript(const std::vector<std::string> &argstrings)
{
  int gsargc = argstrings.size();
  const char *gsargv[gsargc];
  for (int i=0;i<gsargc;i++) {
    gsargv[i] = argstrings[i].c_str();
    LOG_DEBUG(gsargv[i]);
  }
  void *minst;
  int code = gsapi_new_instance(&minst, NULL);
  if (code < 0) {
    LOG_ERR_MSG("gsapi_new_instance() failed", code);
    return false;
  }
  gsapi_set_stdio(minst, NULL, gsdll_stdout, NULL);
  if (!this->rendertofile) {
    code = gsapi_set_display_callback(minst, &display_callbacks);
  }
  code = gsapi_init_with_args(minst, gsargc, (char **)gsargv);

  int code1 = gsapi_exit(minst);
  if ((code == 0) || (code == e_Quit)) {
    code = code1;
  }

  gsapi_delete_instance(minst);

  if ((code == 0) || (code == e_Quit)) {
    return true;
  }
  return false;
}
#endif

#ifndef USE_GHOSTSCRIPT_API
bool PostscriptParser::execute_ghostscript_cmd(const std::vector<std::string> &argstrings)
{
  string cmdstr;
  for (int i=0;i<argstrings.size();i++) {
    cmdstr += argstrings[i] + " ";
  }

  cmdstr += "> ";
  cmdstr += this->filename_vector;

  LOG_DEBUG(cmdstr);

  if (system(cmdstr.c_str())) {
    return false;
  }

  return true;
}
#endif

PostscriptParser::PostscriptParser(DocumentSettings &conf) 
  : FileParser(conf), rendertofile(false), rasterformat(BITMAP), gsimage(NULL), image(NULL)
{
  PostscriptParser::inst = this;
}

PostscriptParser::~PostscriptParser()
{
  // Cleanup unneeded files provided that debug mode is disabled.
  if (cc_loglevel > CC_DEBUG) {
    if (unlink(this->filename_eps.c_str())) {
      LOG_FATAL_MSG("unlink failed", this->filename_eps);
    }
    if (this->conf.get<bool>(DocumentSettings::ENABLE_RASTER)) {
      if (unlink(this->filename_bitmap.c_str())) {
        LOG_FATAL_MSG("unlink failed", this->filename_bitmap);
      }
    }
  }
  delete gsimage;
  delete image;

  PostscriptParser::inst = NULL;
}

bool PostscriptParser::parse(cups_file_t *input_file)
{
  uint32_t resolution = this->conf.get<uint16_t>(DocumentSettings::RESOLUTION);
  double width = this->conf.get<Measurement>(DocumentSettings::WIDTH).in(PX, resolution);
  double height = this->conf.get<Measurement>(DocumentSettings::HEIGHT).in(PX, resolution);
  bool enable_raster = this->conf.get<bool>(DocumentSettings::ENABLE_RASTER);
  string tmpbasepath = this->conf.get<string>(DocumentSettings::TEMP_DIR) + "/" + this->conf.get<string>(DocumentSettings::BASENAME);
  this->filename_eps = tmpbasepath + ".eps";

  if (!createEps(input_file, this->filename_eps)) return false;

  std::vector<std::string> argstrings;
  argstrings.push_back("gs");
  argstrings.push_back("-q");
  argstrings.push_back("-dBATCH");
  argstrings.push_back("-dNOPAUSE");

  argstrings.push_back(str(format("-r%d") % resolution));
  argstrings.push_back(str(format("-g%ux%u") % width % height));

  if (!enable_raster) {
    argstrings.push_back("-sDEVICE=nullpage");
  }
  else if (this->rendertofile) {
    this->filename_bitmap = tmpbasepath;
    switch (this->rasterformat) {
    case BITMAP: 
      argstrings.push_back("-sDEVICE=pbmraw");
      this->filename_bitmap += ".pbm";
      break;
    case GRAYSCALE:
      argstrings.push_back("-sDEVICE=pgmraw");
      this->filename_bitmap += ".pgm";
      break;
    case RGB:
      argstrings.push_back("-sDEVICE=ppmraw");
      this->filename_bitmap += ".ppm";
    default:
      assert(false && "Illegal value for displayformat");
    }
    argstrings.push_back(str(format("-sOutputFile=%s") % filename_bitmap));

    LOG_DEBUG_MSG("Running ghostscript. Raster output", this->filename_bitmap);
  }
  else {
    argstrings.push_back("-sDEVICE=display");
    int formatflags = DISPLAY_ALPHA_NONE | DISPLAY_BIGENDIAN | DISPLAY_TOPFIRST;
    switch (this->rasterformat) {
    case BITMAP:
      formatflags |= DISPLAY_DEPTH_1 | DISPLAY_COLORS_GRAY;
      break;
    case GRAYSCALE:
      formatflags |= DISPLAY_DEPTH_8 | DISPLAY_COLORS_GRAY;
      break;
    case RGB:
      formatflags |= DISPLAY_DEPTH_8 | DISPLAY_COLORS_RGB;
      break;
    default:
      assert(false && "Illegal raster format");
    }
    argstrings.push_back(str(format("-dDisplayFormat=%d") % formatflags));
    LOG_DEBUG("Running ghostscript...");
  }

  argstrings.push_back(filename_eps);

#ifdef USE_GHOSTSCRIPT_API
  if (!execute_ghostscript(argstrings)) {
    LOG_FATAL_STR("ghostscript failed");
    return false;
  }
#else
  this->filename_vector = this->conf.TEMPDIR + "/" + this->conf.basename + ".vector";
  LOG_DEBUG_MSG("                     Vector output", this->filename_vector);
  if (!execute_ghostscript_cmd(argstrings)) {
    LOG_FATAL_STR("ghostscript failed");
    return false;
  }
#endif
  
  return true;
}

bool PostscriptParser::createEps(cups_file_t *input_file, const string &filename_eps)
{
  // Open the encapsulated postscript file for writing.
  FILE *file_eps = fopen(filename_eps.c_str(), "w");
  if (!file_eps) {
    LOG_FATAL_MSG("Can't open", filename_eps);
    return false;
  }
  LOG_DEBUG_MSG("Converting to EPS file", filename_eps);
  
  // Convert PS to EPS (for vector extraction)
  if (!ps_to_eps(&this->conf, input_file, file_eps)) {
    LOG_FATAL_STR("ps_to_eps failed");
    fclose(file_eps);
    return false;
  }
  // Cleanup after encapsulated postscript creation.
  fclose(file_eps);
  if (input_file != cupsFileStdin()) cupsFileClose(input_file);

  return true;
}

std::istream &PostscriptParser::getVectorData()
{
#ifdef USE_GHOSTSCRIPT_API
  return vectorbuffer;
#else
  if (this->vectorfile.is_open()) {
    this->vectorfile.seekg(0);
  }
  else {
    this->vectorfile.open(this->filename_vector.c_str(), std::ios_base::in);
  }
  return this->vectorfile;
#endif  
}

#if 0
void PostscriptParser::printStatistics()
{
  unsigned int r=0,g=0,b=0;
  for (int j=0;j<this->bitmapheight;j++) {
    for (int i=0;i<this->bitmapwidth;i++) {
      uint8_t *buf = (uint8_t *)this->bitmapdata;
      if (buf[(j*bitmapwidth+i)*3] != 255) 
        r++;
      if (buf[(j*bitmapwidth+i)*3+1] != 255) g++;
      if (buf[(j*bitmapwidth+i)*3+2] != 255) b++;
    }
  }
  LOG_DEBUG(r);
  LOG_DEBUG(g);
  LOG_DEBUG(b);
}
#endif

void PostscriptParser::copyPage()
{
  Rectangle cropbox = this->gsimage->autocrop();
  this->image = this->gsimage->copy(cropbox);

  // For debugging, we can export the image here:
#if 0
  BitmapImage *bitmap = dynamic_cast<BitmapImage*>(this->gsimage);
  if (bitmap) bitmap->saveAsPBM("/tmp/out.pbm");
  else {
    GrayscaleImage *gimage = dynamic_cast<GrayscaleImage*>(this->gsimage);
    if (gimage) gimage->saveAsPGM("/tmp/out.pgm");
  }
#endif
}

/*
  Set rowstride to != 0 to use a different rowstride than the natural one.
*/
void PostscriptParser::createImage(uint32_t width, uint32_t height, void *pimage, uint32_t rowstride)
{
  delete this->gsimage;
  delete this->image;
  if (this->rasterformat == BITMAP) {
    this->gsimage = new BitmapImage(width, height, (uint8_t *)pimage);
    if (rowstride != 0) this->gsimage->setRowstride(rowstride);
  }
  else if (this->rasterformat == GRAYSCALE) {
    this->gsimage = new GrayscaleImage(width, height, 1, (uint8_t *)pimage);
    if (rowstride != 0) this->gsimage->setRowstride(rowstride);
  }
  else {
    LOG_FATAL_MSG("Raster format not implemented", this->rasterformat);
  }
}