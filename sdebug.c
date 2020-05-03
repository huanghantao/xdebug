#include "php_xdebug.h"

ZEND_EXTERN_MODULE_GLOBALS(xdebug);

static zend_function *_get_cid = NULL;

user_opcode_handler_t ori_exit_handler;

void function_stack_entry_dtor(void *dummy, void *elem);

void sdebug_init()
{
	if (zend_hash_str_find_ptr(&module_registry, ZEND_STRL("swoole")))
	{
		zend_string *classname = zend_string_init(ZEND_STRL("Swoole\\Coroutine"), 0);
		zend_class_entry *ce = zend_lookup_class(classname);
		zend_string_release(classname);

		_get_cid = zend_hash_str_find_ptr(&(ce->function_table), ZEND_STRL("getcid"));
	}
}

long get_cid()
{
	zval retval;

	if (UNEXPECTED(!_get_cid))
	{
		return 0;
	}

	_get_cid->internal_function.handler(NULL, &retval);

	return Z_LVAL(retval) == -1 ? 0 : Z_LVAL(retval);
}

int add_current_context()
{
	long cid = get_cid();
	zval pData;
	struct zend_xdebug_context *context;

	if (zend_hash_index_exists(&XG(contexts), cid))
	{
		return 0;
	}

	context = emalloc(sizeof(struct zend_xdebug_context));
	context->cid = cid;
	context->level = 0;
	context->stack = xdebug_llist_alloc(function_stack_entry_dtor);
	context->prev_memory = 0;
	context->paths_stack = xdebug_path_info_ctor();
	context->branches.size = 0;
	context->branches.last_branch_nr = NULL;

	ZVAL_PTR(&pData, context);
	zend_hash_index_add(&XG(contexts), cid, &pData);

	return 1;
}

struct zend_xdebug_context *get_current_context()
{
	zval *val = zend_hash_index_find(&XG(contexts), get_cid());

	return val ? (struct zend_xdebug_context *) Z_PTR_P(val) : NULL;
}

void remove_current_context()
{
	long cid = get_cid();
	struct zend_xdebug_context *context;
	zval *pData = zend_hash_index_find(&XG(contexts), cid);

	context = (struct zend_xdebug_context *)Z_PTR_P(pData);

	xdebug_llist_destroy(context->stack, NULL);
	context->stack = NULL;
	context->level = 0;

	/* Clean up path coverage array */
	if (context->paths_stack) {
		xdebug_path_info_dtor(context->paths_stack);
		context->paths_stack = NULL;
	}
	if (context->branches.last_branch_nr) {
		free(context->branches.last_branch_nr);
		context->branches.last_branch_nr = NULL;
		context->branches.size = 0;
	}

	efree(context);

	zend_hash_index_del(&XG(contexts), cid);
}