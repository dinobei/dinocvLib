#include "../common/def_os_selector_export.h"
#include "queue.h"

QUEUE_T* queue_malloc()
{
	QUEUE_T* retv = (QUEUE_T*)malloc(sizeof(QUEUE_T));
	memset(retv, 0, sizeof(QUEUE_T));
	if(!retv) return NULL;
	return retv;
}

bool_d queue_put(QUEUE_T* qu, void *data)
{
	if(!qu || !data) return d_false;
	return list_add_head(qu, data);
}

// 노드는 삭제하고 데이터 주소만 넘겨줌
void *queue_get(QUEUE_T* qu)
{
	void *retv;
	if(!qu) return NULL;

	retv = list_get_tail_data(qu);
	free(list_del_tail(qu));

	return retv;
}

void queue_free(QUEUE_T* qu)
{
	if(!qu) return;
	list_free(qu);
}
