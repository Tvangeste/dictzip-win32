/* maa.h -- Header file for visible libmaa functions
 * Created: Sun Nov 19 13:21:21 1995 by faith@dict.org
 * Copyright 1994-1998, 2002 Rickard E. Faith (faith@dict.org)
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Library General Public License as published
 * by the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 * 
 */

#ifndef _MAA_H_
#define _MAA_H_

#include <stdio.h>
#include <stdarg.h>

#ifndef __GNUC__
#define __attribute__(x)
#endif

				/* If MAA_MAGIC is non-zero, important
                                   structures will be tagged with a magic
                                   number which will be checked for
                                   integrity at each access.  This uses up
                                   more memory, and is slightly slower, but
                                   helps debugging quite a bit. */
#ifndef MAA_MAGIC
#define MAA_MAGIC 1
#endif

#if MAA_MAGIC
#define HSH_MAGIC               0x01020304
#define HSH_MAGIC_FREED         0x10203040
#define SET_MAGIC               0x02030405
#define SET_MAGIC_FREED         0x20304050
#define LST_MAGIC               0x03040506
#define LST_MAGIC_FREED         0x30405060
#define MEM_STRINGS_MAGIC       0x23232323
#define MEM_STRINGS_MAGIC_FREED 0x32323232
#define MEM_OBJECTS_MAGIC       0x42424242
#define MEM_OBJECTS_MAGIC_FREED 0x24242424
#define ARG_MAGIC               0xfeedbead
#define ARG_MAGIC_FREED         0xefdeebda
#define PR_MAGIC                0x0bad7734
#define PR_MAGIC_FREED          0xb0da7743
#define SL_LIST_MAGIC           0xabcdef01
#define SL_LIST_MAGIC_FREED     0xbadcfe10
#define SL_ENTRY_MAGIC          0xacadfeed
#define SL_ENTRY_MAGIC_FREED    0xcadaefde
#endif

/* version.c */

extern const char *maa_revision_string;


/* maa.c */

#define MAA_MEMORY (3U<<30|1<<29) /* Print memory usage statistics at exit */
#define MAA_TIME   (3U<<30|1<<28) /* Print timer information at exit       */
#define MAA_PR     (3U<<30|1<<27) /* Debug process routines                */
#define MAA_SL     (3U<<30|1<<26) /* Debug skip list routines              */
#define MAA_PARSE  (3U<<30|1<<25) /* Debug parsing                         */
#define MAA_SRC    (3U<<30|1<<24) /* Source library                        */

extern void       maa_init( const char *programName );
extern void       maa_shutdown( void );
extern int        maa_version_major( void );
extern int        maa_version_minor( void );
extern const char *maa_version( void );

/* xmalloc.c */
#ifndef DMALLOC_FUNC_CHECK
extern void *xmalloc( size_t size );
extern void *xcalloc( size_t num, size_t size );
extern void *xrealloc( void *pt, size_t size );
extern void xfree( void *pt );
extern char *xstrdup( const char *s );
#endif


/* bit.c */
extern void bit_set( unsigned long *flags, int bit );
extern void bit_clr( unsigned long *flags, int bit );
extern int  bit_tst( unsigned long *flags, int bit );
extern int  bit_cnt( unsigned long *flags );

/* prime.c */

extern int           prm_is_prime( unsigned int value );
extern unsigned long prm_next_prime( unsigned int start );

/* hash.c */

typedef void *hsh_HashTable;
typedef void *hsh_Position;

typedef struct hsh_Stats {
   unsigned long size;		 /* Size of table */
   unsigned long resizings;	 /* Number of resizings */
   unsigned long entries;	 /* Total entries in table */
   unsigned long buckets_used;	 /* Number of hash buckets in use */
   unsigned long singletons;	 /* Number of length one lists */
   unsigned long maximum_length; /* Maximum list length */

   unsigned long retrievals;	 /* Total number of retrievals */
   unsigned long hits;		 /* Number of retrievals from top of a list */
   unsigned long misses;	 /* Number of unsuccessful retrievals */
} *hsh_Stats;

