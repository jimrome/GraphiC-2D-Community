/* DEFINITIONS FOR MISSING AND EMPTY VALUES */

#ifndef MISSINGS_H
 #define MISSINGS_H

/* A missing float value */
typedef union {
	struct {
		unsigned char b1;
		unsigned char b2;
		unsigned char b3;
		unsigned char b4;
	} b;
	float f;
} MISSING_FLOAT_TYPE;


typedef union{
         struct {
          unsigned char b1;
          unsigned char b2;
          unsigned char b3;
          unsigned char b4;
          unsigned char b5;
          unsigned char b6;
          unsigned char b7;
          unsigned char b8;
         }b;
         double d;
} MISSING_DOUBLE_TYPE;

/***************** MISSING DATA VARIABLES ***********************/
extern MISSING_FLOAT_TYPE MISSING_FLOAT;
extern MISSING_FLOAT_TYPE EMPTY_FLOAT;
extern MISSING_FLOAT_TYPE AUTO_FLOAT;
extern MISSING_FLOAT_TYPE DATA_FLOAT;
extern MISSING_DOUBLE_TYPE MISSING_DOUBLE;

#ifndef FLOAT_MISSING
 #define FLOAT_MISSING MISSING_FLOAT.f
#endif

#ifndef FLOAT_EMPTY
 #define FLOAT_EMPTY EMPTY_FLOAT.f
#endif

#ifndef FLOAT_AUTO
 #define FLOAT_AUTO AUTO_FLOAT.f
#endif

#ifndef xFLOAT_DATA
 #define xFLOAT_DATA DATA_FLOAT.f
#endif

#ifndef DOUBLE_MISSING
 #define DOUBLE_MISSING MISSING_DOUBLE.d
#endif

#ifndef INT_MISSING
 #define INT_MISSING  -2147483647
#endif

#ifndef STRING_MISSING
 #define STRING_MISSING   NullStr
#endif


#define BYTEI( v, i )  ((unsigned char)(*((unsigned char *)&(v)+i)))
#define TESTBITS( b, n )      ((b & n) == n)
#define TESTBYTEI( v, i, n )  (TESTBITS( BYTEI( (v), i ), n ))
#define IsNotRealFloat( v )  ( TESTBYTEI( (v), 2, 128 ) && TESTBYTEI( (v), 3, 127 ) )
 
#ifndef IsExactMissingFloat
 #define IsExactMissingFloat( f ) (!memcmp(&f, &MISSING_FLOAT, sizeof(float)))
#endif

#ifndef IsMissFloat
 #define IsMissFloat( fval ) (!memcmp(&fval, &MISSING_FLOAT, sizeof(float)))
#endif

#ifndef IsMissDouble
 #define IsMissDouble( dval ) (!memcmp(&dval, &MISSING_DOUBLE, sizeof(double)))
#endif

#ifndef IsEmptyFloat
 #define IsEmptyFloat( f ) (!memcmp(&f, &EMPTY_FLOAT, sizeof(float)))
#endif

#ifndef IsAutoFloat
 #define IsAutoFloat( f ) (!memcmp(&f, &AUTO_FLOAT, sizeof(float)))
#endif

#ifndef IsDataFloat
 #define IsDataFloat( f ) (!memcmp(&f, &DATA_FLOAT, sizeof(float)))
#endif


#endif /* end of header info */

/* Use the following code to set a float value to a MISSING:
  
  float x = FLOAT_MISSING; // see macro above for defn of FLOAT_MISSING 

  // use the following to test for any NaN 

  if ( IsNotRealFloat( x ) )
  {
     // do something
  }
*/
