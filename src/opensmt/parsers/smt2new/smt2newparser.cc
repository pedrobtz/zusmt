/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 1

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1


/* Substitute the variable and function names.  */
#define yyparse         osmt_yyparse
#define yylex           osmt_yylex
#define yyerror         osmt_yyerror
#define yydebug         osmt_yydebug
#define yynerrs         osmt_yynerrs

/* First part of user prologue.  */
#line 36 "smt2newparser.yy"

#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <vector>
#include <list>
#include <string>
#include <cstring>

#include <api/smt2tokens.h>
#include <parsers/smt2new/smt2newcontext.h>

// TK: I could not arrive at a solution without preceding this before including the gen. header
using namespace opensmt;

#include "smt2newparser.hh"


int osmt_yylex(YYSTYPE* lvalp, YYLTYPE* llocp, void* scanner);

void osmt_yyerror( YYLTYPE* locp, Smt2newContext* context, const char * s )
{
  if (context->interactive)
    printf("At interactive input: %s\n", s);
  else
    printf( "At line %d: %s\n", locp->first_line, s );
//  exit( 1 );
}

#define scanner context->scanner

/* Overallocation to prevent stack overflow */
#define YYMAXDEPTH 1024 * 1024

#line 111 "smt2newparser.cc"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "smt2newparser.hh"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_TK_AS = 3,                      /* TK_AS  */
  YYSYMBOL_TK_DECIMAL = 4,                 /* TK_DECIMAL  */
  YYSYMBOL_TK_EXISTS = 5,                  /* TK_EXISTS  */
  YYSYMBOL_TK_FORALL = 6,                  /* TK_FORALL  */
  YYSYMBOL_TK_LET = 7,                     /* TK_LET  */
  YYSYMBOL_TK_NUMERAL = 8,                 /* TK_NUMERAL  */
  YYSYMBOL_TK_PAR = 9,                     /* TK_PAR  */
  YYSYMBOL_TK_STRING = 10,                 /* TK_STRING  */
  YYSYMBOL_TK_ASSERT = 11,                 /* TK_ASSERT  */
  YYSYMBOL_TK_CHECKSAT = 12,               /* TK_CHECKSAT  */
  YYSYMBOL_TK_DECLARESORT = 13,            /* TK_DECLARESORT  */
  YYSYMBOL_TK_DECLAREFUN = 14,             /* TK_DECLAREFUN  */
  YYSYMBOL_TK_DECLARECONST = 15,           /* TK_DECLARECONST  */
  YYSYMBOL_TK_DEFINESORT = 16,             /* TK_DEFINESORT  */
  YYSYMBOL_TK_DEFINEFUN = 17,              /* TK_DEFINEFUN  */
  YYSYMBOL_TK_EXIT = 18,                   /* TK_EXIT  */
  YYSYMBOL_TK_GETASSERTIONS = 19,          /* TK_GETASSERTIONS  */
  YYSYMBOL_TK_GETASSIGNMENT = 20,          /* TK_GETASSIGNMENT  */
  YYSYMBOL_TK_GETINFO = 21,                /* TK_GETINFO  */
  YYSYMBOL_TK_GETOPTION = 22,              /* TK_GETOPTION  */
  YYSYMBOL_TK_GETPROOF = 23,               /* TK_GETPROOF  */
  YYSYMBOL_TK_GETUNSATCORE = 24,           /* TK_GETUNSATCORE  */
  YYSYMBOL_TK_GETVALUE = 25,               /* TK_GETVALUE  */
  YYSYMBOL_TK_GETMODEL = 26,               /* TK_GETMODEL  */
  YYSYMBOL_TK_POP = 27,                    /* TK_POP  */
  YYSYMBOL_TK_PUSH = 28,                   /* TK_PUSH  */
  YYSYMBOL_TK_SETLOGIC = 29,               /* TK_SETLOGIC  */
  YYSYMBOL_TK_SETINFO = 30,                /* TK_SETINFO  */
  YYSYMBOL_TK_SETOPTION = 31,              /* TK_SETOPTION  */
  YYSYMBOL_TK_THEORY = 32,                 /* TK_THEORY  */
  YYSYMBOL_TK_GETITPS = 33,                /* TK_GETITPS  */
  YYSYMBOL_TK_WRSTATE = 34,                /* TK_WRSTATE  */
  YYSYMBOL_TK_RDSTATE = 35,                /* TK_RDSTATE  */
  YYSYMBOL_TK_SIMPLIFY = 36,               /* TK_SIMPLIFY  */
  YYSYMBOL_TK_WRFUNS = 37,                 /* TK_WRFUNS  */
  YYSYMBOL_TK_ECHO = 38,                   /* TK_ECHO  */
  YYSYMBOL_TK_NUM = 39,                    /* TK_NUM  */
  YYSYMBOL_TK_SYM = 40,                    /* TK_SYM  */
  YYSYMBOL_TK_QSYM = 41,                   /* TK_QSYM  */
  YYSYMBOL_TK_KEY = 42,                    /* TK_KEY  */
  YYSYMBOL_TK_STR = 43,                    /* TK_STR  */
  YYSYMBOL_TK_DEC = 44,                    /* TK_DEC  */
  YYSYMBOL_TK_HEX = 45,                    /* TK_HEX  */
  YYSYMBOL_TK_BIN = 46,                    /* TK_BIN  */
  YYSYMBOL_KW_SORTS = 47,                  /* KW_SORTS  */
  YYSYMBOL_KW_FUNS = 48,                   /* KW_FUNS  */
  YYSYMBOL_KW_SORTSDESCRIPTION = 49,       /* KW_SORTSDESCRIPTION  */
  YYSYMBOL_KW_FUNSDESCRIPTION = 50,        /* KW_FUNSDESCRIPTION  */
  YYSYMBOL_KW_DEFINITION = 51,             /* KW_DEFINITION  */
  YYSYMBOL_KW_NOTES = 52,                  /* KW_NOTES  */
  YYSYMBOL_KW_THEORIES = 53,               /* KW_THEORIES  */
  YYSYMBOL_KW_EXTENSIONS = 54,             /* KW_EXTENSIONS  */
  YYSYMBOL_KW_VALUES = 55,                 /* KW_VALUES  */
  YYSYMBOL_KW_PRINTSUCCESS = 56,           /* KW_PRINTSUCCESS  */
  YYSYMBOL_KW_EXPANDDEFINITIONS = 57,      /* KW_EXPANDDEFINITIONS  */
  YYSYMBOL_KW_INTERACTIVEMODE = 58,        /* KW_INTERACTIVEMODE  */
  YYSYMBOL_KW_PRODUCEPROOFS = 59,          /* KW_PRODUCEPROOFS  */
  YYSYMBOL_KW_PRODUCEUNSATCORES = 60,      /* KW_PRODUCEUNSATCORES  */
  YYSYMBOL_KW_MINIMALUNSATCORES = 61,      /* KW_MINIMALUNSATCORES  */
  YYSYMBOL_KW_PRINTCORESFULL = 62,         /* KW_PRINTCORESFULL  */
  YYSYMBOL_KW_PRODUCEMODELS = 63,          /* KW_PRODUCEMODELS  */
  YYSYMBOL_KW_PRODUCEASSIGNMENTS = 64,     /* KW_PRODUCEASSIGNMENTS  */
  YYSYMBOL_KW_REGULAROUTPUTCHANNEL = 65,   /* KW_REGULAROUTPUTCHANNEL  */
  YYSYMBOL_KW_DIAGNOSTICOUTPUTCHANNEL = 66, /* KW_DIAGNOSTICOUTPUTCHANNEL  */
  YYSYMBOL_KW_RANDOMSEED = 67,             /* KW_RANDOMSEED  */
  YYSYMBOL_KW_VERBOSITY = 68,              /* KW_VERBOSITY  */
  YYSYMBOL_KW_ERRORBEHAVIOR = 69,          /* KW_ERRORBEHAVIOR  */
  YYSYMBOL_KW_NAME = 70,                   /* KW_NAME  */
  YYSYMBOL_KW_NAMED = 71,                  /* KW_NAMED  */
  YYSYMBOL_KW_AUTHORS = 72,                /* KW_AUTHORS  */
  YYSYMBOL_KW_VERSION = 73,                /* KW_VERSION  */
  YYSYMBOL_KW_STATUS = 74,                 /* KW_STATUS  */
  YYSYMBOL_KW_REASONUNKNOWN = 75,          /* KW_REASONUNKNOWN  */
  YYSYMBOL_KW_ALLSTATISTICS = 76,          /* KW_ALLSTATISTICS  */
  YYSYMBOL_77_ = 77,                       /* '('  */
  YYSYMBOL_78_ = 78,                       /* ')'  */
  YYSYMBOL_79___ = 79,                     /* '_'  */
  YYSYMBOL_80_ = 80,                       /* '!'  */
  YYSYMBOL_YYACCEPT = 81,                  /* $accept  */
  YYSYMBOL_script = 82,                    /* script  */
  YYSYMBOL_symbol = 83,                    /* symbol  */
  YYSYMBOL_command_list = 84,              /* command_list  */
  YYSYMBOL_command = 85,                   /* command  */
  YYSYMBOL_attribute_list = 86,            /* attribute_list  */
  YYSYMBOL_attribute = 87,                 /* attribute  */
  YYSYMBOL_attribute_value = 88,           /* attribute_value  */
  YYSYMBOL_identifier = 89,                /* identifier  */
  YYSYMBOL_sort = 90,                      /* sort  */
  YYSYMBOL_sort_list = 91,                 /* sort_list  */
  YYSYMBOL_s_expr = 92,                    /* s_expr  */
  YYSYMBOL_s_expr_list = 93,               /* s_expr_list  */
  YYSYMBOL_spec_const = 94,                /* spec_const  */
  YYSYMBOL_const_val = 95,                 /* const_val  */
  YYSYMBOL_numeral_list = 96,              /* numeral_list  */
  YYSYMBOL_qual_identifier = 97,           /* qual_identifier  */
  YYSYMBOL_var_binding_list = 98,          /* var_binding_list  */
  YYSYMBOL_var_binding = 99,               /* var_binding  */
  YYSYMBOL_sorted_var_list = 100,          /* sorted_var_list  */
  YYSYMBOL_sorted_var = 101,               /* sorted_var  */
  YYSYMBOL_term_list = 102,                /* term_list  */
  YYSYMBOL_term = 103,                     /* term  */
  YYSYMBOL_symbol_list = 104,              /* symbol_list  */
  YYSYMBOL_b_value = 105,                  /* b_value  */
  YYSYMBOL_option = 106,                   /* option  */
  YYSYMBOL_predef_key = 107,               /* predef_key  */
  YYSYMBOL_info_flag = 108                 /* info_flag  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_int16 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if 1

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* 1 */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
             && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
  YYLTYPE yyls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE) \
             + YYSIZEOF (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   408

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  81
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  28
/* YYNRULES -- Number of rules.  */
#define YYNRULES  136
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  257

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   331


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    80,     2,     2,     2,     2,     2,     2,
      77,    78,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,    79,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   102,   102,   104,   106,   111,   112,   116,   122,   128,
     134,   141,   154,   166,   178,   191,   197,   203,   209,   213,
     217,   221,   226,   232,   238,   244,   248,   254,   258,   262,
     268,   274,   280,   284,   286,   295,   296,   300,   302,   304,
     306,   310,   312,   316,   323,   325,   329,   331,   340,   343,
     346,   352,   356,   360,   368,   371,   379,   381,   383,   385,
     387,   391,   393,   397,   399,   403,   405,   415,   416,   420,
     425,   426,   430,   434,   435,   439,   441,   443,   450,   460,
     470,   480,   493,   494,   498,   512,   518,   524,   530,   536,
     542,   548,   554,   560,   566,   572,   578,   584,   590,   598,
     600,   602,   604,   606,   608,   610,   612,   614,   616,   618,
     620,   622,   624,   626,   628,   630,   632,   634,   636,   638,
     640,   642,   644,   646,   648,   650,   652,   654,   656,   660,
     662,   664,   666,   668,   670,   672,   674
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if 1
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "TK_AS", "TK_DECIMAL",
  "TK_EXISTS", "TK_FORALL", "TK_LET", "TK_NUMERAL", "TK_PAR", "TK_STRING",
  "TK_ASSERT", "TK_CHECKSAT", "TK_DECLARESORT", "TK_DECLAREFUN",
  "TK_DECLARECONST", "TK_DEFINESORT", "TK_DEFINEFUN", "TK_EXIT",
  "TK_GETASSERTIONS", "TK_GETASSIGNMENT", "TK_GETINFO", "TK_GETOPTION",
  "TK_GETPROOF", "TK_GETUNSATCORE", "TK_GETVALUE", "TK_GETMODEL", "TK_POP",
  "TK_PUSH", "TK_SETLOGIC", "TK_SETINFO", "TK_SETOPTION", "TK_THEORY",
  "TK_GETITPS", "TK_WRSTATE", "TK_RDSTATE", "TK_SIMPLIFY", "TK_WRFUNS",
  "TK_ECHO", "TK_NUM", "TK_SYM", "TK_QSYM", "TK_KEY", "TK_STR", "TK_DEC",
  "TK_HEX", "TK_BIN", "KW_SORTS", "KW_FUNS", "KW_SORTSDESCRIPTION",
  "KW_FUNSDESCRIPTION", "KW_DEFINITION", "KW_NOTES", "KW_THEORIES",
  "KW_EXTENSIONS", "KW_VALUES", "KW_PRINTSUCCESS", "KW_EXPANDDEFINITIONS",
  "KW_INTERACTIVEMODE", "KW_PRODUCEPROOFS", "KW_PRODUCEUNSATCORES",
  "KW_MINIMALUNSATCORES", "KW_PRINTCORESFULL", "KW_PRODUCEMODELS",
  "KW_PRODUCEASSIGNMENTS", "KW_REGULAROUTPUTCHANNEL",
  "KW_DIAGNOSTICOUTPUTCHANNEL", "KW_RANDOMSEED", "KW_VERBOSITY",
  "KW_ERRORBEHAVIOR", "KW_NAME", "KW_NAMED", "KW_AUTHORS", "KW_VERSION",
  "KW_STATUS", "KW_REASONUNKNOWN", "KW_ALLSTATISTICS", "'('", "')'", "'_'",
  "'!'", "$accept", "script", "symbol", "command_list", "command",
  "attribute_list", "attribute", "attribute_value", "identifier", "sort",
  "sort_list", "s_expr", "s_expr_list", "spec_const", "const_val",
  "numeral_list", "qual_identifier", "var_binding_list", "var_binding",
  "sorted_var_list", "sorted_var", "term_list", "term", "symbol_list",
  "b_value", "option", "predef_key", "info_flag", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-109)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    -109,     8,   -63,  -109,   364,  -109,    94,   -17,   -29,   -29,
     169,   -29,   -29,   -12,    -4,    -1,   121,   228,     1,     4,
      12,    19,    32,    52,   -29,   263,   298,  -109,    68,    70,
      50,    98,   100,  -109,  -109,  -109,  -109,  -109,  -109,  -109,
      13,  -109,  -109,  -109,  -109,    72,  -109,    86,    77,  -109,
    -109,    22,    78,    79,  -109,  -109,  -109,  -109,  -109,  -109,
    -109,  -109,  -109,  -109,  -109,    80,    84,  -109,  -109,  -109,
    -109,  -109,  -109,  -109,  -109,  -109,  -109,  -109,  -109,  -109,
    -109,  -109,  -109,  -109,  -109,  -109,  -109,  -109,  -109,  -109,
    -109,  -109,  -109,  -109,  -109,  -109,  -109,   103,  -109,  -109,
      94,  -109,   105,   107,   110,   134,   120,   134,   -29,   -29,
     -29,   -29,   -29,   -29,   -29,   -29,   -29,   114,   157,   162,
     163,  -109,   125,    -2,   126,   127,  -109,   128,   138,    40,
     151,   152,   155,     7,   -29,    94,    94,  -109,   156,  -109,
      47,  -109,   158,  -109,  -109,  -109,  -109,  -109,  -109,  -109,
    -109,  -109,  -109,  -109,  -109,  -109,  -109,  -109,  -109,  -109,
    -109,  -109,  -109,  -109,  -109,  -109,  -109,  -109,  -109,  -109,
    -109,  -109,  -109,  -109,  -109,  -109,  -109,  -109,  -109,   159,
      22,   160,   160,   195,   234,   263,  -109,  -109,    82,    22,
    -109,    17,   -22,     6,   -18,   196,   -29,  -109,  -109,   -29,
    -109,  -109,   -30,  -109,    75,    22,  -109,  -109,    22,  -109,
      22,  -109,   229,  -109,  -109,  -109,  -109,  -109,  -109,  -109,
      22,   -13,    -8,    94,    -5,  -109,  -109,   193,  -109,   230,
      91,   231,    94,  -109,   -10,   264,    94,    94,   265,    94,
    -109,  -109,  -109,  -109,  -109,  -109,   266,  -109,  -109,   318,
     325,  -109,   326,  -109,  -109,  -109,  -109
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       5,     0,     2,     1,     0,     6,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    73,     0,     0,
       0,     0,     0,    56,     3,     4,    60,    57,    58,    59,
       0,    44,    65,    75,    76,     0,    18,     0,     0,    61,
      62,     0,     0,     0,    33,    19,    28,   136,   129,   130,
     131,   132,   133,   134,   135,     0,     0,    99,   100,   101,
     102,   103,   105,   106,   107,   104,   108,   109,   110,   111,
     112,   113,   114,   115,   116,   117,   118,   119,   120,   121,
     122,   123,   124,   125,   126,   127,   128,     0,    20,    25,
       0,    27,     0,     0,     0,    37,     0,    39,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   117,   118,   119,
     120,    98,     0,     0,     0,     0,    32,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    17,     0,    49,
       0,    46,     0,    82,    70,    31,    29,    30,    73,    16,
      15,     7,    54,    42,    38,    41,     9,    40,    84,    85,
      86,    87,    88,    89,    90,    91,    92,    93,    94,    95,
      96,    97,     8,    21,    74,    22,    23,    24,    34,     0,
       0,     0,     0,     0,     0,     0,    73,    10,     0,     0,
      13,     0,     0,     0,     0,     0,     0,    70,    70,     0,
      67,    64,     0,    35,     0,     0,    48,    49,     0,    83,
       0,    71,     0,    52,    54,    43,    51,    55,    50,    66,
       0,     0,     0,     0,     0,    63,    45,     0,    77,     0,
       0,     0,     0,    26,     0,     0,     0,     0,     0,     0,
      68,    81,    36,    12,    47,    11,     0,    53,    72,     0,
       0,    69,     0,    14,    80,    79,    78
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -109,  -109,    -7,  -109,  -109,  -109,   -20,   199,   -44,   -38,
     198,  -109,   192,     5,  -109,  -109,   301,  -109,   183,   -51,
     -33,  -108,    -6,  -109,   109,  -109,   391,  -109
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_uint8 yydefgoto[] =
{
       0,     1,    41,     2,     5,   227,   106,   154,    42,   206,
     188,   217,   194,    43,    51,   202,    44,   224,   200,   192,
     211,   123,   174,   191,   159,   122,   107,    65
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      45,    47,    48,    49,    52,    53,   121,   141,     3,   225,
     129,    34,    35,   142,     4,    50,   129,   104,   130,   131,
     132,    33,    34,    35,   213,    36,    37,    38,    39,    33,
      34,    35,   213,    36,    37,    38,    39,    33,    34,    35,
     193,    36,    37,    38,    39,    33,    34,    35,   226,    36,
      37,    38,    39,    34,    35,   196,   210,    34,    35,   214,
     215,    46,    34,    35,   196,   236,    54,   214,   247,   196,
     237,   102,   199,   239,    55,    40,   173,    56,   204,    98,
      34,    35,    99,    40,   212,   180,   134,    34,    35,   100,
     133,   103,   134,   135,   148,   208,   189,   101,   153,   140,
     153,   158,   158,   158,   158,   158,   158,   158,   158,   158,
     155,   124,   155,   125,    33,    34,    35,   179,    36,    37,
      38,    39,    34,    35,   179,   138,   134,   184,   126,   185,
     186,    34,    35,    33,    34,    35,   141,    36,    37,    38,
      39,   127,   195,   128,   141,   141,   221,   222,   197,   198,
     137,   207,    40,   228,   139,   143,   144,   168,   145,   140,
     205,   141,   146,    57,   141,   203,   141,   229,   140,   244,
     231,    40,   232,    33,    34,    35,   141,    36,    37,    38,
      39,   147,   235,   149,   209,   150,   141,   216,   151,   220,
      58,    59,   223,    60,    61,    62,    63,    64,   156,   218,
     169,   170,   171,   172,   175,   176,   177,   242,    33,    34,
      35,   152,    36,    37,    38,    39,   178,   238,   160,   161,
     162,   163,   164,   165,   166,   167,   246,   216,   181,   182,
     249,   250,   183,   252,   187,   105,   190,   196,   134,   218,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    81,    82,    83,    84,    85,    86,
      87,    88,    89,    90,    91,    92,    93,    94,    95,    96,
      66,   241,   199,   201,   219,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    90,    91,
      92,    93,    94,    95,    96,   105,   157,   233,   243,   245,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    81,    82,    83,    84,    85,    86,
      87,    88,    89,    90,    91,    92,    93,    94,    95,    96,
     105,   136,   248,   251,   253,    67,    68,    69,    70,    71,
      72,    73,    74,    75,   108,   109,   110,   111,   112,   113,
     114,   115,   116,   117,   118,   119,   120,    89,    90,    91,
      92,    93,    94,    95,    96,     6,     7,     8,     9,    10,
      11,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    25,    26,   254,    27,    28,    29,
      30,    31,    32,   255,   256,   230,   234,   240,    97
};

