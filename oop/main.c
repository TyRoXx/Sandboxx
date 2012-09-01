#include <stdlib.h>
#include <stdio.h>

#define INTERFACE_BEGIN(name) \
struct name ## type \
{ \
	void (*destroy)(struct name *);

#define EXTEND(base) \
	struct base ## type _base_;

#define INTERFACE_METHOD(result, name, ...) \
	result (*name)(void *, __VA_ARGS__);

#define INTERFACE_END(name) \
} name ## vtable; \
typedef struct name { \
struct name ## type *_class_; \
} name;

#define CLASS_BEGIN(name, base) \
struct name ## type \
{ \
	EXTEND(base)

#define CLASS_METHOD INTERFACE_METHOD

#define CLASS_END(name, elements) \
} name ## vtable; \
typedef struct name \
{ \
struct name ## type *_class_; \
elements elems; \
} name;

#define DEFINE_METHOD(class, base_, name, func) \
	(class ## vtable) . name = func;

#define DEFINE_VIRTUAL(class, base_, name, func) \
	((struct base_ ## type *) & class ## vtable) -> name = func;

#define CONSTRUCT(class, this, ...) \
	do { \
	(this)._class_ = &(class ## vtable); \
	((this)._class_->construct)(&(this), __VA_ARGS__); \
	} while (0)

#define DESTROY(interface, this) \
	((struct interface ## type *)((this)._class_))-> \
		destroy((struct interface *)&(this))

#define CALL_METHOD(obj, method, ...) \
	((obj)._class_->method)(&obj, __VA_ARGS__)

#define CALL_VIRTUAL(interface, obj, method, ...) \
	((struct interface ## type *)((obj)._class_))->method(&obj, __VA_ARGS__)

#define EXACT_CAST(target, this) \
	(( (struct target ## type *)((this)->_class_) == &(target ## vtable)) ? (struct target *)(this) : 0)


INTERFACE_BEGIN(Shape)
	INTERFACE_METHOD(double, getArea, )
INTERFACE_END(Shape)

typedef struct
{
	double x, y, radius;
} CircleElements;

CLASS_BEGIN(Circle, Shape)
	CLASS_METHOD(void, construct, double, double, double)
CLASS_END(Circle, CircleElements)

static void circle_construct(Circle *this, double x, double y, double radius)
{
	this->elems.x = x;
	this->elems.y = y;
	this->elems.radius = radius;
}

static void circle_destroy(Shape *this)
{
	Circle *c = EXACT_CAST(Circle, this);
	printf("dtor %f %f %f\n", c->elems.x, c->elems.y, c->elems.radius);
}

static double getArea(Circle *this)
{
	return this->elems.x * this->elems.y;
}

int main(void)
{
	DEFINE_METHOD(Circle, Shape, construct, circle_construct)
	DEFINE_VIRTUAL(Circle, Shape, destroy, circle_destroy)
	DEFINE_VIRTUAL(Circle, Shape, getArea, getArea)

	{
		Circle c;
		double area;

		CONSTRUCT(Circle, c, 10, 20, 30);
		area = CALL_VIRTUAL(Shape, c, getArea, );
		printf("Area: %f\n", area);
		DESTROY(Shape, c);
	}
	
	return 0;
}
