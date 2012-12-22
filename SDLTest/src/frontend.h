#ifndef FRONTEND_H
#define FRONTEND_H


struct FrontendType;


typedef struct Frontend
{
	struct FrontendType const *type;
}
Frontend;


typedef struct FrontendType
{
	void (*destroy)(Frontend *);
	void (*main_loop)(Frontend *);
}
FrontendType;


#endif
