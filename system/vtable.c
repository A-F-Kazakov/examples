#include <stdio.h>

typedef void (*virtual_function)(void*);

typedef struct
{
	virtual_function foo;
	virtual_function bar;
} vtable;

typedef struct
{
	vtable* vtable;
	int base_data;
} base;

void base_foo(void* self)
{
	base* this = (base*)self;
	printf("Base::foo(), base_data = %d\n", this->base_data);
}

void base_bar(void* self)
{
	(void)(self);
	//base* this = (base*)self;
	printf("Base::bar()\n");
}

vtable base_vtable = {.foo = base_foo, .bar = base_bar};

void base_init(base* self, int data)
{
	self->vtable	 = &base_vtable;
	self->base_data = data;
}

//

typedef struct
{
	base base;
	int derived_data;
} derived;

void derived_foo(void* self)
{
	derived* this = (derived*)self;
	printf("Derived::foo(), base_data = %d, derived_data = %d\n", this->base.base_data, this->derived_data);
}

vtable derived_vtable = { .foo = derived_foo, .bar = base_bar };

void derived_init(derived* self, int base_data, int derived_data)
{
	base_init((base*)self, base_data);
	self->base.vtable	 = &derived_vtable;
	self->derived_data = derived_data;
}

int main()
{
    base b;
    base_init(&b, 10);
    
    derived d;
    derived_init(&d, 20, 30);
    
    base* objects[] = {&b, (base*)&d};
    
    for (int i = 0; i < 2; i++)
	{
        objects[i]->vtable->foo(objects[i]);
        objects[i]->vtable->bar(objects[i]);
    }
    
    return 0;
}
