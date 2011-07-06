#ifndef GLOBALDEFS_INCLUDED
#define GLOBALDEFS_INCLUDED

#define TF_TABLE_SIZE (256)

#ifndef M_PI
#define M_PI (3.14159)
#endif

#define EPSILON (0.000001)
#define START_EPSILON (0.01)
#define START_EPSILON1 (0.009)

#define RATIO

//#define SPHERE
#define CONE

#ifdef ENGINE
#define FILENAME ("/Users/000617123/Desktop/data/Engine256.raw")
#define XMAX (256)
#define YMAX (256)
#define ZMAX (256)

#define XSIZE 1.0  
#define YSIZE 1.0
#define ZSIZE 1.0
#endif

#ifdef CTA
#define FILENAME ("/Users/000617123/Desktop/data/CTA.raw")
#define XMAX (512)
#define YMAX (512)
#define ZMAX (79)

#define XSIZE 1.0  
#define YSIZE 1.0
#define ZSIZE 0.1617646188829

#define XSCALE 1.0  
#define YSCALE 1.0
#define ZSCALE 0.1617646188829
#endif

#ifdef ASTRO
#define VOL1_SLIT1 ("/Users/000617123/Desktop/data/1068s1g.txt")
#define VOL1_SLIT2 ("/Users/000617123/Desktop/data/1068s2g.txt")
#define VOL1_SLIT3 ("/Users/000617123/Desktop/data/1068s3g.txt")
#define VOL1_SLIT4 ("/Users/000617123/Desktop/data/1068s4g.txt")
#define VOL1_SLIT5 ("/Users/000617123/Desktop/data/1068s5g.txt")
#define VOL1_SLIT6 ("/Users/000617123/Desktop/data/1068s6g.txt")
#define VOL2_SLIT1 ("/Users/000617123/Desktop/data/1068s1a.txt")
#define VOL2_SLIT2 ("/Users/000617123/Desktop/data/1068s2a.txt")
#define VOL2_SLIT3 ("/Users/000617123/Desktop/data/1068s3a.txt")
#define VOL2_SLIT4 ("/Users/000617123/Desktop/data/1068s4a.txt")
#define VOL2_SLIT5 ("/Users/000617123/Desktop/data/1068s5a.txt")
#define VOL2_SLIT6 ("/Users/000617123/Desktop/data/1068s6a.txt")
#define GRAD_X     ("/Users/000617123/Desktop/data/1068gAllNNx.txt")
#define GRAD_Y     ("/Users/000617123/Desktop/data/1068gAllNNy.txt")

#define XMAX (256)
#define YMAX (256)
#define ZMAX (64)

#define XSIZE 1.0  
#define YSIZE 1.0
#define ZSIZE 0.25 

#define XSCALE 0.718750
#define YSCALE 1.000000
#define ZSCALE 0.234375
#endif

#ifdef RATIO
#define BLOB_FILE  ("/Users/000617123/Desktop/data/blobID.txt")
#define VOL1_SLIT1 ("/Users/000617123/Desktop/data/gaussian/gaussianCompSlit1.bin")
#define VOL1_SLIT2 ("/Users/000617123/Desktop/data/gaussian/gaussianCompSlit2.bin")
#define VOL1_SLIT3 ("/Users/000617123/Desktop/data/gaussian/gaussianCompSlit3.bin")
#define VOL1_SLIT4 ("/Users/000617123/Desktop/data/gaussian/gaussianCompSlit4.bin")
#define VOL1_SLIT5 ("/Users/000617123/Desktop/data/gaussian/gaussianCompSlit5.bin")
#define VOL1_SLIT6 ("/Users/000617123/Desktop/data/gaussian/gaussianCompSlit6.bin")
#define VOL2_SLIT1 ("/Users/000617123/Desktop/data/1068s1g.txt")
#define VOL2_SLIT2 ("/Users/000617123/Desktop/data/1068s2g.txt")
#define VOL2_SLIT3 ("/Users/000617123/Desktop/data/1068s3g.txt")
#define VOL2_SLIT4 ("/Users/000617123/Desktop/data/1068s4g.txt")
#define VOL2_SLIT5 ("/Users/000617123/Desktop/data/1068s5g.txt")
#define VOL2_SLIT6 ("/Users/000617123/Desktop/data/1068s6g.txt")
#define VOL3_SLIT1 ("/Users/000617123/Desktop/data/fwhm/fwhms1a.txt")
#define VOL3_SLIT2 ("/Users/000617123/Desktop/data/fwhm/fwhms2a.txt")
#define VOL3_SLIT3 ("/Users/000617123/Desktop/data/fwhm/fwhms3a.txt")
#define VOL3_SLIT4 ("/Users/000617123/Desktop/data/fwhm/fwhms4a.txt")
#define VOL3_SLIT5 ("/Users/000617123/Desktop/data/fwhm/fwhms5a.txt")
#define VOL3_SLIT6 ("/Users/000617123/Desktop/data/fwhm/fwhms6a.txt")
#define VOL4_SLIT1 ("/Users/000617123/Desktop/data/testmod/FakeMod1.txt")
#define VOL4_SLIT2 ("/Users/000617123/Desktop/data/testmod/FakeMod2.txt")
#define VOL4_SLIT3 ("/Users/000617123/Desktop/data/testmod/FakeMod3.txt")
#define VOL4_SLIT4 ("/Users/000617123/Desktop/data/testmod/FakeMod4.txt")
#define VOL4_SLIT5 ("/Users/000617123/Desktop/data/testmod/FakeMod5.txt")
#define VOL4_SLIT6 ("/Users/000617123/Desktop/data/testmod/FakeMod6.txt")

#define XMAX (256)
#define YMAX (256)
#define ZMAX (64)

#define XSIZE 1.0  
#define YSIZE 1.0
#define ZSIZE 0.25 

//#define XSCALE 0.718750
#define XSCALE 1.000000
#define YSCALE 1.000000
#define ZSCALE 0.187500
#endif

#define CLAMP(a,b,c) { b = ((b) < (a))? (a) : (((b) > (c))? (c): (b));}

#endif