extern hsh_HashTable hsh_create( unsigned long (*hash)( const void * ),
				 int (*compare)( const void *,
						 const void * ) );
extern void          hsh_destroy( hsh_HashTable table );
extern int           hsh_insert( hsh_HashTable table,
				 const void *key,
				 const void *datum );
extern int           hsh_delete( hsh_HashTable table, const void *key );
extern const void    *hsh_retrieve( hsh_HashTable table, const void *key );
extern int           hsh_iterate( hsh_HashTable table,
				  int (*iterator)( const void *key,
						   const void *datum ) );
extern int           hsh_iterate_arg( hsh_HashTable table,
				      int (*iterator)( const void *key,
						       const void *datum,
						       void *arg ),
				      void *arg );
extern hsh_Stats     hsh_get_stats( hsh_HashTable table );
extern void          hsh_print_stats( hsh_HashTable table, FILE *stream );
extern unsigned long hsh_string_hash( const void *key );
extern unsigned long hsh_pointer_hash( const void *key );
extern int           hsh_string_compare( const void *key1, const void *key2 );
extern int           hsh_pointer_compare( const void *key1, const void *key2 );
extern void          hsh_key_strings(hsh_HashTable);

extern hsh_Position  hsh_init_position( hsh_HashTable table );
extern hsh_Position  hsh_next_position( hsh_HashTable table,
					hsh_Position position );
extern void          *hsh_get_position( hsh_Position position, void **key );
extern int           hsh_readonly( hsh_HashTable table, int flag );

#define HSH_POSITION_INIT(P,T)  ((P)=hsh_init_position(T))
#define HSH_POSITION_NEXT(P,T)  ((P)=hsh_next_position(T,P))
#define HSH_POSITION_OK(P)      (P)
#define HSH_POSITION_GET(P,K,D) ((D)=hsh_get_position(P,&K))
#define HSH_POSITION_GETKEY(P,K) (hsh_get_position(P,&K))

/* iterate over all (key, datum) pairs, (K,D), in hash table T */
#define HSH_ITERATE(T,P,K,D)                                                 \
   for (HSH_POSITION_INIT((P),(T));                                          \
	HSH_POSITION_OK(P) && (HSH_POSITION_GET((P),(K),(D)),1);             \
	HSH_POSITION_NEXT((P),(T)))

/* iterate over all keys K, in hash table T */
#define HSH_ITERATE_KEYS(T,P,K)                                              \
   for (HSH_POSITION_INIT((P),(T));                                          \
	HSH_POSITION_OK(P) && (HSH_POSITION_GETKEY((P),(K)),1);              \
	HSH_POSITION_NEXT((P),(T)))

/* If the HSH_ITERATE loop is exited before all elements in the table are
   seen, then HSH_ITERATE_END should be called.  Calling this function
   after complete loops does no harm. */
#define HSH_ITERATE_END(T) hsh_readonly(T,0)

   
/* set.c */

typedef void *set_Set;
typedef void *set_Position;

typedef struct set_Stats{
   unsigned long size;		 /* Size of table */
   unsigned long resizings;	 /* Number of resizings */
   unsigned long entries;	 /* Total entries in table */
   unsigned long buckets_used;	 /* Number of hash buckets in use */
   unsigned long singletons;	 /* Number of length one lists */
   unsigned long maximum_length; /* Maximum list length */

   unsigned long retrievals;	 /* Total number of retrievals */
   unsigned long hits;		 /* Number of retrievals from top of a list */
   unsigned long misses;	 /* Number of unsuccessful retrievals */
} *set_Stats;

typedef unsigned long (*set_HashFunction)( const void * );
typedef int           (*set_CompareFunction)( const void *, const void * );

extern set_Set             set_create( set_HashFunction hash,
				       set_CompareFunction compare );
extern set_HashFunction    set_get_hash( set_Set set );
extern set_CompareFunction set_get_compare( set_Set set );
extern void                set_destroy( set_Set set );
extern int                 set_insert( set_Set set, const void *elem );
extern int                 set_delete( set_Set set, const void *elem );
extern int                 set_member( set_Set set, const void *elem );
extern int                 set_iterate( set_Set set,
					int (*iterator)( const void *key ) );
