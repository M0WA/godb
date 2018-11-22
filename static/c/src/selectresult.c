#include "selectresult.h"

#include <stdlib.h>

#include "helper.h"
#include "table.h"


	/*
int dump_selectresult(const SelectResult *res, char** buf) {
	*buf = 0;
	for(size_t i = 0; i < res->nrows; i++) {
		const void *row = get_row_selectresult(res,i);
		if(!row) {
			return 1; }
		for(size_t j = 0; j < res->ncols; j++) {
			void *colbuf = get_columnbuf_from_row(def,j,row);
			if(!colbuf) {
				return 1; }
			switch(res->cols[j].type) {

			}
		}
		if(append_string("\n")) {
			return 1; }
	}
	return 1;
}
*/

int destroy_selectresult(struct _SelectResult *res) {
	if(res) {
		if(res->row) {
			for(size_t i = 0; i < res->ncols; i++) {
				if(res->row[i]) {
					free(res->row[i]);
					res->row[i] = 0;
				}
			}
			free(res->row);
		}
		res->row = 0;
	}
	return 0;
}
