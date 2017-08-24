#define ADLINK_MATRIX_DIO_API

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned char  U8;
typedef short          I16;
typedef unsigned short U16;
typedef long           I32;
typedef unsigned long  U32;
typedef float          F32;
typedef double         F64;

/*
 * Error Number
 */
#define MatrixNoError               0
#define ErrorOpenDriverFailed      -13
#define ErrorDeviceIoctl           -200

I16 __stdcall GPIO_Init();
I16 __stdcall GPI_Read(U16* pwState);
I16 __stdcall GPO_Write(U16 wState);
I16 __stdcall GPO_Read(U16* pwState);

I16 __stdcall ADMX_DIO_Init();
I16 __stdcall ADMX_DIO_Close();
I16 __stdcall ADMX_DI_ReadPort(U32* pwState);
I16 __stdcall ADMX_DO_WritePort(U32 wState);
I16 __stdcall ADMX_DO_ReadPort(U32* pwState);

//I16 __stdcall ADMX_DI_ReadLine(U16 Line, U16 *State);
//I16 __stdcall ADMX_DO_WriteLine(U16 Line, U16 Value);
//I16 __stdcall ADMX_DO_ReadLine(U16 Line, U16 *Value);

#ifdef __cplusplus
}
#endif
