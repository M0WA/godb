#include "join.h"

#include "dblimits.h"
#include "stringbuf.h"
#include "logger.h"
#include "table.h"
#include "column.h"

#include <string.h>

static int join_stmt_string(const struct _JoinStmt *join,const char *delimiter,struct _StringBuf *buf) {
	const char *del = delimiter ? delimiter : "";
	const char fmt[] = "%s JOIN %s%s%s ON %s%s%s.%s%s%s = %s%s%s.%s%s%s";
	const char *left = "LEFT", *right = "RIGHT", *inner = "INNER", *outer = "OUTER";
	const char *typestr = 0;

	switch(join->type) {
	case DBJOIN_TYPE_LEFT:
		typestr = left;
		break;
	case DBJOIN_TYPE_RIGHT:
		typestr = right;
		break;
	case DBJOIN_TYPE_INNER:
		typestr = inner;
		break;
	case DBJOIN_TYPE_OUTER:
		typestr = outer;
		break;
	default:
		return 1;
	}

	if( stringbuf_appendf(buf,fmt,typestr,
			del,join->right->table,del,
			del,join->right->table,del,del,join->right->name,
			del,del,join->left->table,del,del,join->left->name,del) ) {
		return 1;
	}
	return 0;
}

int join_clause_string(const struct _JoinClause *join,const char *delimiter,struct _StringBuf *buf) {
	for(size_t i = 0; i < join->njoins; i++) {
		if( join_stmt_string(join->joins[i],delimiter,buf) ) {
			return 1;
		}
	}
	return 0;
}

int join_append(struct _JoinClause *join, struct _JoinStmt *stmt) {
	if(join->joins) {
		join->joins = realloc(join->joins,sizeof(struct _JoinStmt) * (join->njoins + 1));
	} else {
		join->joins = malloc(sizeof(struct _JoinStmt));
	}
	if(!join->joins) {
		return 1;
	}
	join->joins[join->njoins] = stmt;
	join->njoins++;
	return 0;
}

void join_destroy(struct _JoinClause *join) {
	if(join->joins) {
		for(size_t i = 0; i < join->njoins; i++) {
			if(join->joins[i]) {
				free(join->joins[i]);
			}
		}
		free(join->joins);
		join->joins = 0;
		join->njoins = 0;
	}
}