extern int                 set_iterate_arg( set_Set set,
					    int (*iterator)( const void *elem,
							     void *arg ),
					    void *arg );
extern set_Set             set_add( set_Set set1, set_Set set2 );
extern set_Set             set_del( set_Set set1, set_Set set2 );
extern set_Set             set_union( set_Set set1, set_Set set2 );
extern set_Set             set_inter( set_Set set1, set_Set set2 );
extern set_Set             set_diff( set_Set set1, set_Set set2 );
extern int                 set_equal( set_Set set1, set_Set set2 );
extern set_Stats           set_get_stats( set_Set set );
extern void                set_print_stats( set_Set set, FILE *stream );
extern int                 set_count( set_Set set );
extern set_Position        set_init_position( set_Set set );
extern set_Position        set_next_position( set_Set set,
					      set_Position position );
extern void                *set_get_position( set_Position position );
extern int                 set_readonly( set_Set set, int flag );

#define SET_POSITION_INIT(P,S) ((P)=set_init_position(S))
#define SET_POSITION_NEXT(P,S) ((P)=set_next_position(S,P))
#define SET_POSITION_OK(P)     (P)
#define SET_POSITION_GET(P,E)  ((E)=set_get_position(P))

/* iterate over all entries E in set S */
#define SET_ITERATE(S,P,E)                                                   \
   for (SET_POSITION_INIT((P),(S));                                          \
	SET_POSITION_OK(P) && (SET_POSITION_GET((P),(E)),1);                 \
	SET_POSITION_NEXT((P),(S)))

/* If the SET_ITERATE loop is exited before all elements in the set are
   seen, then SET_ITERATE_END should be called.  Calling this function
   after complete loops does no harm. */
#define SET_ITERATE_END(S) set_readonly(S,0)

/* stack.c */

typedef void *stk_Stack;

extern stk_Stack stk_create( void );
extern void      stk_destroy( stk_Stack stack );
extern void      stk_push( stk_Stack stack, void *datum );
extern void      *stk_pop( stk_Stack stack );
extern void      *stk_top( stk_Stack stack );

/* list.c */

typedef void *lst_List;
typedef void *lst_Position;

extern lst_List     lst_create( void );
extern void         lst_destroy( lst_List list );
extern void         lst_append( lst_List list, const void *datum );
extern void         lst_push( lst_List list, const void *datum );
extern void         *lst_pop( lst_List list );
extern void         *lst_top( lst_List list );
extern void         *lst_nth_get( lst_List list, unsigned int n );
extern void         lst_nth_set( lst_List list, unsigned int n,
				 const void *datum );
extern int          lst_member( lst_List list, const void *datum );
extern unsigned int lst_length( lst_List list );
extern int          lst_iterate( lst_List list,
				 int (*iterator)( const void *datum ) );
extern int          lst_iterate_arg( lst_List list,
				     int (*iterator)( const void *datum,
						      void *arg ),
				     void *arg );
extern void         lst_truncate( lst_List list, unsigned int length );
extern void         lst_truncate_position( lst_List list,
					   lst_Position position );
extern lst_Position lst_init_position( lst_List list );
extern lst_Position lst_last_position( lst_List list );
extern lst_Position lst_next_position( lst_Position position );
extern lst_Position lst_nth_position( lst_List list, unsigned int n );
extern void         *lst_get_position( lst_Position position );
extern void         lst_set_position( lst_Position position,
				      const void *datum );
extern void         lst_dump( lst_List list );
extern void         _lst_shutdown( void );
extern long int     lst_total_allocated( void );

#define LST_POSITION_INIT(P,L) ((P)=lst_init_position(L))
#define LST_POSITION_NEXT(P)   ((P)=lst_next_position(P))
#define LST_POSITION_OK(P)     (P)
#define LST_POSITION_GET(P,E)  ((E)=lst_get_position(P))

