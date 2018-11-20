#include "selectresult.h"

#include <stdlib.h>

int destroy_selectresult(struct _SelectResult* res) {
	if(res) {
		if(res->rows) {
			free(res->rows);
		}
		free(res);
	}
	return 0;
}
