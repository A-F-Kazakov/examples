#include <iostream>
#include <string>

using namespace std;

struct Component
{
	virtual ~Component() = default;
	virtual string operation() const = 0;
};

struct ConcreteComponent : Component
{
	string operation() const override { return "ConcreteComponent"; }
};

struct Decorator : Component
{
	Decorator(Component* component) : component_(component) {}

	string operation() const override { return component_->operation(); }

	protected:
		Component* component_;
};

struct ConcreteDecoratorA : Decorator
{
	ConcreteDecoratorA(Component* component) : Decorator(component) {}
	string operation() const override { return "ConcreteDecoratorA(" + Decorator::operation() + ")"; }
};

struct ConcreteDecoratorB : Decorator
{
	ConcreteDecoratorB(Component* component) : Decorator(component) {}
	string operation() const override { return "ConcreteDecoratorB(" + Decorator::operation() + ")"; }
};

void print(const Component& component)
{
	cout << "RESULT: " << component.operation();
	cout << "\n\n";
}

int main()
{
	ConcreteComponent simple;
	cout << "Client: I've got a simple component:\n";
	print(simple);

	ConcreteDecoratorA decorator1(&simple);
	cout << "Client: Now I've got a decorated component:\n";
	print(decorator1);

	ConcreteDecoratorB decorator2(&decorator1);
	cout << "Client: I can even decorate decorator:\n";
	print(decorator2);
}