/* iterate over all entries E in list L */
#define LST_ITERATE(L,P,E)                                                  \
   for (LST_POSITION_INIT((P),(L));                                         \
        LST_POSITION_OK(P) && (LST_POSITION_GET((P),(E)),1);                \
        LST_POSITION_NEXT(P))

/* iterate over all entries in lists L1 and L2 */
#define LST_ITERATE2(L1,L2,P1,P2,E1,E2)                                      \
   for (LST_POSITION_INIT((P1),(L1)), LST_POSITION_INIT((P2),(L2));          \
	LST_POSITION_OK(P1) && LST_POSITION_OK(P2)                           \
	   && (LST_POSITION_GET((P1),(E1)),LST_POSITION_GET((P2),(E2)),1);   \
	LST_POSITION_NEXT(P1), LST_POSITION_NEXT(P2))

/* error.c */

extern void       err_set_program_name( const char *programName );
extern const char *err_program_name( void );
extern void       err_fatal( const char *routine, const char *format, ... )
   __attribute__((noreturn,format(printf, 2, 3)));
extern void       err_fatal_errno( const char *routine,
				   const char *format, ... )
   __attribute__((noreturn,format(printf, 2, 3)));

extern void       err_warning( const char *routine, const char *format, ... )
   __attribute__((format(printf, 2, 3)));

extern void       err_internal( const char *routine, const char *format, ... )
   __attribute__((noreturn,format(printf, 2, 3)));

/* memory.c */

typedef void *mem_String;
typedef void *mem_Object;

typedef struct mem_StringStats {
   int count;			/* Number of strings or objects */
   int bytes;			/* Number of bytes allocated */
} *mem_StringStats;

typedef struct mem_ObjectStats {
   int total;			/* Total objects requested */
   int used;			/* Total currently in use */
   int reused;			/* Total reused  */
   int size;			/* Size of each object */
} *mem_ObjectStats;

extern mem_String      mem_create_strings( void );
extern void            mem_destroy_strings( mem_String info );
extern const char      *mem_strcpy( mem_String info, const char *string );
extern const char      *mem_strncpy( mem_String info,
				     const char *string,
				     int len );
extern void            mem_grow( mem_String info,
				 const char *string,
				 int len );
extern const char      *mem_finish( mem_String info );
extern mem_StringStats mem_get_string_stats( mem_String info );
extern void            mem_print_string_stats( mem_String info, FILE *stream );

extern mem_Object      mem_create_objects( int size );
extern void            mem_destroy_objects( mem_Object info );
extern void            *mem_get_object( mem_Object info );
extern void            *mem_get_empty_object( mem_Object info );
extern void            mem_free_object( mem_Object info, void *obj );
extern mem_ObjectStats mem_get_object_stats( mem_Object info );
extern void            mem_print_object_stats( mem_Object info, FILE *stream );

/* string.c */

typedef void *str_Pool;
typedef hsh_Position str_Position;

typedef struct str_Stats {
   int count;			/* Number of strings created */
   int bytes;			/* Number of bytes allocated */
   int retrievals;		/* Total number of retrievals */
   int hits;			/* Number of retrievals from top of a list */
   int misses;			/* Number of unsuccessful retrievals */
} *str_Stats;

extern str_Pool   str_pool_create( void );
extern void       str_pool_destroy( str_Pool pool );
extern int        str_pool_exists( str_Pool pool, const char *s );
extern const char *str_pool_find( str_Pool pool, const char *s );
extern const char *str_pool_copy( str_Pool pool, const char *s );
extern const char *str_pool_copyn( str_Pool pool, const char *s, int length );
extern void       str_pool_grow( str_Pool pool, const char *s, int length );
extern const char *str_pool_finish( str_Pool pool );
extern str_Stats  str_pool_get_stats( str_Pool pool );
extern void       str_pool_print_stats( str_Pool pool, FILE *stream );

extern str_Position  str_pool_init_position(
   str_Pool table );
extern str_Position  str_pool_next_position(
   str_Pool table,
   str_Position position );
extern void str_pool_get_position(
   str_Position position,
   char const *const *key );
#define str_pool_readonly(pool, flag) hsh_readonly ((pool), (flag))