static const yytype_uint8 yycheck[] =
{
       6,     8,     9,    10,    11,    12,    26,    51,     0,    39,
       3,    40,    41,    51,    77,    10,     3,    24,     5,     6,
       7,    39,    40,    41,    42,    43,    44,    45,    46,    39,
      40,    41,    42,    43,    44,    45,    46,    39,    40,    41,
     148,    43,    44,    45,    46,    39,    40,    41,    78,    43,
      44,    45,    46,    40,    41,    77,    78,    40,    41,    77,
      78,    78,    40,    41,    77,    78,    78,    77,    78,    77,
      78,    39,    77,    78,    78,    77,    78,    78,   186,    78,
      40,    41,    78,    77,    78,   129,    79,    40,    41,    77,
      77,    39,    79,    80,   100,    78,   140,    78,   105,    77,
     107,   108,   109,   110,   111,   112,   113,   114,   115,   116,
     105,    43,   107,    43,    39,    40,    41,    77,    43,    44,
      45,    46,    40,    41,    77,    39,    79,   134,    78,   135,
     136,    40,    41,    39,    40,    41,   180,    43,    44,    45,
      46,    43,   180,    43,   188,   189,   197,   198,   181,   182,
      78,   189,    77,    78,    77,    77,    77,    43,    78,    77,
      78,   205,    78,    42,   208,   185,   210,   205,    77,    78,
     208,    77,   210,    39,    40,    41,   220,    43,    44,    45,
      46,    78,   220,    78,   191,    78,   230,   194,    78,   196,
      69,    70,   199,    72,    73,    74,    75,    76,    78,   194,
      43,    39,    39,    78,    78,    78,    78,   227,    39,    40,
      41,    77,    43,    44,    45,    46,    78,   223,   109,   110,
     111,   112,   113,   114,   115,   116,   232,   234,    77,    77,
     236,   237,    77,   239,    78,    42,    78,    77,    79,   234,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      42,    78,    77,    39,    78,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    42,   107,    78,    78,    78,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      42,    40,    78,    78,    78,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    78,    33,    34,    35,
      36,    37,    38,    78,    78,   207,   214,   224,    17
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,    82,    84,     0,    77,    85,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    43,    44,    45,    46,
      77,    83,    89,    94,    97,   103,    78,    83,    83,    83,
      94,    95,    83,    83,    78,    78,    78,    42,    69,    70,
      72,    73,    74,    75,    76,   108,    42,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    73,    74,    75,    76,   107,    78,    78,
      77,    78,    39,    39,    83,    42,    87,   107,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    66,    67,
      68,    87,   106,   102,    43,    43,    78,    43,    43,     3,
       5,     6,     7,    77,    79,    80,    97,    78,    39,    77,
      77,    89,    90,    77,    77,    78,    78,    78,   103,    78,
      78,    78,    77,    83,    88,    94,    78,    88,    83,   105,
     105,   105,   105,   105,   105,   105,   105,   105,    43,    43,
      39,    39,    78,    78,   103,    78,    78,    78,    78,    77,
      89,    77,    77,    77,    83,   103,   103,    78,    91,    89,
      78,   104,   100,   102,    93,    90,    77,   101,   101,    77,
      99,    39,    96,    87,   102,    78,    90,    90,    78,    83,
      78,   101,    78,    42,    77,    78,    83,    92,    94,    78,
      83,   100,   100,    83,    98,    39,    78,    86,    78,    90,
      91,    90,    90,    78,    93,    90,    78,    78,   103,    78,
      99,    78,    87,    78,    78,    78,   103,    78,    78,   103,
     103,    78,   103,    78,    78,    78,    78
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    81,    82,    83,    83,    84,    84,    85,    85,    85,
      85,    85,    85,    85,    85,    85,    85,    85,    85,    85,
      85,    85,    85,    85,    85,    85,    85,    85,    85,    85,
      85,    85,    85,    85,    85,    86,    86,    87,    87,    87,
      87,    88,    88,    88,    89,    89,    90,    90,    91,    91,
      92,    92,    92,    92,    93,    93,    94,    94,    94,    94,
      94,    95,    95,    96,    96,    97,    97,    98,    98,    99,
     100,   100,   101,   102,   102,   103,   103,   103,   103,   103,
     103,   103,   104,   104,   105,   106,   106,   106,   106,   106,
     106,   106,   106,   106,   106,   106,   106,   106,   106,   107,
     107,   107,   107,   107,   107,   107,   107,   107,   107,   107,
     107,   107,   107,   107,   107,   107,   107,   107,   107,   107,
     107,   107,   107,   107,   107,   107,   107,   107,   107,   108,
     108,   108,   108,   108,   108,   108,   108
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     1,     1,     0,     2,     4,     4,     4,
       5,     8,     8,     5,     9,     4,     4,     4,     3,     3,
       3,     4,     4,     4,     4,     3,     7,     3,     3,     4,
       4,     4,     3,     3,     4,     0,     2,     1,     2,     1,
       2,     1,     1,     3,     1,     5,     1,     5,     2,     0,
       1,     1,     1,     3,     0,     2,     1,     1,     1,     1,
       1,     1,     1,     2,     1,     1,     5,     0,     2,     4,
       0,     2,     4,     0,     2,     1,     1,     5,     8,     8,
       8,     6,     0,     2,     1,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (&yylloc, context, YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF

/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)                                \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;        \
          (Current).first_column = YYRHSLOC (Rhs, 1).first_column;      \
          (Current).last_line    = YYRHSLOC (Rhs, N).last_line;         \
          (Current).last_column  = YYRHSLOC (Rhs, N).last_column;       \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).first_line   = (Current).last_line   =              \
            YYRHSLOC (Rhs, 0).last_line;                                \
          (Current).first_column = (Current).last_column =              \
            YYRHSLOC (Rhs, 0).last_column;                              \
        }                                                               \
    while (0)
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K])


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)


/* YYLOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

# ifndef YYLOCATION_PRINT

#  if defined YY_LOCATION_PRINT

   /* Temporary convenience wrapper in case some people defined the
      undocumented and private YY_LOCATION_PRINT macros.  */
#   define YYLOCATION_PRINT(File, Loc)  YY_LOCATION_PRINT(File, *(Loc))

#  elif defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL

/* Print *YYLOCP on YYO.  Private, do not rely on its existence. */

YY_ATTRIBUTE_UNUSED
static int
yy_location_print_ (FILE *yyo, YYLTYPE const * const yylocp)
{
  int res = 0;
  int end_col = 0 != yylocp->last_column ? yylocp->last_column - 1 : 0;
  if (0 <= yylocp->first_line)
    {
      res += YYFPRINTF (yyo, "%d", yylocp->first_line);
      if (0 <= yylocp->first_column)
        res += YYFPRINTF (yyo, ".%d", yylocp->first_column);
    }
  if (0 <= yylocp->last_line)
    {
      if (yylocp->first_line < yylocp->last_line)
        {
          res += YYFPRINTF (yyo, "-%d", yylocp->last_line);
          if (0 <= end_col)
            res += YYFPRINTF (yyo, ".%d", end_col);
        }
      else if (0 <= end_col && yylocp->first_column < end_col)
        res += YYFPRINTF (yyo, "-%d", end_col);
    }
  return res;
}

#   define YYLOCATION_PRINT  yy_location_print_

    /* Temporary convenience wrapper in case some people defined the
       undocumented and private YY_LOCATION_PRINT macros.  */
#   define YY_LOCATION_PRINT(File, Loc)  YYLOCATION_PRINT(File, &(Loc))

#  else

#   define YYLOCATION_PRINT(File, Loc) ((void) 0)
    /* Temporary convenience wrapper in case some people defined the
       undocumented and private YY_LOCATION_PRINT macros.  */
#   define YY_LOCATION_PRINT  YYLOCATION_PRINT

#  endif
# endif /* !defined YYLOCATION_PRINT */


# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value, Location, context); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, Smt2newContext* context)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  YY_USE (yylocationp);
  YY_USE (context);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, Smt2newContext* context)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  YYLOCATION_PRINT (yyo, yylocationp);
  YYFPRINTF (yyo, ": ");
  yy_symbol_value_print (yyo, yykind, yyvaluep, yylocationp, context);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp, YYLTYPE *yylsp,
                 int yyrule, Smt2newContext* context)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)],
                       &(yylsp[(yyi + 1) - (yynrhs)]), context);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, yylsp, Rule, context); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


/* Context of a parse error.  */
typedef struct
{
  yy_state_t *yyssp;
  yysymbol_kind_t yytoken;
  YYLTYPE *yylloc;
} yypcontext_t;

/* Put in YYARG at most YYARGN of the expected tokens given the
   current YYCTX, and return the number of tokens stored in YYARG.  If
   YYARG is null, return the number of expected tokens (guaranteed to
   be less than YYNTOKENS).  Return YYENOMEM on memory exhaustion.
   Return 0 if there are more than YYARGN expected tokens, yet fill
   YYARG up to YYARGN. */
