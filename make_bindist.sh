#!/usr/bin/env bash
DISTDIR=dist/abstimmimimi
rm -rf dist
mkdir -p $DISTDIR
cp abstimmimimi $DISTDIR
strip $DISTDIR/abstimmimimi.exe
LIBS=(
	libstdc++-6.dll\
	libgcc_s_seh-1.dll\
	libglibmm-2.4-1.dll\
	libglib-2.0-0.dll\
	libgio-2.0-0.dll\
	libgiomm-2.4-1.dll\
	libwinpthread-1.dll\
	libgmodule-2.0-0.dll\
	libgobject-2.0-0.dll\
	zlib1.dll\
	libintl-8.dll\
	libsigc-2.0-0.dll\
	libffi-7.dll\
	libiconv-2.dll\
	libpcre-1.dll\
	libpcre2-8-0.dll\
	libatkmm-1.6-1.dll\
	libatk-1.0-0.dll\
	libgtk-3-0.dll\
	libgtkmm-3.0-1.dll\
	libpango-1.0-0.dll\
	libpangomm-1.4-1.dll\
	libcairomm-1.0-1.dll\
	libcairo-2.dll\
	libpangocairo-1.0-0.dll\
	libgdk-3-0.dll\
	libgdkmm-3.0-1.dll\
	libgdk_pixbuf-2.0-0.dll\
	libpangoft2-1.0-0.dll\
	libpangowin32-1.0-0.dll\
	libfontconfig-1.dll\
	libfreetype-6.dll\
	libcairo-gobject-2.dll\
	libepoxy-0.dll\
	libharfbuzz-0.dll\
	libpixman-1-0.dll\
	libpng16-16.dll\
	libexpat-1.dll\
	libbz2-1.dll\
	libgraphite2.dll\
	libjpeg-8.dll\
	liblzma-5.dll\
	libtiff-5.dll\
	libfribidi-0.dll\
	libthai-0.dll\
	libbrotlicommon.dll\
	libbrotlidec.dll\
	libdatrie-1.dll\
)
for LIB in "${LIBS[@]}"
do
   cp /mingw64/bin/$LIB $DISTDIR
done

mkdir -p $DISTDIR/share/icons
#cp -r /mingw64/share/icons/Adwaita $DISTDIR/share/icons
#cp -r /mingw64/share/icons/hicolor $DISTDIR/share/icons
rm -rf $DISTDIR/share/icons/Adwaita/cursors

mkdir -p $DISTDIR/lib
cp -r /mingw64/lib/gdk-pixbuf-2.0 $DISTDIR/lib
gdk-pixbuf-query-loaders > $DISTDIR/lib/gdk-pixbuf-2.0/2.10.0/loaders.cache
rm $DISTDIR/lib/gdk-pixbuf-*/*/loaders/*.a

mkdir -p $DISTDIR/share/glib-2.0/schemas
cp /mingw64/share/glib-2.0/schemas/gschemas.compiled $DISTDIR/share/glib-2.0/schemas

git log -10 | unix2dos > dist/log.txt
if [ "$1" != "-n" ]; then
	cd dist
	zip -r abstimmimimi-$(date +%Y-%m-%d-%H%M).zip abstimmimimi log.txt
fi