extern int        str_pool_iterate(
   str_Pool pool,
   int (*iterator)( const char *s ) );
extern int        str_pool_iterate_arg(
   str_Pool pool,
   int (*iterator)( const char *s, void *arg ),
   void *arg );

#define STR_POSITION_INIT(P,T)  ((P)=str_pool_init_position(T))
#define STR_POSITION_NEXT(P,T)  ((P)=str_pool_next_position(T,P))
#define STR_POSITION_OK(P)      (P)
#define STR_POSITION_GET(P,K)   (str_pool_get_position(P,&K), K)

/* iterate over all keys (K) in string pool T */
#define STR_ITERATE(T,P,K)                                                   \
   for (STR_POSITION_INIT((P),(T));                                          \
	STR_POSITION_OK(P) && (STR_POSITION_GET((P),(K)),1);                 \
	STR_POSITION_NEXT((P),(T)))

/* If the STR_ITERATE loop is exited before all elements in the table are
   seen, then STR_ITERATE_END should be called.  Calling this function
   after complete loops does no harm. */
#define STR_ITERATE_END(T) str_readonly(T,0)

extern int        str_exists( const char *s );
extern const char *str_find( const char *s );
extern const char *str_findn( const char *s, int length );
extern const char *str_copy( const char *s );
extern const char *str_copyn( const char *s, int length );
extern void       str_grow( const char *s, int length );
extern const char *str_finish( void );
extern const char *str_unique( const char *prefix );
extern void       str_destroy( void );
extern str_Stats  str_get_stats( void );
extern void       str_print_stats( FILE *stream );

/* debug.c */

typedef unsigned long int dbg_Type;

extern void           dbg_register( dbg_Type flag, const char *name );
extern void           _dbg_register( dbg_Type flag, const char *name );
extern void           dbg_destroy( void );
extern void           dbg_set( const char *name );
extern void           dbg_set_flag( dbg_Type flag );
extern void           dbg_unset_flag( dbg_Type flag );
extern int            dbg_test( dbg_Type flag );
extern void           dbg_list( FILE *stream );

#define PRINTF(flag,arg)  if (dbg_test(flag)) { log_info arg; }

/* flags.c */

typedef unsigned long int flg_Type;

extern void           flg_register( flg_Type flag, const char *name );
extern void           flg_destroy( void );
extern void           flg_set( const char *name );
extern int            flg_test( flg_Type flag );
extern void           flg_list( FILE *stream );
extern const          char *flg_name( flg_Type flag );

/* timer.c */

extern void   tim_start( const char *name );
extern void   tim_stop( const char *name );
extern void   tim_reset( const char *name );
extern double tim_get_real( const char *name );
extern double tim_get_user( const char *name );
extern double tim_get_system( const char *name );
extern void   tim_print_timer( FILE *str, const char *name );
extern void   tim_print_timers( FILE *str );
extern void   _tim_shutdown( void );

/* arg.c */

#define ARG_NO_ESCAPE 0x0001	/* Backslashed are not escape characters */
#define ARG_NO_QUOTE  0x0002	/* Don't use quote marks for quoting */

typedef void *arg_List;

extern arg_List   arg_create( void );
extern void       arg_destroy( arg_List arg );
extern arg_List   arg_add( arg_List arg, const char *string );
extern arg_List   arg_addn( arg_List arg, const char *string, int length );
extern void       arg_grow( arg_List arg, const char *string, int length );
extern arg_List   arg_finish( arg_List arg );
extern const char *arg_get( arg_List arg, int item );
extern int        arg_count( arg_List arg );
extern void       arg_get_vector( arg_List arg, int *argc, char ***argv );
extern arg_List   arg_argify( const char *string, int quoteStyle );

/* pr.c */

#define PR_USE_STDIN        0x00000001
#define PR_USE_STDOUT       0x00000002
#define PR_USE_STDERR       0x00000004
#define PR_CREATE_STDIN     0x00000010
#define PR_CREATE_STDOUT    0x00000020
#define PR_CREATE_STDERR    0x00000040
#define PR_STDERR_TO_STDOUT 0x00000100