static int
yypcontext_expected_tokens (const yypcontext_t *yyctx,
                            yysymbol_kind_t yyarg[], int yyargn)
{
  /* Actual size of YYARG. */
  int yycount = 0;
  int yyn = yypact[+*yyctx->yyssp];
  if (!yypact_value_is_default (yyn))
    {
      /* Start YYX at -YYN if negative to avoid negative indexes in
         YYCHECK.  In other words, skip the first -YYN actions for
         this state because they are default actions.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;
      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yyx;
      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
        if (yycheck[yyx + yyn] == yyx && yyx != YYSYMBOL_YYerror
            && !yytable_value_is_error (yytable[yyx + yyn]))
          {
            if (!yyarg)
              ++yycount;
            else if (yycount == yyargn)
              return 0;
            else
              yyarg[yycount++] = YY_CAST (yysymbol_kind_t, yyx);
          }
    }
  if (yyarg && yycount == 0 && 0 < yyargn)
    yyarg[0] = YYSYMBOL_YYEMPTY;
  return yycount;
}




#ifndef yystrlen
# if defined __GLIBC__ && defined _STRING_H
#  define yystrlen(S) (YY_CAST (YYPTRDIFF_T, strlen (S)))
# else
/* Return the length of YYSTR.  */
static YYPTRDIFF_T
yystrlen (const char *yystr)
{
  YYPTRDIFF_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
# endif
#endif

#ifndef yystpcpy
# if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#  define yystpcpy stpcpy
# else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
# endif
#endif

#ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYPTRDIFF_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYPTRDIFF_T yyn = 0;
      char const *yyp = yystr;
      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            else
              goto append;

          append:
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (yyres)
    return yystpcpy (yyres, yystr) - yyres;
  else
    return yystrlen (yystr);
}
#endif


static int
yy_syntax_error_arguments (const yypcontext_t *yyctx,
                           yysymbol_kind_t yyarg[], int yyargn)
{
  /* Actual size of YYARG. */
  int yycount = 0;
  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yyctx->yytoken != YYSYMBOL_YYEMPTY)
    {
      int yyn;
      if (yyarg)
        yyarg[yycount] = yyctx->yytoken;
      ++yycount;
      yyn = yypcontext_expected_tokens (yyctx,
                                        yyarg ? yyarg + 1 : yyarg, yyargn - 1);
      if (yyn == YYENOMEM)
        return YYENOMEM;
      else
        yycount += yyn;
    }
  return yycount;
}

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return -1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return YYENOMEM if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYPTRDIFF_T *yymsg_alloc, char **yymsg,
                const yypcontext_t *yyctx)
{
  enum { YYARGS_MAX = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat: reported tokens (one for the "unexpected",
     one per "expected"). */
  yysymbol_kind_t yyarg[YYARGS_MAX];
  /* Cumulated lengths of YYARG.  */
  YYPTRDIFF_T yysize = 0;

  /* Actual size of YYARG. */
  int yycount = yy_syntax_error_arguments (yyctx, yyarg, YYARGS_MAX);
  if (yycount == YYENOMEM)
    return YYENOMEM;

  switch (yycount)
    {
#define YYCASE_(N, S)                       \
      case N:                               \
        yyformat = S;                       \
        break
    default: /* Avoid compiler warnings. */
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
#undef YYCASE_
    }

  /* Compute error message size.  Don't count the "%s"s, but reserve
     room for the terminator.  */
  yysize = yystrlen (yyformat) - 2 * yycount + 1;
  {
    int yyi;
    for (yyi = 0; yyi < yycount; ++yyi)
      {
        YYPTRDIFF_T yysize1
          = yysize + yytnamerr (YY_NULLPTR, yytname[yyarg[yyi]]);
        if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
          yysize = yysize1;
        else
          return YYENOMEM;
      }
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return -1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yytname[yyarg[yyi++]]);
          yyformat += 2;
        }
      else
        {
          ++yyp;
          ++yyformat;
        }
  }
  return 0;
}


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep, YYLTYPE *yylocationp, Smt2newContext* context)
{
  YY_USE (yyvaluep);
  YY_USE (yylocationp);
  YY_USE (context);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  switch (yykind)
    {
    case YYSYMBOL_TK_NUM: /* TK_NUM  */
#line 79 "smt2newparser.yy"
            { free(((*yyvaluep).str)); }
#line 1521 "smt2newparser.cc"
        break;

    case YYSYMBOL_TK_SYM: /* TK_SYM  */
#line 79 "smt2newparser.yy"
            { free(((*yyvaluep).str)); }
#line 1527 "smt2newparser.cc"
        break;

    case YYSYMBOL_TK_QSYM: /* TK_QSYM  */
#line 79 "smt2newparser.yy"
            { free(((*yyvaluep).str)); }
#line 1533 "smt2newparser.cc"
        break;

    case YYSYMBOL_TK_KEY: /* TK_KEY  */
#line 79 "smt2newparser.yy"
            { free(((*yyvaluep).str)); }
#line 1539 "smt2newparser.cc"
        break;

    case YYSYMBOL_TK_STR: /* TK_STR  */
#line 79 "smt2newparser.yy"
            { free(((*yyvaluep).str)); }
#line 1545 "smt2newparser.cc"
        break;

    case YYSYMBOL_TK_DEC: /* TK_DEC  */
#line 79 "smt2newparser.yy"
            { free(((*yyvaluep).str)); }
#line 1551 "smt2newparser.cc"
        break;

    case YYSYMBOL_TK_HEX: /* TK_HEX  */
#line 79 "smt2newparser.yy"
            { free(((*yyvaluep).str)); }
#line 1557 "smt2newparser.cc"
        break;

    case YYSYMBOL_TK_BIN: /* TK_BIN  */
#line 79 "smt2newparser.yy"
            { free(((*yyvaluep).str)); }
#line 1563 "smt2newparser.cc"
        break;

    case YYSYMBOL_KW_SORTS: /* KW_SORTS  */
#line 79 "smt2newparser.yy"
            { free(((*yyvaluep).str)); }
#line 1569 "smt2newparser.cc"
        break;

    case YYSYMBOL_KW_FUNS: /* KW_FUNS  */
#line 79 "smt2newparser.yy"
            { free(((*yyvaluep).str)); }
#line 1575 "smt2newparser.cc"
        break;

    case YYSYMBOL_KW_SORTSDESCRIPTION: /* KW_SORTSDESCRIPTION  */
#line 79 "smt2newparser.yy"
            { free(((*yyvaluep).str)); }
#line 1581 "smt2newparser.cc"
        break;

    case YYSYMBOL_KW_FUNSDESCRIPTION: /* KW_FUNSDESCRIPTION  */
#line 79 "smt2newparser.yy"
            { free(((*yyvaluep).str)); }
#line 1587 "smt2newparser.cc"
        break;

    case YYSYMBOL_KW_DEFINITION: /* KW_DEFINITION  */
#line 79 "smt2newparser.yy"
            { free(((*yyvaluep).str)); }
#line 1593 "smt2newparser.cc"
        break;

    case YYSYMBOL_KW_NOTES: /* KW_NOTES  */
#line 79 "smt2newparser.yy"
            { free(((*yyvaluep).str)); }
#line 1599 "smt2newparser.cc"
        break;

    case YYSYMBOL_KW_THEORIES: /* KW_THEORIES  */
#line 79 "smt2newparser.yy"
            { free(((*yyvaluep).str)); }
#line 1605 "smt2newparser.cc"
        break;

    case YYSYMBOL_KW_EXTENSIONS: /* KW_EXTENSIONS  */
#line 79 "smt2newparser.yy"
            { free(((*yyvaluep).str)); }
#line 1611 "smt2newparser.cc"
        break;

    case YYSYMBOL_KW_VALUES: /* KW_VALUES  */
#line 79 "smt2newparser.yy"
            { free(((*yyvaluep).str)); }
#line 1617 "smt2newparser.cc"
        break;

    case YYSYMBOL_KW_PRINTSUCCESS: /* KW_PRINTSUCCESS  */
#line 79 "smt2newparser.yy"
            { free(((*yyvaluep).str)); }
#line 1623 "smt2newparser.cc"
        break;

    case YYSYMBOL_KW_EXPANDDEFINITIONS: /* KW_EXPANDDEFINITIONS  */
#line 79 "smt2newparser.yy"
            { free(((*yyvaluep).str)); }
#line 1629 "smt2newparser.cc"
        break;

    case YYSYMBOL_KW_INTERACTIVEMODE: /* KW_INTERACTIVEMODE  */
#line 79 "smt2newparser.yy"
            { free(((*yyvaluep).str)); }
#line 1635 "smt2newparser.cc"
        break;

    case YYSYMBOL_KW_PRODUCEPROOFS: /* KW_PRODUCEPROOFS  */
#line 79 "smt2newparser.yy"
            { free(((*yyvaluep).str)); }
#line 1641 "smt2newparser.cc"
        break;

    case YYSYMBOL_KW_PRODUCEUNSATCORES: /* KW_PRODUCEUNSATCORES  */
#line 79 "smt2newparser.yy"
            { free(((*yyvaluep).str)); }
#line 1647 "smt2newparser.cc"
        break;

    case YYSYMBOL_KW_MINIMALUNSATCORES: /* KW_MINIMALUNSATCORES  */
#line 79 "smt2newparser.yy"
            { free(((*yyvaluep).str)); }
#line 1653 "smt2newparser.cc"
        break;

    case YYSYMBOL_KW_PRINTCORESFULL: /* KW_PRINTCORESFULL  */
#line 79 "smt2newparser.yy"
            { free(((*yyvaluep).str)); }
#line 1659 "smt2newparser.cc"
        break;

    case YYSYMBOL_KW_PRODUCEMODELS: /* KW_PRODUCEMODELS  */
#line 79 "smt2newparser.yy"
            { free(((*yyvaluep).str)); }
#line 1665 "smt2newparser.cc"
        break;

    case YYSYMBOL_KW_PRODUCEASSIGNMENTS: /* KW_PRODUCEASSIGNMENTS  */
#line 79 "smt2newparser.yy"
            { free(((*yyvaluep).str)); }
#line 1671 "smt2newparser.cc"
        break;

    case YYSYMBOL_KW_REGULAROUTPUTCHANNEL: /* KW_REGULAROUTPUTCHANNEL  */
#line 79 "smt2newparser.yy"
            { free(((*yyvaluep).str)); }
#line 1677 "smt2newparser.cc"
        break;

    case YYSYMBOL_KW_DIAGNOSTICOUTPUTCHANNEL: /* KW_DIAGNOSTICOUTPUTCHANNEL  */
#line 79 "smt2newparser.yy"
            { free(((*yyvaluep).str)); }
#line 1683 "smt2newparser.cc"
        break;

    case YYSYMBOL_KW_RANDOMSEED: /* KW_RANDOMSEED  */
#line 79 "smt2newparser.yy"
            { free(((*yyvaluep).str)); }
#line 1689 "smt2newparser.cc"
        break;

    case YYSYMBOL_KW_VERBOSITY: /* KW_VERBOSITY  */
#line 79 "smt2newparser.yy"
            { free(((*yyvaluep).str)); }
#line 1695 "smt2newparser.cc"
        break;

    case YYSYMBOL_KW_ERRORBEHAVIOR: /* KW_ERRORBEHAVIOR  */
#line 79 "smt2newparser.yy"
            { free(((*yyvaluep).str)); }
#line 1701 "smt2newparser.cc"
        break;

    case YYSYMBOL_KW_NAME: /* KW_NAME  */
#line 79 "smt2newparser.yy"
            { free(((*yyvaluep).str)); }
#line 1707 "smt2newparser.cc"
        break;

    case YYSYMBOL_KW_NAMED: /* KW_NAMED  */
#line 79 "smt2newparser.yy"
            { free(((*yyvaluep).str)); }
#line 1713 "smt2newparser.cc"
        break;

    case YYSYMBOL_KW_AUTHORS: /* KW_AUTHORS  */
#line 79 "smt2newparser.yy"
            { free(((*yyvaluep).str)); }
#line 1719 "smt2newparser.cc"
        break;

    case YYSYMBOL_KW_VERSION: /* KW_VERSION  */
#line 79 "smt2newparser.yy"
            { free(((*yyvaluep).str)); }
#line 1725 "smt2newparser.cc"
        break;

    case YYSYMBOL_KW_STATUS: /* KW_STATUS  */
#line 79 "smt2newparser.yy"
            { free(((*yyvaluep).str)); }
#line 1731 "smt2newparser.cc"
        break;

    case YYSYMBOL_KW_REASONUNKNOWN: /* KW_REASONUNKNOWN  */
#line 79 "smt2newparser.yy"
            { free(((*yyvaluep).str)); }
#line 1737 "smt2newparser.cc"
        break;

    case YYSYMBOL_KW_ALLSTATISTICS: /* KW_ALLSTATISTICS  */
#line 79 "smt2newparser.yy"
            { free(((*yyvaluep).str)); }
#line 1743 "smt2newparser.cc"
        break;

    case YYSYMBOL_symbol: /* symbol  */
#line 80 "smt2newparser.yy"
            { delete ((*yyvaluep).snode); }
#line 1749 "smt2newparser.cc"
        break;

    case YYSYMBOL_command_list: /* command_list  */
#line 81 "smt2newparser.yy"
            { if (((*yyvaluep).snode_list)) { for (auto node : *((*yyvaluep).snode_list)) { delete node; } delete ((*yyvaluep).snode_list); }}
#line 1755 "smt2newparser.cc"
        break;

    case YYSYMBOL_command: /* command  */
#line 80 "smt2newparser.yy"
            { delete ((*yyvaluep).snode); }
#line 1761 "smt2newparser.cc"
        break;

    case YYSYMBOL_attribute_list: /* attribute_list  */
#line 81 "smt2newparser.yy"
            { if (((*yyvaluep).snode_list)) { for (auto node : *((*yyvaluep).snode_list)) { delete node; } delete ((*yyvaluep).snode_list); }}
#line 1767 "smt2newparser.cc"
        break;

    case YYSYMBOL_attribute: /* attribute  */
#line 80 "smt2newparser.yy"
            { delete ((*yyvaluep).snode); }
#line 1773 "smt2newparser.cc"
        break;

    case YYSYMBOL_attribute_value: /* attribute_value  */
#line 80 "smt2newparser.yy"
            { delete ((*yyvaluep).snode); }
#line 1779 "smt2newparser.cc"
        break;

    case YYSYMBOL_identifier: /* identifier  */
#line 80 "smt2newparser.yy"
            { delete ((*yyvaluep).snode); }
#line 1785 "smt2newparser.cc"
        break;

    case YYSYMBOL_sort: /* sort  */
#line 80 "smt2newparser.yy"
            { delete ((*yyvaluep).snode); }
#line 1791 "smt2newparser.cc"
        break;

    case YYSYMBOL_sort_list: /* sort_list  */
#line 81 "smt2newparser.yy"
            { if (((*yyvaluep).snode_list)) { for (auto node : *((*yyvaluep).snode_list)) { delete node; } delete ((*yyvaluep).snode_list); }}
#line 1797 "smt2newparser.cc"
        break;

    case YYSYMBOL_s_expr: /* s_expr  */
#line 80 "smt2newparser.yy"
            { delete ((*yyvaluep).snode); }
#line 1803 "smt2newparser.cc"
        break;

    case YYSYMBOL_s_expr_list: /* s_expr_list  */
#line 81 "smt2newparser.yy"
            { if (((*yyvaluep).snode_list)) { for (auto node : *((*yyvaluep).snode_list)) { delete node; } delete ((*yyvaluep).snode_list); }}
#line 1809 "smt2newparser.cc"
        break;

    case YYSYMBOL_spec_const: /* spec_const  */
#line 80 "smt2newparser.yy"
            { delete ((*yyvaluep).snode); }
#line 1815 "smt2newparser.cc"
        break;

    case YYSYMBOL_const_val: /* const_val  */
#line 80 "smt2newparser.yy"
            { delete ((*yyvaluep).snode); }
#line 1821 "smt2newparser.cc"
        break;

    case YYSYMBOL_numeral_list: /* numeral_list  */
#line 81 "smt2newparser.yy"
            { if (((*yyvaluep).snode_list)) { for (auto node : *((*yyvaluep).snode_list)) { delete node; } delete ((*yyvaluep).snode_list); }}
#line 1827 "smt2newparser.cc"
        break;

    case YYSYMBOL_qual_identifier: /* qual_identifier  */
#line 80 "smt2newparser.yy"
            { delete ((*yyvaluep).snode); }
#line 1833 "smt2newparser.cc"
        break;

    case YYSYMBOL_var_binding_list: /* var_binding_list  */
#line 81 "smt2newparser.yy"
            { if (((*yyvaluep).snode_list)) { for (auto node : *((*yyvaluep).snode_list)) { delete node; } delete ((*yyvaluep).snode_list); }}
#line 1839 "smt2newparser.cc"
        break;

    case YYSYMBOL_var_binding: /* var_binding  */
#line 80 "smt2newparser.yy"
            { delete ((*yyvaluep).snode); }
#line 1845 "smt2newparser.cc"
        break;

    case YYSYMBOL_sorted_var_list: /* sorted_var_list  */
#line 81 "smt2newparser.yy"
            { if (((*yyvaluep).snode_list)) { for (auto node : *((*yyvaluep).snode_list)) { delete node; } delete ((*yyvaluep).snode_list); }}
#line 1851 "smt2newparser.cc"
        break;

    case YYSYMBOL_sorted_var: /* sorted_var  */
#line 80 "smt2newparser.yy"
            { delete ((*yyvaluep).snode); }
#line 1857 "smt2newparser.cc"
        break;

    case YYSYMBOL_term_list: /* term_list  */
#line 81 "smt2newparser.yy"
            { if (((*yyvaluep).snode_list)) { for (auto node : *((*yyvaluep).snode_list)) { delete node; } delete ((*yyvaluep).snode_list); }}
#line 1863 "smt2newparser.cc"
        break;

    case YYSYMBOL_term: /* term  */
#line 80 "smt2newparser.yy"
            { delete ((*yyvaluep).snode); }
#line 1869 "smt2newparser.cc"
        break;

    case YYSYMBOL_symbol_list: /* symbol_list  */
#line 81 "smt2newparser.yy"
            { if (((*yyvaluep).snode_list)) { for (auto node : *((*yyvaluep).snode_list)) { delete node; } delete ((*yyvaluep).snode_list); }}
#line 1875 "smt2newparser.cc"
        break;

    case YYSYMBOL_b_value: /* b_value  */
#line 80 "smt2newparser.yy"
            { delete ((*yyvaluep).snode); }
#line 1881 "smt2newparser.cc"
        break;

    case YYSYMBOL_option: /* option  */
#line 80 "smt2newparser.yy"
            { delete ((*yyvaluep).snode); }
#line 1887 "smt2newparser.cc"
        break;

    case YYSYMBOL_predef_key: /* predef_key  */
#line 79 "smt2newparser.yy"
            { free(((*yyvaluep).str)); }
#line 1893 "smt2newparser.cc"
        break;

    case YYSYMBOL_info_flag: /* info_flag  */
#line 80 "smt2newparser.yy"
            { delete ((*yyvaluep).snode); }
#line 1899 "smt2newparser.cc"
        break;

      default:
        break;
    }
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}






