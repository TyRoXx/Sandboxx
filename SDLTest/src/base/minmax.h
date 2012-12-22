#ifndef MINMAX_H
#define MINMAX_H


#define DEFINE_MAX_FUNCTION(type_) \
type_ max_ ## type_(type_ left, type_ right) \
{ \
	return (left > right) ? left : right; \
}

#define DEFINE_MIN_FUNCTION(type_) \
type_ min_ ## type_(type_ left, type_ right) \
{ \
	return (left < right) ? left : right; \
}


static DEFINE_MIN_FUNCTION(ptrdiff_t)
static DEFINE_MAX_FUNCTION(ptrdiff_t)


#endif