extern int  pr_open( const char *command, int flags,
		     int *infd, int *outfd, int *errfd );
extern int  pr_close_nowait( int fd );
extern int  pr_close( int fd );
extern int  pr_wait( int pid );
extern void _pr_shutdown( void );
extern int  pr_spawn( const char *command );
int pr_readwrite( int in, int out,
		  const char *inBuffer, int inLen,
		  char *outBuffer, int outMaxLen );
int pr_filter( const char *command,
	       const char *inBuffer, int inLen,
	       char *outBuffer, int outMaxLen );

/* sl.c */

typedef void *sl_List;
typedef int (*sl_Iterator)( const void *datum );
typedef int (*sl_IteratorArg)( const void *datum, void *arg );

extern sl_List    sl_create( int (*compare)( const void *key1,
					     const void *key2 ),
			     const void *(*key)( const void *datum ),
			     const char *(*print)( const void *datum ) );
extern void       sl_destroy( sl_List list );
extern void       _sl_shutdown( void );
extern void       sl_insert( sl_List list, const void *datum );
extern void       sl_delete( sl_List list, const void *datum );
extern const void *sl_find( sl_List list, const void *key );
extern int        sl_iterate( sl_List list, sl_Iterator f );
extern int        sl_iterate_arg( sl_List list, sl_IteratorArg f, void *arg );
extern void       _sl_dump( sl_List list );

/* text.c */

extern const char * txt_soundex( const char *string );
extern void txt_soundex2(
   const char *string,
   char *result /* five chars */ );

/* base64.c */

extern const char    *b64_encode( unsigned long val );
extern unsigned long b64_decode( const char *val );
extern unsigned long b64_decode_buf (const char *val, size_t len);

/* base26.c */

extern const char    *b26_encode( unsigned long val );
extern unsigned long b26_decode( const char *val );

/* source.c */

typedef void *src_Type;

typedef struct src_Stats {
   int lines_used;		/* Lines used */
   int lines_allocated;		/* Lines allocated */
   int lines_bytes;		/* Bytes required to store lines */
   int tokens_total;		/* Tokens used */
   int tokens_reused;		/* Tokens reused */
   int tokens_size;		/* Size of a token information object */
} *src_Stats;

extern void       src_create( void );
extern void       src_destroy( void );
extern const char *src_line( const char *line, int len );
extern void       src_new_file( const char *filename );
extern void       src_new_line( int line );
extern void       src_advance( int length );
extern void       src_cpp_line( const char *line, int length );
extern src_Type   src_get( int length );
extern const char *src_filename( src_Type source );
extern int        src_linenumber( src_Type source );
extern int        src_offset( src_Type source );
extern int        src_length( src_Type source );
extern const char *src_source_line( src_Type source );
extern void       src_parse_error( FILE *stream, src_Type source,
				   const char *message );
extern void       src_print_error( FILE *stream, src_Type source,
				   const char *format, ... );
extern void       src_print_message( FILE *str, src_Type source,
                                     const char *format, ... );
extern void       src_print_line( FILE *stream, src_Type source );
extern src_Stats  src_get_stats( void );
extern void       src_print_stats( FILE *stream );

/* parse-concrete.c */

extern void   prs_register_concrete( const char *symbol,
				     const char *concrete );
extern const  char *prs_concrete( const char *symbol );
extern void   _prs_shutdown( void );

/* log.c */
#define LOG_OPTION_FULL    1
#define LOG_OPTION_NO_FULL 2

extern void       log_syslog( const char *ident );
extern void       log_set_facility( const char *facility );
extern const char *log_get_facility( void );
extern void       log_option( int option );
extern void       log_file( const char *ident, const char *filename );
extern void       log_stream( const char *ident, FILE *stream );
extern void       log_close( void );
extern void       log_error_va( const char *routine,
                                const char *format, va_list ap );
extern void       log_error( const char *routine, const char *format, ... );
extern void       log_info_va( const char *format, va_list ap );
extern void       log_info( const char *format, ... );

#endif