/*----------.
| yyparse.  |
`----------*/

int
yyparse (Smt2newContext* context)
{
/* Lookahead token kind.  */
int yychar;


/* The semantic value of the lookahead symbol.  */
/* Default value used for initialization, for pacifying older GCCs
   or non-GCC compilers.  */
YY_INITIAL_VALUE (static YYSTYPE yyval_default;)
YYSTYPE yylval YY_INITIAL_VALUE (= yyval_default);

/* Location data for the lookahead symbol.  */
static YYLTYPE yyloc_default
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
  = { 1, 1, 1, 1 }
# endif
;
YYLTYPE yylloc = yyloc_default;

    /* Number of syntax errors so far.  */
    int yynerrs = 0;

    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

    /* The location stack: array, bottom, top.  */
    YYLTYPE yylsa[YYINITDEPTH];
    YYLTYPE *yyls = yylsa;
    YYLTYPE *yylsp = yyls;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

  /* The locations where the error started and ended.  */
  YYLTYPE yyerror_range[3];

  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYPTRDIFF_T yymsg_alloc = sizeof yymsgbuf;

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  yylsp[0] = yylloc;
  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;
        YYLTYPE *yyls1 = yyls;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yyls1, yysize * YYSIZEOF (*yylsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
        yyls = yyls1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
        YYSTACK_RELOCATE (yyls_alloc, yyls);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex (&yylval, &yylloc, scanner);
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      yyerror_range[1] = yylloc;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END
  *++yylsp = yylloc;

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

  /* Default location. */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  yyerror_range[1] = yyloc;
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* script: command_list  */
#line 102 "smt2newparser.yy"
                     { ASTNode *n = new ASTNode(CMDL_T, strdup("main-script")); n->children = (yyvsp[0].snode_list); context->insertRoot(n); }
#line 2205 "smt2newparser.cc"
    break;

  case 3: /* symbol: TK_SYM  */
#line 105 "smt2newparser.yy"
        { (yyval.snode) = new ASTNode(SYM_T, (yyvsp[0].str)); }
#line 2211 "smt2newparser.cc"
    break;

  case 4: /* symbol: TK_QSYM  */
#line 107 "smt2newparser.yy"
        { (yyval.snode) = new ASTNode(QSYM_T, (yyvsp[0].str)); }
#line 2217 "smt2newparser.cc"
    break;

  case 5: /* command_list: %empty  */
#line 111 "smt2newparser.yy"
        { (yyval.snode_list) = new std::vector<ASTNode*>(); }
#line 2223 "smt2newparser.cc"
    break;

  case 6: /* command_list: command_list command  */
#line 113 "smt2newparser.yy"
        { (*(yyvsp[-1].snode_list)).push_back((yyvsp[0].snode)); (yyval.snode_list) = (yyvsp[-1].snode_list); }
#line 2229 "smt2newparser.cc"
    break;

  case 7: /* command: '(' TK_SETLOGIC symbol ')'  */
#line 117 "smt2newparser.yy"
        {
            (yyval.snode) = new ASTNode(CMD_T, (yyvsp[-2].tok));
            (yyval.snode)->children = new std::vector<ASTNode*>();
            (yyval.snode)->children->push_back((yyvsp[-1].snode));
        }
#line 2239 "smt2newparser.cc"
    break;

  case 8: /* command: '(' TK_SETOPTION option ')'  */
#line 123 "smt2newparser.yy"
        {
            (yyval.snode) = new ASTNode(CMD_T, (yyvsp[-2].tok));
            (yyval.snode)->children = new std::vector<ASTNode*>();
            (yyval.snode)->children->push_back((yyvsp[-1].snode));
        }
#line 2249 "smt2newparser.cc"
    break;

  case 9: /* command: '(' TK_SETINFO attribute ')'  */
#line 129 "smt2newparser.yy"
        {
            (yyval.snode) = new ASTNode(CMD_T, (yyvsp[-2].tok));
            (yyval.snode)->children = new std::vector<ASTNode*>();
            (yyval.snode)->children->push_back((yyvsp[-1].snode));
        }
#line 2259 "smt2newparser.cc"
    break;

  case 10: /* command: '(' TK_DECLARESORT symbol TK_NUM ')'  */
#line 135 "smt2newparser.yy"
        {
            (yyval.snode) = new ASTNode(CMD_T, (yyvsp[-3].tok));
            (yyval.snode)->children = new std::vector<ASTNode*>();
            (yyval.snode)->children->push_back((yyvsp[-2].snode));
            (yyval.snode)->children->push_back(new ASTNode(NUM_T, (yyvsp[-1].str)));
        }
#line 2270 "smt2newparser.cc"
    break;

  case 11: /* command: '(' TK_DEFINESORT symbol '(' symbol_list ')' sort ')'  */
#line 142 "smt2newparser.yy"
        {
            (yyval.snode) = new ASTNode(CMD_T, (yyvsp[-6].tok));
            (yyval.snode)->children = new std::vector<ASTNode*>();
            (yyval.snode)->children->push_back((yyvsp[-5].snode));

            ASTNode* syml = new ASTNode(SYML_T, NULL);
            syml->children = (yyvsp[-3].snode_list);
            (yyval.snode)->children->push_back(syml);

            (yyval.snode)->children->push_back((yyvsp[-1].snode));
        }
#line 2286 "smt2newparser.cc"
    break;

  case 12: /* command: '(' TK_DECLAREFUN symbol '(' sort_list ')' sort ')'  */
#line 155 "smt2newparser.yy"
        {
            (yyval.snode) = new ASTNode(CMD_T, (yyvsp[-6].tok));
            (yyval.snode)->children = new std::vector<ASTNode*>();
            (yyval.snode)->children->push_back((yyvsp[-5].snode));

            ASTNode* sortl = new ASTNode(SORTL_T, NULL);
            sortl->children = (yyvsp[-3].snode_list);
            (yyval.snode)->children->push_back(sortl);

            (yyval.snode)->children->push_back((yyvsp[-1].snode));
        }
#line 2302 "smt2newparser.cc"
    break;

  case 13: /* command: '(' TK_DECLARECONST const_val sort ')'  */
#line 167 "smt2newparser.yy"
        {
            (yyval.snode) = new ASTNode(CMD_T, (yyvsp[-3].tok));
            (yyval.snode)->children = new std::vector<ASTNode*>();
            (yyval.snode)->children->push_back((yyvsp[-2].snode));

            ASTNode* sortl = new ASTNode(SORTL_T, NULL);
            sortl->children = new std::vector<ASTNode*>();
            (yyval.snode)->children->push_back(sortl);

            (yyval.snode)->children->push_back((yyvsp[-1].snode));
        }
#line 2318 "smt2newparser.cc"
    break;

  case 14: /* command: '(' TK_DEFINEFUN symbol '(' sorted_var_list ')' sort term ')'  */
#line 179 "smt2newparser.yy"
        {
            (yyval.snode) = new ASTNode(CMD_T, (yyvsp[-7].tok));
            (yyval.snode)->children = new std::vector<ASTNode*>();
            (yyval.snode)->children->push_back((yyvsp[-6].snode));

            ASTNode* svl = new ASTNode(SVL_T, NULL);
            svl->children = (yyvsp[-4].snode_list);
            (yyval.snode)->children->push_back(svl);

            (yyval.snode)->children->push_back((yyvsp[-2].snode));
            (yyval.snode)->children->push_back((yyvsp[-1].snode));
        }
#line 2335 "smt2newparser.cc"
    break;

  case 15: /* command: '(' TK_PUSH TK_NUM ')'  */
#line 192 "smt2newparser.yy"
        {
            (yyval.snode) = new ASTNode(CMD_T, (yyvsp[-2].tok));
            (yyval.snode)->children = new std::vector<ASTNode*>();
            (yyval.snode)->children->push_back(new ASTNode(NUM_T, (yyvsp[-1].str)));
        }
#line 2345 "smt2newparser.cc"
    break;

  case 16: /* command: '(' TK_POP TK_NUM ')'  */
#line 198 "smt2newparser.yy"
        {
            (yyval.snode) = new ASTNode(CMD_T, (yyvsp[-2].tok));
            (yyval.snode)->children = new std::vector<ASTNode*>();
            (yyval.snode)->children->push_back(new ASTNode(NUM_T, (yyvsp[-1].str)));
        }
#line 2355 "smt2newparser.cc"
    break;

  case 17: /* command: '(' TK_ASSERT term ')'  */
#line 204 "smt2newparser.yy"
        {
            (yyval.snode) = new ASTNode(CMD_T, (yyvsp[-2].tok));
            (yyval.snode)->children = new std::vector<ASTNode*>();
            (yyval.snode)->children->push_back((yyvsp[-1].snode));
        }
#line 2365 "smt2newparser.cc"
    break;

  case 18: /* command: '(' TK_CHECKSAT ')'  */
#line 210 "smt2newparser.yy"
        {
            (yyval.snode) = new ASTNode(CMD_T, (yyvsp[-1].tok));
        }
#line 2373 "smt2newparser.cc"
    break;

  case 19: /* command: '(' TK_GETASSERTIONS ')'  */
#line 214 "smt2newparser.yy"
        {
            (yyval.snode) = new ASTNode(CMD_T, (yyvsp[-1].tok));
        }
#line 2381 "smt2newparser.cc"
    break;

  case 20: /* command: '(' TK_GETPROOF ')'  */
#line 218 "smt2newparser.yy"
        {
            (yyval.snode) = new ASTNode(CMD_T, (yyvsp[-1].tok));
        }
#line 2389 "smt2newparser.cc"
    break;

  case 21: /* command: '(' TK_GETITPS term_list ')'  */
#line 222 "smt2newparser.yy"
        {
            (yyval.snode) = new ASTNode(CMD_T, (yyvsp[-2].tok));
            (yyval.snode)->children = (yyvsp[-1].snode_list);
        }
#line 2398 "smt2newparser.cc"
    break;

  case 22: /* command: '(' TK_WRSTATE TK_STR ')'  */
#line 227 "smt2newparser.yy"
        {
            (yyval.snode) = new ASTNode(CMD_T, (yyvsp[-2].tok));
            (yyval.snode)->children = new std::vector<ASTNode*>();
            (yyval.snode)->children->push_back(new ASTNode(UATTR_T, (yyvsp[-1].str)));
        }
#line 2408 "smt2newparser.cc"
    break;

  case 23: /* command: '(' TK_RDSTATE TK_STR ')'  */
#line 233 "smt2newparser.yy"
        {
            (yyval.snode) = new ASTNode(CMD_T, (yyvsp[-2].tok));
            (yyval.snode)->children = new std::vector<ASTNode*>();
            (yyval.snode)->children->push_back(new ASTNode(UATTR_T, (yyvsp[-1].str)));
        }
#line 2418 "smt2newparser.cc"
    break;

  case 24: /* command: '(' TK_WRFUNS TK_STR ')'  */
#line 239 "smt2newparser.yy"
        {
            (yyval.snode) = new ASTNode(CMD_T, (yyvsp[-2].tok));
            (yyval.snode)->children = new std::vector<ASTNode*>();
            (yyval.snode)->children->push_back(new ASTNode(UATTR_T, (yyvsp[-1].str)));
        }
#line 2428 "smt2newparser.cc"
    break;

  case 25: /* command: '(' TK_GETUNSATCORE ')'  */
#line 245 "smt2newparser.yy"
        {
            (yyval.snode) = new ASTNode(CMD_T, (yyvsp[-1].tok));
        }
#line 2436 "smt2newparser.cc"
    break;

  case 26: /* command: '(' TK_GETVALUE '(' term term_list ')' ')'  */
#line 249 "smt2newparser.yy"
        {
            (yyval.snode) = new ASTNode(CMD_T, (yyvsp[-5].tok));
            (yyval.snode)->children = (yyvsp[-2].snode_list);
            (yyval.snode)->children->insert((yyval.snode)->children->begin(), (yyvsp[-3].snode));
        }
#line 2446 "smt2newparser.cc"
    break;

  case 27: /* command: '(' TK_GETMODEL ')'  */
#line 255 "smt2newparser.yy"
            {
                (yyval.snode) = new ASTNode(CMD_T, (yyvsp[-1].tok));
            }
#line 2454 "smt2newparser.cc"
    break;

  case 28: /* command: '(' TK_GETASSIGNMENT ')'  */
#line 259 "smt2newparser.yy"
        {
            (yyval.snode) = new ASTNode(CMD_T, (yyvsp[-1].tok));
        }
#line 2462 "smt2newparser.cc"
    break;

  case 29: /* command: '(' TK_GETOPTION TK_KEY ')'  */
#line 263 "smt2newparser.yy"
        {
            (yyval.snode) = new ASTNode(CMD_T, (yyvsp[-2].tok));
            (yyval.snode)->children = new std::vector<ASTNode*>();
            (yyval.snode)->children->push_back(new ASTNode(UATTR_T, (yyvsp[-1].str)));
        }
#line 2472 "smt2newparser.cc"
    break;

  case 30: /* command: '(' TK_GETOPTION predef_key ')'  */
#line 269 "smt2newparser.yy"
        {
            (yyval.snode) = new ASTNode(CMD_T, (yyvsp[-2].tok));
            (yyval.snode)->children = new std::vector<ASTNode*>();
            (yyval.snode)->children->push_back(new ASTNode(PATTR_T, (yyvsp[-1].str)));
        }
#line 2482 "smt2newparser.cc"
    break;

  case 31: /* command: '(' TK_GETINFO info_flag ')'  */
#line 275 "smt2newparser.yy"
        {
            (yyval.snode) = new ASTNode(CMD_T, (yyvsp[-2].tok));
            (yyval.snode)->children = new std::vector<ASTNode*>();
            (yyval.snode)->children->push_back((yyvsp[-1].snode));
        }
#line 2492 "smt2newparser.cc"
    break;

  case 32: /* command: '(' TK_SIMPLIFY ')'  */
#line 281 "smt2newparser.yy"
        {
            (yyval.snode) = new ASTNode(CMD_T, (yyvsp[-1].tok));
        }
#line 2500 "smt2newparser.cc"
    break;

  case 33: /* command: '(' TK_EXIT ')'  */
#line 285 "smt2newparser.yy"
        { (yyval.snode) = new ASTNode(CMD_T, (yyvsp[-1].tok)); }
#line 2506 "smt2newparser.cc"
    break;

  case 34: /* command: '(' TK_ECHO TK_STR ')'  */
#line 287 "smt2newparser.yy"
        {
            (yyval.snode) = new ASTNode(CMD_T, (yyvsp[-2].tok));
            (yyval.snode)->children = new std::vector<ASTNode*>();
            (yyval.snode)->children->push_back(new ASTNode(UATTR_T, (yyvsp[-1].str)));
        }
#line 2516 "smt2newparser.cc"
    break;

  case 35: /* attribute_list: %empty  */
#line 295 "smt2newparser.yy"
        { (yyval.snode_list) = new std::vector<ASTNode*>(); }
#line 2522 "smt2newparser.cc"
    break;

  case 36: /* attribute_list: attribute_list attribute  */
#line 297 "smt2newparser.yy"
        { (yyvsp[-1].snode_list)->push_back((yyvsp[0].snode)); (yyval.snode_list) = (yyvsp[-1].snode_list); }
#line 2528 "smt2newparser.cc"
    break;

  case 37: /* attribute: TK_KEY  */
#line 301 "smt2newparser.yy"
        { (yyval.snode) = new ASTNode(UATTR_T, (yyvsp[0].str)); }
#line 2534 "smt2newparser.cc"
    break;

  case 38: /* attribute: TK_KEY attribute_value  */
#line 303 "smt2newparser.yy"
        { (yyval.snode) = new ASTNode(UATTR_T, (yyvsp[-1].str)); (yyval.snode)->children = new std::vector<ASTNode*>(); (yyval.snode)->children->push_back((yyvsp[0].snode)); }
#line 2540 "smt2newparser.cc"
    break;

  case 39: /* attribute: predef_key  */
#line 305 "smt2newparser.yy"
        { (yyval.snode) = new ASTNode(PATTR_T, (yyvsp[0].str)); }
#line 2546 "smt2newparser.cc"
    break;

  case 40: /* attribute: predef_key attribute_value  */
#line 307 "smt2newparser.yy"
        { (yyval.snode) = new ASTNode(PATTR_T, (yyvsp[-1].str)); (yyval.snode)->children = new std::vector<ASTNode*>(); (yyval.snode)->children->push_back((yyvsp[0].snode)); }
#line 2552 "smt2newparser.cc"
    break;

  case 41: /* attribute_value: spec_const  */
#line 311 "smt2newparser.yy"
        { (yyval.snode) = new ASTNode(SPECC_T, NULL); (yyval.snode)->children = new std::vector<ASTNode*>(); (yyval.snode)->children->push_back((yyvsp[0].snode)); }
#line 2558 "smt2newparser.cc"
    break;

  case 42: /* attribute_value: symbol  */
#line 313 "smt2newparser.yy"
        {
            (yyval.snode) = (yyvsp[0].snode);
        }
#line 2566 "smt2newparser.cc"
    break;

  case 43: /* attribute_value: '(' s_expr_list ')'  */
#line 317 "smt2newparser.yy"
        {
            (yyval.snode) = new ASTNode(SEXPRL_T, NULL);
            (yyval.snode)->children = (yyvsp[-1].snode_list);
        }
#line 2575 "smt2newparser.cc"
    break;

  case 44: /* identifier: symbol  */
#line 324 "smt2newparser.yy"
        { (yyval.snode) = (yyvsp[0].snode); }
#line 2581 "smt2newparser.cc"
    break;

  case 45: /* identifier: '(' '_' symbol numeral_list ')'  */
#line 326 "smt2newparser.yy"
        { (yyval.snode) = (yyvsp[-2].snode); (yyval.snode)->children = (yyvsp[-1].snode_list); }
#line 2587 "smt2newparser.cc"
    break;

  case 46: /* sort: identifier  */
#line 330 "smt2newparser.yy"
      { (yyval.snode) = (yyvsp[0].snode); }
#line 2593 "smt2newparser.cc"
    break;

  case 47: /* sort: '(' identifier sort sort_list ')'  */
#line 332 "smt2newparser.yy"
      {
        (yyval.snode) = new ASTNode(LID_T, NULL);
        (yyval.snode)->children = (yyvsp[-1].snode_list);
        (yyval.snode)->children->insert((yyval.snode)->children->begin(), (yyvsp[-2].snode));
        (yyval.snode)->children->insert((yyval.snode)->children->begin(), (yyvsp[-3].snode));
      }
#line 2604 "smt2newparser.cc"
    break;

  case 48: /* sort_list: sort_list sort  */
#line 341 "smt2newparser.yy"
        { (yyvsp[-1].snode_list)->push_back((yyvsp[0].snode)); (yyval.snode_list) = (yyvsp[-1].snode_list); }
#line 2610 "smt2newparser.cc"
    break;

  case 49: /* sort_list: %empty  */
#line 343 "smt2newparser.yy"
        { (yyval.snode_list) = new std::vector<ASTNode*>(); }
#line 2616 "smt2newparser.cc"
    break;

  case 50: /* s_expr: spec_const  */
#line 347 "smt2newparser.yy"
        {
            (yyval.snode) = new ASTNode(SPECC_T, NULL);
            (yyval.snode)->children = new std::vector<ASTNode*>();
            (yyval.snode)->children->push_back((yyvsp[0].snode));
        }
#line 2626 "smt2newparser.cc"
    break;

  case 51: /* s_expr: symbol  */
#line 353 "smt2newparser.yy"
        {
            (yyval.snode) = (yyvsp[0].snode);
        }
#line 2634 "smt2newparser.cc"
    break;

  case 52: /* s_expr: TK_KEY  */
#line 357 "smt2newparser.yy"
        {
            (yyval.snode) = new ASTNode(UATTR_T, (yyvsp[0].str));
        }
#line 2642 "smt2newparser.cc"
    break;

  case 53: /* s_expr: '(' s_expr_list ')'  */
#line 361 "smt2newparser.yy"
        {
            (yyval.snode) = new ASTNode(SEXPRL_T, NULL);
            (yyval.snode)->children = (yyvsp[-1].snode_list);
        }
#line 2651 "smt2newparser.cc"
    break;

  case 54: /* s_expr_list: %empty  */
#line 368 "smt2newparser.yy"
        {
            (yyval.snode_list) = new std::vector<ASTNode*>();
        }
#line 2659 "smt2newparser.cc"
    break;

  case 55: /* s_expr_list: s_expr_list s_expr  */
#line 372 "smt2newparser.yy"
        {
            (yyvsp[-1].snode_list)->push_back((yyvsp[0].snode));
            (yyval.snode_list) = (yyvsp[-1].snode_list);
        }
#line 2668 "smt2newparser.cc"
    break;

  case 56: /* spec_const: TK_NUM  */
#line 380 "smt2newparser.yy"
        { (yyval.snode) = new ASTNode(NUM_T, (yyvsp[0].str)); }
#line 2674 "smt2newparser.cc"
    break;

  case 57: /* spec_const: TK_DEC  */
#line 382 "smt2newparser.yy"
        { (yyval.snode) = new ASTNode(DEC_T, (yyvsp[0].str)); }
#line 2680 "smt2newparser.cc"
    break;

  case 58: /* spec_const: TK_HEX  */
#line 384 "smt2newparser.yy"
        { (yyval.snode) = new ASTNode(HEX_T, (yyvsp[0].str)); }
#line 2686 "smt2newparser.cc"
    break;

  case 59: /* spec_const: TK_BIN  */
#line 386 "smt2newparser.yy"
        { (yyval.snode) = new ASTNode(BIN_T, (yyvsp[0].str)); }
#line 2692 "smt2newparser.cc"
    break;

  case 60: /* spec_const: TK_STR  */
#line 388 "smt2newparser.yy"
        { (yyval.snode) = new ASTNode(STR_T, (yyvsp[0].str)); }
#line 2698 "smt2newparser.cc"
    break;

  case 61: /* const_val: symbol  */
#line 392 "smt2newparser.yy"
        { (yyval.snode) = (yyvsp[0].snode); }
#line 2704 "smt2newparser.cc"
    break;

  case 62: /* const_val: spec_const  */
#line 394 "smt2newparser.yy"
        { (yyval.snode) = (yyvsp[0].snode); }
#line 2710 "smt2newparser.cc"
    break;

  case 63: /* numeral_list: numeral_list TK_NUM  */
#line 398 "smt2newparser.yy"
        { (yyvsp[-1].snode_list)->push_back(new ASTNode(NUM_T, (yyvsp[0].str))); (yyval.snode_list) = (yyvsp[-1].snode_list); }
#line 2716 "smt2newparser.cc"
    break;

  case 64: /* numeral_list: TK_NUM  */
#line 400 "smt2newparser.yy"
        { (yyval.snode_list) = new std::vector<ASTNode*>(); (yyval.snode_list)->push_back(new ASTNode(NUM_T, (yyvsp[0].str))); }
#line 2722 "smt2newparser.cc"
    break;

  case 65: /* qual_identifier: identifier  */
#line 404 "smt2newparser.yy"
        { (yyval.snode) = (yyvsp[0].snode); }
#line 2728 "smt2newparser.cc"
    break;

  case 66: /* qual_identifier: '(' TK_AS identifier sort ')'  */
#line 406 "smt2newparser.yy"
        {
            (yyval.snode) = new ASTNode(AS_T, NULL);
            (yyval.snode)->children = new std::vector<ASTNode*>();
            (yyval.snode)->children->push_back((yyvsp[-2].snode));
            (yyval.snode)->children->push_back((yyvsp[-1].snode));
        }
#line 2739 "smt2newparser.cc"
    break;

  case 67: /* var_binding_list: %empty  */
#line 415 "smt2newparser.yy"
        { (yyval.snode_list) = new std::vector<ASTNode*>(); }
#line 2745 "smt2newparser.cc"
    break;

  case 68: /* var_binding_list: var_binding_list var_binding  */
#line 417 "smt2newparser.yy"
        { (yyvsp[-1].snode_list)->push_back((yyvsp[0].snode)); (yyval.snode_list) = (yyvsp[-1].snode_list); }
#line 2751 "smt2newparser.cc"
    break;

  case 69: /* var_binding: '(' symbol term ')'  */
#line 421 "smt2newparser.yy"
        { (yyval.snode) = new ASTNode(VARB_T, strdup((yyvsp[-2].snode)->getValue())); delete (yyvsp[-2].snode); (yyval.snode)->children = new std::vector<ASTNode*>(); (yyval.snode)->children->push_back((yyvsp[-1].snode)); }
#line 2757 "smt2newparser.cc"
    break;

  case 70: /* sorted_var_list: %empty  */
#line 425 "smt2newparser.yy"
        { (yyval.snode_list) = new std::vector<ASTNode*>(); }
#line 2763 "smt2newparser.cc"
    break;

  case 71: /* sorted_var_list: sorted_var_list sorted_var  */
#line 427 "smt2newparser.yy"
        { (yyvsp[-1].snode_list)->push_back((yyvsp[0].snode)); (yyval.snode_list) = (yyvsp[-1].snode_list); }
#line 2769 "smt2newparser.cc"
    break;

  case 72: /* sorted_var: '(' symbol sort ')'  */
#line 431 "smt2newparser.yy"
        { (yyval.snode) = new ASTNode(SV_T, strdup((yyvsp[-2].snode)->getValue())); delete (yyvsp[-2].snode); (yyval.snode)->children = new std::vector<ASTNode*>(); (yyval.snode)->children->push_back((yyvsp[-1].snode)); }
#line 2775 "smt2newparser.cc"
    break;

  case 73: /* term_list: %empty  */
#line 434 "smt2newparser.yy"
        { (yyval.snode_list) = new std::vector<ASTNode*>(); }
#line 2781 "smt2newparser.cc"
    break;

  case 74: /* term_list: term_list term  */
#line 436 "smt2newparser.yy"
        { (yyvsp[-1].snode_list)->push_back((yyvsp[0].snode)); (yyval.snode_list) = (yyvsp[-1].snode_list); }
#line 2787 "smt2newparser.cc"
    break;

  case 75: /* term: spec_const  */
#line 440 "smt2newparser.yy"
        { (yyval.snode) = new ASTNode(TERM_T, NULL); (yyval.snode)->children = new std::vector<ASTNode*>(); (yyval.snode)->children->push_back((yyvsp[0].snode)); }
#line 2793 "smt2newparser.cc"
    break;

  case 76: /* term: qual_identifier  */
#line 442 "smt2newparser.yy"
        { (yyval.snode) = new ASTNode(QID_T, NULL); (yyval.snode)->children = new std::vector<ASTNode*>(); (yyval.snode)->children->push_back((yyvsp[0].snode)); }
#line 2799 "smt2newparser.cc"
    break;

  case 77: /* term: '(' qual_identifier term term_list ')'  */
#line 444 "smt2newparser.yy"
        {
            (yyval.snode) = new ASTNode(LQID_T, NULL);
            (yyval.snode)->children = (yyvsp[-1].snode_list);
            (yyval.snode)->children->insert((yyval.snode)->children->begin(), (yyvsp[-2].snode));
            (yyval.snode)->children->insert((yyval.snode)->children->begin(), (yyvsp[-3].snode));
        }
#line 2810 "smt2newparser.cc"
    break;

  case 78: /* term: '(' TK_LET '(' var_binding var_binding_list ')' term ')'  */
#line 451 "smt2newparser.yy"
        {
            (yyval.snode) = new ASTNode(LET_T, NULL);
            (yyval.snode)->children = new std::vector<ASTNode*>();
            (yyvsp[-3].snode_list)->insert((yyvsp[-3].snode_list)->begin(), (yyvsp[-4].snode));
            ASTNode* vbl = new ASTNode(VARBL_T, NULL);
            vbl->children = (yyvsp[-3].snode_list);
            (yyval.snode)->children->push_back(vbl);
            (yyval.snode)->children->push_back((yyvsp[-1].snode));
        }
#line 2824 "smt2newparser.cc"
    break;

  case 79: /* term: '(' TK_FORALL '(' sorted_var sorted_var_list ')' term ')'  */
#line 461 "smt2newparser.yy"
        {
            (yyval.snode) = new ASTNode(FORALL_T, NULL);
            (yyval.snode)->children = new std::vector<ASTNode*>();
            (yyvsp[-3].snode_list)->insert((yyvsp[-3].snode_list)->begin(), (yyvsp[-4].snode));
            ASTNode* svl = new ASTNode(SVL_T, NULL);
            svl->children = (yyvsp[-3].snode_list);
            (yyval.snode)->children->push_back(svl);
            (yyval.snode)->children->push_back((yyvsp[-1].snode));
        }
#line 2838 "smt2newparser.cc"
    break;

  case 80: /* term: '(' TK_EXISTS '(' sorted_var sorted_var_list ')' term ')'  */
#line 471 "smt2newparser.yy"
        {
            (yyval.snode) = new ASTNode(EXISTS_T, NULL);
            (yyval.snode)->children = new std::vector<ASTNode*>();
            (yyvsp[-3].snode_list)->insert((yyvsp[-3].snode_list)->begin(), (yyvsp[-4].snode));
            ASTNode* svl = new ASTNode(SVL_T, NULL);
            svl->children = (yyvsp[-3].snode_list);
            (yyval.snode)->children->push_back(svl);
            (yyval.snode)->children->push_back((yyvsp[-1].snode));
        }
#line 2852 "smt2newparser.cc"
    break;

  case 81: /* term: '(' '!' term attribute attribute_list ')'  */
#line 481 "smt2newparser.yy"
        {
            (yyval.snode) = new ASTNode(BANG_T, NULL);
            (yyval.snode)->children = new std::vector<ASTNode*>();
            (yyval.snode)->children->push_back((yyvsp[-3].snode));
            ASTNode *atrs = new ASTNode(GATTRL_T, NULL);
            (yyvsp[-1].snode_list)->insert((yyvsp[-1].snode_list)->begin(), (yyvsp[-2].snode));
            atrs->children = (yyvsp[-1].snode_list);
            (yyval.snode)->children->push_back(atrs);
        }
#line 2866 "smt2newparser.cc"
    break;

  case 82: /* symbol_list: %empty  */
#line 493 "smt2newparser.yy"
        { (yyval.snode_list) = new std::vector<ASTNode*>(); }
#line 2872 "smt2newparser.cc"
    break;

  case 83: /* symbol_list: symbol_list symbol  */
#line 495 "smt2newparser.yy"
        { (yyvsp[-1].snode_list)->push_back((yyvsp[0].snode)); (yyval.snode_list) = (yyvsp[-1].snode_list); }
#line 2878 "smt2newparser.cc"
    break;

  case 84: /* b_value: symbol  */
#line 499 "smt2newparser.yy"
        {
            const char * str = (yyvsp[0].snode)->getValue();
            if (strcmp(str, "true") == 0 or strcmp(str, "false") == 0) {
                (yyval.snode) = new ASTNode(BOOL_T, strdup((yyvsp[0].snode)->getValue())); delete (yyvsp[0].snode);
            }
            else {
                printf("Syntax error: expecting either 'true' or 'false', got '%s'\n", str);
                delete (yyvsp[0].snode);
                YYERROR;
            }
        }
#line 2894 "smt2newparser.cc"
    break;

  case 85: /* option: KW_PRINTSUCCESS b_value  */
#line 513 "smt2newparser.yy"
        {
            (yyval.snode) = new ASTNode(OPTION_T, (yyvsp[-1].str));
            (yyval.snode)->children = new std::vector<ASTNode*>();
            (yyval.snode)->children->push_back((yyvsp[0].snode));
        }
#line 2904 "smt2newparser.cc"
    break;

  case 86: /* option: KW_EXPANDDEFINITIONS b_value  */
#line 519 "smt2newparser.yy"
        {
            (yyval.snode) = new ASTNode(OPTION_T, (yyvsp[-1].str));
            (yyval.snode)->children = new std::vector<ASTNode*>();
            (yyval.snode)->children->push_back((yyvsp[0].snode));
        }
#line 2914 "smt2newparser.cc"
    break;

  case 87: /* option: KW_INTERACTIVEMODE b_value  */
#line 525 "smt2newparser.yy"
        {
            (yyval.snode) = new ASTNode(OPTION_T, (yyvsp[-1].str));
            (yyval.snode)->children = new std::vector<ASTNode*>();
            (yyval.snode)->children->push_back((yyvsp[0].snode));
        }
#line 2924 "smt2newparser.cc"
    break;

  case 88: /* option: KW_PRODUCEPROOFS b_value  */
#line 531 "smt2newparser.yy"
        {
            (yyval.snode) = new ASTNode(OPTION_T, (yyvsp[-1].str));
            (yyval.snode)->children = new std::vector<ASTNode*>();
            (yyval.snode)->children->push_back((yyvsp[0].snode));
        }
#line 2934 "smt2newparser.cc"
    break;

  case 89: /* option: KW_PRODUCEUNSATCORES b_value  */
#line 537 "smt2newparser.yy"
        {
            (yyval.snode) = new ASTNode(OPTION_T, (yyvsp[-1].str));
            (yyval.snode)->children = new std::vector<ASTNode*>();
            (yyval.snode)->children->push_back((yyvsp[0].snode));
        }
#line 2944 "smt2newparser.cc"
    break;

  case 90: /* option: KW_MINIMALUNSATCORES b_value  */
#line 543 "smt2newparser.yy"
        {
            (yyval.snode) = new ASTNode(OPTION_T, (yyvsp[-1].str));
            (yyval.snode)->children = new std::vector<ASTNode*>();
            (yyval.snode)->children->push_back((yyvsp[0].snode));
        }
#line 2954 "smt2newparser.cc"
    break;

  case 91: /* option: KW_PRINTCORESFULL b_value  */
#line 549 "smt2newparser.yy"
        {
            (yyval.snode) = new ASTNode(OPTION_T, (yyvsp[-1].str));
            (yyval.snode)->children = new std::vector<ASTNode*>();
            (yyval.snode)->children->push_back((yyvsp[0].snode));
        }
#line 2964 "smt2newparser.cc"
    break;

  case 92: /* option: KW_PRODUCEMODELS b_value  */
#line 555 "smt2newparser.yy"
        {
            (yyval.snode) = new ASTNode(OPTION_T, (yyvsp[-1].str));
            (yyval.snode)->children = new std::vector<ASTNode*>();
            (yyval.snode)->children->push_back((yyvsp[0].snode));
        }
#line 2974 "smt2newparser.cc"
    break;

  case 93: /* option: KW_PRODUCEASSIGNMENTS b_value  */
#line 561 "smt2newparser.yy"
        {
            (yyval.snode) = new ASTNode(OPTION_T, (yyvsp[-1].str));
            (yyval.snode)->children = new std::vector<ASTNode*>();
            (yyval.snode)->children->push_back((yyvsp[0].snode));
        }
#line 2984 "smt2newparser.cc"
    break;

  case 94: /* option: KW_REGULAROUTPUTCHANNEL TK_STR  */
#line 567 "smt2newparser.yy"
        {
            (yyval.snode) = new ASTNode(OPTION_T, (yyvsp[-1].str));
            (yyval.snode)->children = new std::vector<ASTNode*>();
            (yyval.snode)->children->push_back(new ASTNode(STR_T, (yyvsp[0].str)));
        }
#line 2994 "smt2newparser.cc"
    break;

  case 95: /* option: KW_DIAGNOSTICOUTPUTCHANNEL TK_STR  */
#line 573 "smt2newparser.yy"
        {
            (yyval.snode) = new ASTNode(OPTION_T, (yyvsp[-1].str));
            (yyval.snode)->children = new std::vector<ASTNode*>();
            (yyval.snode)->children->push_back(new ASTNode(STR_T, (yyvsp[0].str)));
        }
#line 3004 "smt2newparser.cc"
    break;

  case 96: /* option: KW_RANDOMSEED TK_NUM  */
#line 579 "smt2newparser.yy"
        {
            (yyval.snode) = new ASTNode(OPTION_T, (yyvsp[-1].str));
            (yyval.snode)->children = new std::vector<ASTNode*>();
            (yyval.snode)->children->push_back(new ASTNode(NUM_T, (yyvsp[0].str)));
        }
#line 3014 "smt2newparser.cc"
    break;

  case 97: /* option: KW_VERBOSITY TK_NUM  */
#line 585 "smt2newparser.yy"
        {
            (yyval.snode) = new ASTNode(OPTION_T, (yyvsp[-1].str));
            (yyval.snode)->children = new std::vector<ASTNode*>();
            (yyval.snode)->children->push_back(new ASTNode(NUM_T, (yyvsp[0].str)));
        }
#line 3024 "smt2newparser.cc"
    break;

  case 98: /* option: attribute  */
#line 591 "smt2newparser.yy"
        {
            (yyval.snode) = new ASTNode(OPTION_T, NULL);
            (yyval.snode)->children = new std::vector<ASTNode*>();
            (yyval.snode)->children->push_back((yyvsp[0].snode));
        }
#line 3034 "smt2newparser.cc"
    break;

  case 99: /* predef_key: KW_SORTS  */
#line 599 "smt2newparser.yy"
        { (yyval.str) = (yyvsp[0].str); }
#line 3040 "smt2newparser.cc"
    break;

  case 100: /* predef_key: KW_FUNS  */
#line 601 "smt2newparser.yy"
        { (yyval.str) = (yyvsp[0].str); }
#line 3046 "smt2newparser.cc"
    break;

  case 101: /* predef_key: KW_SORTSDESCRIPTION  */
#line 603 "smt2newparser.yy"
        { (yyval.str) = (yyvsp[0].str); }
#line 3052 "smt2newparser.cc"
    break;

  case 102: /* predef_key: KW_FUNSDESCRIPTION  */
#line 605 "smt2newparser.yy"
        { (yyval.str) = (yyvsp[0].str); }
#line 3058 "smt2newparser.cc"
    break;

  case 103: /* predef_key: KW_DEFINITION  */
#line 607 "smt2newparser.yy"
        { (yyval.str) = (yyvsp[0].str); }
#line 3064 "smt2newparser.cc"
    break;

  case 104: /* predef_key: KW_VALUES  */
#line 609 "smt2newparser.yy"
        { (yyval.str) = (yyvsp[0].str); }
#line 3070 "smt2newparser.cc"
    break;

  case 105: /* predef_key: KW_NOTES  */
#line 611 "smt2newparser.yy"
        { (yyval.str) = (yyvsp[0].str); }
#line 3076 "smt2newparser.cc"
    break;

  case 106: /* predef_key: KW_THEORIES  */
#line 613 "smt2newparser.yy"
        { (yyval.str) = (yyvsp[0].str); }
#line 3082 "smt2newparser.cc"
    break;

  case 107: /* predef_key: KW_EXTENSIONS  */
#line 615 "smt2newparser.yy"
        { (yyval.str) = (yyvsp[0].str); }
#line 3088 "smt2newparser.cc"
    break;

  case 108: /* predef_key: KW_PRINTSUCCESS  */
#line 617 "smt2newparser.yy"
        { (yyval.str) = (yyvsp[0].str); }
#line 3094 "smt2newparser.cc"
    break;

  case 109: /* predef_key: KW_EXPANDDEFINITIONS  */
#line 619 "smt2newparser.yy"
        { (yyval.str) = (yyvsp[0].str); }
#line 3100 "smt2newparser.cc"
    break;

  case 110: /* predef_key: KW_INTERACTIVEMODE  */
#line 621 "smt2newparser.yy"
        { (yyval.str) = (yyvsp[0].str); }
#line 3106 "smt2newparser.cc"
    break;

  case 111: /* predef_key: KW_PRODUCEPROOFS  */
#line 623 "smt2newparser.yy"
        { (yyval.str) = (yyvsp[0].str); }
#line 3112 "smt2newparser.cc"
    break;

  case 112: /* predef_key: KW_PRODUCEUNSATCORES  */
#line 625 "smt2newparser.yy"
        { (yyval.str) = (yyvsp[0].str); }
#line 3118 "smt2newparser.cc"
    break;

  case 113: /* predef_key: KW_MINIMALUNSATCORES  */
#line 627 "smt2newparser.yy"
        { (yyval.str) = (yyvsp[0].str); }
#line 3124 "smt2newparser.cc"
    break;

  case 114: /* predef_key: KW_PRINTCORESFULL  */
#line 629 "smt2newparser.yy"
        { (yyval.str) = (yyvsp[0].str); }
#line 3130 "smt2newparser.cc"
    break;

  case 115: /* predef_key: KW_PRODUCEMODELS  */
#line 631 "smt2newparser.yy"
        { (yyval.str) = (yyvsp[0].str); }
#line 3136 "smt2newparser.cc"
    break;

  case 116: /* predef_key: KW_PRODUCEASSIGNMENTS  */
#line 633 "smt2newparser.yy"
        { (yyval.str) = (yyvsp[0].str); }
#line 3142 "smt2newparser.cc"
    break;

  case 117: /* predef_key: KW_REGULAROUTPUTCHANNEL  */
#line 635 "smt2newparser.yy"
        { (yyval.str) = (yyvsp[0].str); }
#line 3148 "smt2newparser.cc"
    break;

  case 118: /* predef_key: KW_DIAGNOSTICOUTPUTCHANNEL  */
#line 637 "smt2newparser.yy"
        { (yyval.str) = (yyvsp[0].str); }
#line 3154 "smt2newparser.cc"
    break;

  case 119: /* predef_key: KW_RANDOMSEED  */
#line 639 "smt2newparser.yy"
        { (yyval.str) = (yyvsp[0].str); }
#line 3160 "smt2newparser.cc"
    break;

  case 120: /* predef_key: KW_VERBOSITY  */
#line 641 "smt2newparser.yy"
        { (yyval.str) = (yyvsp[0].str); }
#line 3166 "smt2newparser.cc"
    break;

  case 121: /* predef_key: KW_ERRORBEHAVIOR  */
#line 643 "smt2newparser.yy"
        { (yyval.str) = (yyvsp[0].str); }
#line 3172 "smt2newparser.cc"
    break;

  case 122: /* predef_key: KW_NAME  */
#line 645 "smt2newparser.yy"
        { (yyval.str) = (yyvsp[0].str); }
#line 3178 "smt2newparser.cc"
    break;

  case 123: /* predef_key: KW_NAMED  */
#line 647 "smt2newparser.yy"
        { (yyval.str) = (yyvsp[0].str); }
#line 3184 "smt2newparser.cc"
    break;

  case 124: /* predef_key: KW_AUTHORS  */
#line 649 "smt2newparser.yy"
        { (yyval.str) = (yyvsp[0].str); }
#line 3190 "smt2newparser.cc"
    break;

  case 125: /* predef_key: KW_VERSION  */
#line 651 "smt2newparser.yy"
        { (yyval.str) = (yyvsp[0].str); }
#line 3196 "smt2newparser.cc"
    break;

  case 126: /* predef_key: KW_STATUS  */
#line 653 "smt2newparser.yy"
        { (yyval.str) = (yyvsp[0].str); }
#line 3202 "smt2newparser.cc"
    break;

  case 127: /* predef_key: KW_REASONUNKNOWN  */
#line 655 "smt2newparser.yy"
        { (yyval.str) = (yyvsp[0].str); }
#line 3208 "smt2newparser.cc"
    break;

  case 128: /* predef_key: KW_ALLSTATISTICS  */
#line 657 "smt2newparser.yy"
        { (yyval.str) = (yyvsp[0].str); }
#line 3214 "smt2newparser.cc"
    break;

  case 129: /* info_flag: KW_ERRORBEHAVIOR  */
#line 661 "smt2newparser.yy"
        { (yyval.snode) = new ASTNode(INFO_T, (yyvsp[0].str)); }
#line 3220 "smt2newparser.cc"
    break;

  case 130: /* info_flag: KW_NAME  */
#line 663 "smt2newparser.yy"
        { (yyval.snode) = new ASTNode(INFO_T, (yyvsp[0].str)); }
#line 3226 "smt2newparser.cc"
    break;

  case 131: /* info_flag: KW_AUTHORS  */
#line 665 "smt2newparser.yy"
        { (yyval.snode) = new ASTNode(INFO_T, (yyvsp[0].str)); }
#line 3232 "smt2newparser.cc"
    break;

  case 132: /* info_flag: KW_VERSION  */
#line 667 "smt2newparser.yy"
        { (yyval.snode) = new ASTNode(INFO_T, (yyvsp[0].str)); }
#line 3238 "smt2newparser.cc"
    break;

  case 133: /* info_flag: KW_STATUS  */
#line 669 "smt2newparser.yy"
        { (yyval.snode) = new ASTNode(INFO_T, (yyvsp[0].str)); }
#line 3244 "smt2newparser.cc"
    break;

  case 134: /* info_flag: KW_REASONUNKNOWN  */
#line 671 "smt2newparser.yy"
        { (yyval.snode) = new ASTNode(INFO_T, (yyvsp[0].str)); }
#line 3250 "smt2newparser.cc"
    break;

  case 135: /* info_flag: KW_ALLSTATISTICS  */
#line 673 "smt2newparser.yy"
        { (yyval.snode) = new ASTNode(INFO_T, (yyvsp[0].str)); }
#line 3256 "smt2newparser.cc"
    break;

  case 136: /* info_flag: TK_KEY  */
#line 675 "smt2newparser.yy"
        {
            (yyval.snode) = new ASTNode(INFO_T, NULL);
            (yyval.snode)->children = new std::vector<ASTNode*>();
            (yyval.snode)->children->push_back(new ASTNode(GATTR_T, (yyvsp[0].str)));
        }
#line 3266 "smt2newparser.cc"
    break;


#line 3270 "smt2newparser.cc"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;
  *++yylsp = yyloc;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      {
        yypcontext_t yyctx
          = {yyssp, yytoken, &yylloc};
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = yysyntax_error (&yymsg_alloc, &yymsg, &yyctx);
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == -1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = YY_CAST (char *,
                             YYSTACK_ALLOC (YY_CAST (YYSIZE_T, yymsg_alloc)));
            if (yymsg)
              {
                yysyntax_error_status
                  = yysyntax_error (&yymsg_alloc, &yymsg, &yyctx);
                yymsgp = yymsg;
              }
            else
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = YYENOMEM;
              }
          }
        yyerror (&yylloc, context, yymsgp);
        if (yysyntax_error_status == YYENOMEM)
          YYNOMEM;
      }
    }

  yyerror_range[1] = yylloc;
  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval, &yylloc, context);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;

      yyerror_range[1] = *yylsp;
      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp, yylsp, context);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  yyerror_range[2] = yylloc;
  ++yylsp;
  YYLLOC_DEFAULT (*yylsp, yyerror_range, 2);

  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (&yylloc, context, YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, &yylloc, context);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp, yylsp, context);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
  return yyresult;
}

#line 682 "smt2newparser.yy"


//=======================================================================================
// Auxiliary Routines
