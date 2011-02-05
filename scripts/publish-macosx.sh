#!/bin/sh

VERSION=`date "+%Y.%m.%d"`
#VERSION=2010.05

# This is the same location as DEPLOYDIR in macosx-build-dependencies.sh
export MACOSX_DEPLOY_DIR=$PWD/../libraries/install

echo "Building ctrl-cut-$VERSION..."
qmake -recursive VERSION=$VERSION CONFIG+=deploy CONFIG-=debug ctrl-cut.pro
make -s clean
make -j4
if [[ $? != 0 ]]; then
  exit 1
fi

INSTALL_PRINTERS_DIR=/Library/Printers
INSTALL_CTRLCUT_DIR=${INSTALL_PRINTERS_DIR}/Ctrl-Cut
INSTALL_FILTER_DIR=/usr/libexec/cups/filter
INSTALL_BACKEND_DIR=/usr/libexec/cups/backend

EXECUTABLE=src/cups-filter/ctrl-cut

echo "Sanity check of the executable..."
DYLD_LIBRARY_PATH=${MACOSX_DEPLOY_DIR}/lib `dirname $0`/macosx-sanity-check.py $EXECUTABLE
if [[ $? != 0 ]]; then
  exit 1
fi

echo "Building package..."
# Point the executable to the installed location of the libraries
install_name_tool -change libgs.9.00.dylib ${INSTALL_CTRLCUT_DIR}/lib/libgs.9.00.dylib $EXECUTABLE

# Copy files to folder structure
rm -rf root
mkdir -p root${INSTALL_CTRLCUT_DIR}/Icons
cp images/EpilogLegend36EXT.icns root${INSTALL_CTRLCUT_DIR}/Icons
mkdir -p root${INSTALL_CTRLCUT_DIR}/lib
cp ${MACOSX_DEPLOY_DIR}/lib/libgs.9.00.dylib root${INSTALL_CTRLCUT_DIR}/lib
mkdir -p root${INSTALL_PRINTERS_DIR}/PPDs/Contents/Resources
cp etc/ppd/EpilogLegend36EXT.ppd root${INSTALL_PRINTERS_DIR}/PPDs/Contents/Resources
mkdir -p root${INSTALL_FILTER_DIR}
cp $EXECUTABLE root${INSTALL_FILTER_DIR}
mkdir -p root${INSTALL_BACKEND_DIR}
cp src/lpd-epilog/lpd-epilog root${INSTALL_BACKEND_DIR}

# Build package
/Developer/Applications/Utilities/PackageMaker.app/Contents/MacOS/PackageMaker --doc packaging/Ctrl-Cut.pmdoc --out Ctrl-Cut.pkg

rm -r root