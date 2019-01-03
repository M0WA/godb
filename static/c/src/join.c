#include "join.h"

#include "dblimits.h"
#include "stringbuf.h"
#include "logger.h"
#include "table.h"
#include "column.h"

#include <string.h>

static int join_stmt_string(const struct _JoinStmt *join,struct _StringBuf *buf) {
	const char fmt[] = "%s JOIN %s ON %s.%s = %s.%s";
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

	if( stringbuf_appendf(buf,fmt,typestr,join->right->table,join->right->table,join->right->name,join->left->table,join->left->name) ) {
		return 1;
	}
	return 0;
}

int join_clause_string(const struct _JoinClause *join,struct _StringBuf *buf) {
	for(size_t i = 0; i < join->njoins; i++) {
		if( join_stmt_string(join->joins[i],buf) ) {
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
