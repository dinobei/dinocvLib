#include "../common/def_os_selector_export.h"
#include "memmgr.h"

LIST_D *_memmgr; // global list (private)

MGRHWND create_memmgr(
	CFUNC cfunc,
	IFUNC ifunc,
	DFUNC dfunc,
	int const_cnt,
	int supplement_cnt)
{
	register int i;
	MEMMGR_D *hmem;

	// malloc at first
	if(!_memmgr)
		_memmgr = dinocv_list_malloc();

	// create & init memory list
	hmem = (MEMMGR_D *)malloc(sizeof(MEMMGR_D));
	memset(hmem, 0, sizeof(MEMMGR_D));
	hmem->qu = queue_malloc();
	hmem->cfunc = cfunc;
	hmem->ifunc = ifunc;
	hmem->dfunc = dfunc;
	hmem->const_cnt = const_cnt;
	hmem->supplement_cnt = supplement_cnt;

	for(i =0  ; i < const_cnt ; i++){
		queue_put(hmem->qu, cfunc());
	}
	
	// add to memory manager
	dinocv_list_add_tail(_memmgr, hmem);
	return _memmgr->cnt-1;
}

void *get_memmgr(MGRHWND handle)
{
	MEMMGR_D *hmem;
	int i, scnt;

	if(!_memmgr) return NULL;
	hmem = (MEMMGR_D *)dinocv_list_get_idx_data(_memmgr, handle);
	if(!hmem) return NULL;

	// supplement
	if(!hmem->qu->cnt){
		scnt = hmem->supplement_cnt;
		for(i = 0 ; i < scnt ; i++)
			queue_put(hmem->qu, hmem->cfunc());
	}

	return queue_get(hmem->qu);
}

bool_d put_memmgr(MGRHWND handle, void *mem_pt)
{
	MEMMGR_D *hmem;
	hmem = (MEMMGR_D *)dinocv_list_get_idx_data(_memmgr, handle);

	// call init callback function
	hmem->ifunc(mem_pt);

	queue_put(hmem->qu, mem_pt);
	return d_true;
}

bool_d delete_all_memmgr(bool_d is_force_free)
{
	MEMMGR_D *hmem;
	NODE_D *node;
	int i, cnt;

	if(!_memmgr)
		return d_false;
	cnt = _memmgr->cnt;

	for(i = 0 ; i < cnt ; i++){
		node = dinocv_list_del_head(_memmgr);
		hmem = (MEMMGR_D *)node->data;
		
		//queue_free(hmem->qu);
		while(hmem->qu->cnt)
		{
			hmem->dfunc(queue_get(hmem->qu));
			//NODE_T *node = (NODE_T *)queue_get(hmem->qu);
			//hmem->dfunc(node->data);
			//free(node);
		}

		queue_free(hmem->qu);

		free(hmem);
		free(node);
	}
	dinocv_list_free(_memmgr);
	_memmgr=NULL;

	return d_true;
}
