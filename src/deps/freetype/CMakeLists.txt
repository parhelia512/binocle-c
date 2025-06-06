project(freetype)

add_definitions(-DFT2_BUILD_LIBRARY)

include_directories(.)

set(SOURCES
        src/autofit/afblue.c
        src/autofit/afcjk.c
        src/autofit/afdummy.c
        src/autofit/afglobal.c
        src/autofit/afhints.c
        src/autofit/afindic.c
        src/autofit/aflatin.c
        src/autofit/afloader.c
        src/autofit/afmodule.c
        src/autofit/afranges.c
        src/autofit/afshaper.c
        src/autofit/autofit.c
        src/autofit/ft-hb.c
        src/base/ftadvanc.c
        src/base/ftbase.c
        src/base/ftbbox.c
        src/base/ftbdf.c
        src/base/ftbitmap.c
        src/base/ftcalc.c
        src/base/ftcid.c
        src/base/ftcolor.c
        src/base/ftdbgmem.c
        src/base/ftdebug.c
        src/base/fterrors.c
        src/base/ftfntfmt.c
        src/base/ftfstype.c
        src/base/ftgasp.c
        src/base/ftgloadr.c
        src/base/ftglyph.c
        src/base/ftgxval.c
        src/base/fthash.c
        src/base/ftinit.c
        src/base/ftlcdfil.c
        src/base/ftmac.c
        src/base/ftmm.c
        src/base/ftobjs.c
        src/base/ftotval.c
        src/base/ftoutln.c
        src/base/ftpatent.c
        src/base/ftpfr.c
        src/base/ftpsprop.c
        src/base/ftrfork.c
        src/base/ftsnames.c
        src/base/ftstream.c
        src/base/ftstroke.c
        src/base/ftsynth.c
        src/base/ftsystem.c
        src/base/fttrigon.c
        src/base/fttype1.c
        src/base/ftutil.c
        src/base/ftwinfnt.c
        #src/base/md5.c
        src/bdf/bdf.c
        src/bdf/bdfdrivr.c
        src/bdf/bdflib.c
        src/bzip2/ftbzip2.c
        src/cache/ftcache.c
        src/cache/ftcbasic.c
        src/cache/ftccache.c
        src/cache/ftccmap.c
        src/cache/ftcglyph.c
        src/cache/ftcimage.c
        src/cache/ftcmanag.c
        src/cache/ftcmru.c
        src/cache/ftcsbits.c
        src/cff/cff.c
        src/cff/cffcmap.c
        src/cff/cffdrivr.c
        src/cff/cfferrs.h
        src/cff/cffgload.c
        src/cff/cffload.c
        src/cff/cffobjs.c
        src/cff/cffparse.c
        src/cid/cidgload.c
        src/cid/cidload.c
        src/cid/cidobjs.c
        src/cid/cidparse.c
        src/cid/cidriver.c
        src/cid/type1cid.c
        src/gzip/adler32.c
        #src/gzip/crc32.c
        src/gzip/ftgzip.c
        #src/gzip/inffast.c
        #src/gzip/inflate.c
        #src/gzip/inftrees.c
        #src/gzip/zutil.c
        src/lzw/ftlzw.c
        src/lzw/ftzopen.c
        src/otvalid/otvalid.c
        src/otvalid/otvbase.c
        src/otvalid/otvcommn.c
        src/otvalid/otvgdef.c
        src/otvalid/otvgpos.c
        src/otvalid/otvgsub.c
        src/otvalid/otvjstf.c
        src/otvalid/otvmath.c
        src/otvalid/otvmod.c
        src/pcf/pcf.c
        src/pcf/pcfdrivr.c
        src/pcf/pcfread.c
        src/pcf/pcfutil.c
        src/pfr/pfr.c
        src/pfr/pfrcmap.c
        src/pfr/pfrdrivr.c
        src/pfr/pfrgload.c
        src/pfr/pfrload.c
        src/pfr/pfrobjs.c
        src/pfr/pfrsbit.c
        src/psaux/afmparse.c
        src/psaux/cffdecode.c
        src/psaux/psarrst.c
        src/psaux/psaux.c
        src/psaux/psauxmod.c
        src/psaux/psblues.c
        src/psaux/psconv.c
        src/psaux/pserror.c
        src/psaux/psfont.c
        src/psaux/psft.c
        src/psaux/pshints.c
        src/psaux/psintrp.c
        src/psaux/psobjs.c
        src/psaux/psread.c
        src/psaux/psstack.c
        src/psaux/t1cmap.c
        src/psaux/t1decode.c
        src/pshinter/pshalgo.c
        src/pshinter/pshglob.c
        src/pshinter/pshinter.c
        src/pshinter/pshmod.c
        src/pshinter/pshrec.c
        src/psnames/psmodule.c
        src/psnames/psnames.c
        src/raster/ftraster.c
        src/raster/ftrend1.c
        src/raster/raster.c
        src/sdf/ftbsdf.c
        src/sdf/ftsdf.c
        src/sdf/ftsdfcommon.c
        src/sdf/ftsdfrend.c
        src/sdf/sdf.c
        src/sfnt/pngshim.c
        src/sfnt/sfdriver.c
        src/sfnt/sfnt.c
        src/sfnt/sfobjs.c
        src/sfnt/sfwoff.c
        src/sfnt/sfwoff2.c
        src/sfnt/ttbdf.c
        src/sfnt/ttcmap.c
        src/sfnt/ttcolr.c
        src/sfnt/ttcpal.c
        src/sfnt/ttgpos.c
        src/sfnt/ttkern.c
        src/sfnt/ttload.c
        src/sfnt/ttmtx.c
        src/sfnt/ttpost.c
        src/sfnt/ttsbit.c
        src/sfnt/ttsvg.c
        src/sfnt/woff2tags.c
        src/smooth/ftgrays.c
        src/smooth/ftsmooth.c
        src/smooth/smooth.c
        src/svg/ftsvg.c
        src/svg/svg.c
        src/truetype/truetype.c
        src/truetype/ttdriver.c
        src/truetype/ttgload.c
        src/truetype/ttgxvar.c
        src/truetype/ttinterp.c
        src/truetype/ttobjs.c
        src/truetype/ttpload.c
        src/type1/t1afm.c
        src/type1/t1driver.c
        src/type1/t1gload.c
        src/type1/t1load.c
        src/type1/t1objs.c
        src/type1/t1parse.c
        src/type1/type1.c
        src/type42/t42drivr.c
        src/type42/t42objs.c
        src/type42/t42parse.c
        src/type42/type42.c
        src/winfonts/winfnt.c
)

set(HEADERS
        include/ft2build.h
        include/freetype/freetype.h
        include/freetype/config/ftconfig.h
        include/freetype/config/ftstdlib.h
)

set(MACOS_HEADERS
        include/freetype/ftmac.h
)

include_directories("include")
include_directories("include/freetype")
include_directories("include/freetype/config")
include_directories("include/freetype/internal")
include_directories("include/freetype/internal/services")
include_directories("src/autofit")
include_directories("src/base")
include_directories("src/gzip")
include_directories("src/lzw")
include_directories("src/psaux")
include_directories("src/psnames")
include_directories("src/raster")
include_directories("src/sfnt")
include_directories("src/smooth")
include_directories("src/truetype")

if (APPLE)
    add_library(freetype OBJECT ${HEADERS} ${MACOS_HEADERS} ${SOURCES})
else()
    add_library(freetype OBJECT ${HEADERS} ${SOURCES})
endif (APPLE)

if (IOS)
    set_target_properties(
            freetype
            PROPERTIES
            XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET 13.1
    )
endif (IOS)
