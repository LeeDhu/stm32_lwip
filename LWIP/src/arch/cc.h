/**
 * @file cc.h
 * @brief complete the type definitions and macros ...
 * @author Li Ming <Lee.dhucst@gmail.com>
 * @url http://blog.csdn.net/lim680
 * @version V1.0
 * @date 2014-04-14
 * @Copyright (c) DHU, CST.  All Rights Reserved
 */

#ifndef __CC_H
#define __CC_H

typedef unsigned char   u8_t;
typedef signed char     s8_t;
typedef unsigned short  u16_t;
typedef signed short    s16_t;
typedef unsigned int    u32_t;
typedef signed  int     s32_t;
typedef unsigned int    sys_prot_t;
typedef unsigned int    mem_ptr_t;

#define PACK_STRUCT_FIELD(x) x
#define PACK_STRUCT_STRUCT
#define PACK_STRUCT_BEGIN  __packed   /* size aligned */
#define PACK_STRUCT_END

/*
 * enable debug 
 * define debug output
 */
#define LWIP_DEBUG   
#define LWIP_PLATFORM_DIAG(x)    //do{Uart_printf x;} while(0)
#define LWIP_PLATFORM_ASSERT(x)  //do{Uart_printf(x);}while(0)


#define U16_F "4d"
#define S16_F "4d"
#define X16_F "4x"
#define U32_F "8ld"
#define S32_F "8ld"
#define X32_F "8lx"

#define LWIP_PROVIDE_ERRNO
#define BYTE_ORDER LITTLE_ENDIAN

#endif
