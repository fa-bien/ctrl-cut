TARGET = ctrl-cut
#CONFIG += raster
CONFIG += gsapi
CONFIG += boost
CONFIG += cups
CONFIG += rsvg
CONFIG += cairo-ps
CONFIG += gio
CONFIG += boost_thread
CONFIG += libxml++
CONFIG += magick++

INCLUDEPATH += ../librsvg/

include(common.pri)

DEFINES += ETLOG DEBUG=4

SOURCES += \
	  Driver.cpp \
          Ctrl-Cut.cpp \
          LaserJob.cpp \
          PostscriptParser.cpp \
          util/LaserConfig.cpp \
          util/Eps.cpp \
	  util/Svg2Ps.cpp \
	  util/SvgFix.cpp \
	  util/SvgDocument.cpp \
          tri_logger/tri_logger.cpp \
          vector/HPGLEncoder.cpp \
          vector/graph/SegmentGraph.cpp \
          vector/graph/StringGraph.cpp \
	  vector/graph/Traverse.cpp \
	  vector/geom/Geometry.cpp \
          vector/model/CutModel.cpp \
	  vector/model/Explode.cpp \
          vector/graph/Deonion.cpp \
	  vector/model/Reduce.cpp

HEADERS += \
          LaserJob.h \
          FileParser.h \
          util/2D.h \
          util/PJL.h \
          util/Eps.h \
          util/Logger.h \
          util/LaserConfig.h \
          util/Svg2Ps.h \
	  util/SvgFix.h \
          util/SvgDocument.cpp \
	  Ctrl-Cut.h \
          Driver.h \
          vector/graph/Traverse.h \
          vector/graph/CutGraph.h \
	  vector/graph/SegmentGraph.h \
	  vector/graph/StringGraph.h \
          vector/geom/Geometry.h \
          vector/model/CutModel.h \
          vector/model/Explode.h \
	  vector/model/Reduce.h \
          vector/graph/Deonion.h \
          vector/HPGLEncoder.h

SOURCES += \
          raster/DownSample.cpp \
          raster/PclEncoder.cpp \
          raster/Raster.cpp \
          raster/Dither.cpp \
          raster/DitherFloydSteinberg.cpp \
          raster/DitherSierra3.cpp \
          raster/DitherBayer.cpp \
          raster/DitherJarvis.cpp \
          raster/DitherStucki.cpp \
          raster/DitherBurke.cpp \
          raster/DitherSierra2.cpp \
          raster/DitherThreshold.cpp

HEADERS  += \
          raster/DownSample.h \
          raster/AbstractImage.h \
          raster/Image.h \
          raster/MMapMatrix.h \
          raster/PclEncoder.h \
          raster/PPMFile.h \
          raster/Raster.h \
          raster/Dither.h \
          raster/DitherFloydSteinberg.h \
          raster/DitherSierra3.h \
          raster/DitherBayer.h \
          raster/DitherJarvis.h \
          raster/DitherStucki.h \
          raster/DitherBurke.h \
          raster/DitherSierra2.h \
          raster/DitherThreshold.h
