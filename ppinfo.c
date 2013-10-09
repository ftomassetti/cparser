/*
 * This file is part of cparser.
 * Copyright (C) 2012 Matthias Braun <matze@braunis.de>
 */
#define _GNU_SOURCE

#define TARGET_INCLUDE_DIR NULL
#define PREPROCESSOR "cpp -U__STRICT_ANSI__"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <assert.h>

#ifdef _WIN32

#include <fcntl.h>
#include <io.h>

/* no eXecute on Win32 */
#define X_OK 0
#define W_OK 2
#define R_OK 4

#define O_RDWR          _O_RDWR
#define O_CREAT         _O_CREAT
#define O_EXCL          _O_EXCL
#define O_BINARY        _O_BINARY

/* remap some names, we are not in the POSIX world */
#define access(fname, mode)      _access(fname, mode)
#define mktemp(tmpl)             _mktemp(tmpl)
#define open(fname, oflag, mode) _open(fname, oflag, mode)
#define fdopen(fd, mode)         _fdopen(fd, mode)
#define popen(cmd, mode)         _popen(cmd, mode)
#define pclose(file)             _pclose(file)
#define unlink(filename)         _unlink(filename)
#define isatty(fd)               _isatty(fd)

#else
#include <unistd.h>
#define HAVE_MKSTEMP
#endif

#include <libfirm/firm.h>
#include <libfirm/be.h>
#include <libfirm/statev.h>

#include <revision.h>
#include "adt/util.h"
#include "ast_t.h"
#include "preprocessor.h"
#include "token_t.h"
#include "types.h"
#include "type_hash.h"
#include "parser.h"
#include "type_t.h"
#include "ast2firm.h"
#include "diagnostic.h"
#include "lang_features.h"
#include "driver/firm_opt.h"
#include "driver/firm_timing.h"
#include "driver/firm_machine.h"
#include "adt/error.h"
#include "adt/strutil.h"
#include "adt/array.h"
#include "symbol_table.h"
#include "wrappergen/write_fluffy.h"
#include "wrappergen/write_jna.h"
#include "wrappergen/write_compoundsizes.h"
#include "warning.h"
#include "help.h"
#include "mangle.h"
#include "printer.h"

unsigned int        c_mode                    = _C89 | _C99 | _GNUC;
bool                byte_order_big_endian     = false;
bool                strict_mode               = false;
bool                freestanding              = false;

static const char  *compiler_include_dir      = COMPILER_INCLUDE_DIR;
static const char  *local_include_dir         = LOCAL_INCLUDE_DIR;
static const char  *target_include_dir        = TARGET_INCLUDE_DIR;
static const char  *system_include_dir        = SYSTEM_INCLUDE_DIR;

static bool               char_is_signed      = true;
static atomic_type_kind_t wchar_atomic_kind   = ATOMIC_TYPE_INT;
static const char        *user_label_prefix   = "";
static unsigned           features_on         = 0;
static unsigned           features_off        = 0;
static const char        *dumpfunction        = NULL;
static struct obstack     file_obst;
static const char        *external_preprocessor = PREPROCESSOR;

static machine_triple_t *target_machine;
static const char       *target_triple;
static int               verbose;
static struct obstack    cppflags_obst;
static struct obstack    ldflags_obst;
static struct obstack    asflags_obst;
static struct obstack    codegenflags_obst;
static const char       *asflags;
static const char       *outname;
static bool              define_intmax_types;
static bool              construct_dep_target;
static bool              dump_defines;

unsigned            architecture_modulo_shift = 0;
bool                enable_main_collect2_hack = false;

int main(int argc, char* argv[])
{
	printf("Parto senza bombare...\n");
	gen_firm_init();
	init_ident();
	init_mode();
	init_preprocessor();
	exit_preprocessor();
	print_defines();
	printf("...e termino senza bombare!\n");
}