#include <iostream>
#include <string_view>

using namespace std;

struct BaseComponent;

struct Mediator
{
	virtual ~Mediator()	= default;
	virtual void notify(BaseComponent *sender, string_view event) const = 0;
};

struct BaseComponent
{
	BaseComponent(Mediator *mediator = nullptr) : mediator_(mediator) {}

  void setMediator(Mediator *mediator) { mediator_ = mediator; }

	protected:
		Mediator *mediator_;
};

struct Component1 : BaseComponent
{
	void doA()
	{
		cout << "Component 1 does A.\n";
		mediator_->notify(this, "A");
	}

	void doB()
	{
		cout << "Component 1 does B.\n";
		mediator_->notify(this, "B");
	}
};

struct Component2 : BaseComponent
{
	void doC()
	{
		cout << "Component 2 does C.\n";
		mediator_->notify(this, "C");
	}

	void doD()
	{
		cout << "Component 2 does D.\n";
		mediator_->notify(this, "D");
	}
};

struct ConcreteMediator : Mediator
{
	ConcreteMediator(Component1 *c1, Component2 *c2) : component1_(c1), component2_(c2)
	{
		component1_->setMediator(this);
		component2_->setMediator(this);
	}

	void notify(BaseComponent *sender, string_view event) const override
	{
		if (event == "A")
		{
			cout << "Mediator reacts on A and triggers following operations:\n";
			component2_->doC();
		}
		if (event == "D")
		{
			cout << "Mediator reacts on D and triggers following operations:\n";
			component1_->doB();
			component2_->doC();
		}
	}

	private:
		Component1 *component1_;
		Component2 *component2_;
};

int main()
{
	auto c1 = make_unique<Component1>();
	auto c2 = make_unique<Component2>();

	ConcreteMediator mediator{c1.get(), c2.get()};

	cout << "Client triggers operation A.\n";
	c1->doA();
	cout << "\n";
	cout << "Client triggers operation D.\n";
	c2->doD();
}
