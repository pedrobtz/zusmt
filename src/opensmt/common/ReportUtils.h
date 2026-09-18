#include <r_compat.h>
/*
 *  Copyright (c) 2008-2012, Roberto Bruttomesso <roberto.bruttomesso@gmail.com>
 *  Copyright (c) 2012-2022, Antti Hyvarinen <antti.hyvarinen@gmail.com>
 *  Copyright (c) 2022, Martin Blicha <martin.blicha@gmail.com>
 *
 *  SPDX-License-Identifier: MIT
 *
 */

#ifndef OPENSMT_REPORTUTILS_H
#define OPENSMT_REPORTUTILS_H

#define opensmt_warning( S )      { zusmt::rerr() << "; Warning: " << S << std::endl; }
#define opensmt_warning2( S, T )  { cerr << "; Warning: " << S << " " << T << endl; }

#define reportf(format, ...) ( REprintf(format __VA_OPT__(,) __VA_ARGS__) )

#endif //OPENSMT_REPORTUTILS_H
