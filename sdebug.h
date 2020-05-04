#include "config.h"
#if HAVE_SWOOLE
#ifndef SDEBUG_H
#define SDEBUG_H

extern user_opcode_handler_t ori_exit_handler;

void sdebug_init();
void remove_current_context();
int add_current_context();
struct zend_xdebug_context *get_current_context();

#define GET_CUR_CONTEXT_BEGIN struct zend_xdebug_context *_context;
#define GET_CUR_CONTEXT_END _context = get_current_context();
#define CUR_CONTEXT _context

#define ADD_CUR_CONTEXT add_current_context()
#define REMOVE_CUR_CONTEXT remove_current_context()

#define CUR_XG(v) (_context->v)

#endif
#endif