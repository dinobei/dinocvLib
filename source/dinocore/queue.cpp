#include "../common/def_os_selector_export.h"
#include "queue.h"

QUEUE_D* queue_malloc()
{
	QUEUE_D* retv = (QUEUE_D*)malloc(sizeof(QUEUE_D));
	memset(retv, 0, sizeof(QUEUE_D));
	if(!retv) return NULL;
	return retv;
}

bool_d queue_put(QUEUE_D* qu, void *data)
{
	if(!qu || !data) return d_false;
	return dinocv_list_add_head(qu, data);
}

// 노드는 삭제하고 데이터 주소만 넘겨줌
void *queue_get(QUEUE_D* qu)
{
	void *retv;
	if(!qu) return NULL;

	retv = dinocv_list_get_tail_data(qu);
	free(dinocv_list_del_tail(qu));

	return retv;
}

void queue_free(QUEUE_D* qu)
{
	if(!qu) return;
	dinocv_list_free(qu);
}
