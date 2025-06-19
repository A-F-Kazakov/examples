#include <iostream>
#include <string>
#include <string_view>
#include <memory>

using namespace std;

struct Command
{
	virtual ~Command() = default;
	virtual void execute() const = 0;
};

struct SimpleCommand : Command
{
	explicit SimpleCommand(string pay_load) : pay_load_(move(pay_load)) {}
	void execute() const override { cout << "SimpleCommand: See, I can do simple things like printing (" << pay_load_ << ")\n"; }

	private:
		string pay_load_;
};

struct Receiver
{
	void doSomething(string_view a) { cout << "Receiver: Working on (" << a << ".)\n"; }
	void doSomethingElse(string_view b) { cout << "Receiver: Also working on (" << b << ".)\n"; }
};

struct ComplexCommand : Command
{
	ComplexCommand(unique_ptr<Receiver>&& receiver, string a, string b) :
		receiver_(move(receiver)), a_(move(a)), b_(move(b))
	{}

	void execute() const override
	{
		cout << "ComplexCommand: Complex stuff should be done by a receiver object.\n";
		receiver_->doSomething(a_);
		receiver_->doSomethingElse(b_);
	}

	private:
		unique_ptr<Receiver> receiver_;
		string a_;
		string b_;
};

struct Invoker
{
	void setOnStart(unique_ptr<Command> command) { swap(on_start_, command); }
	void setOnFinish(unique_ptr<Command> command) { swap(on_finish_, command); }

	void doSomethingImportant()
	{
		cout << "Invoker: Does anybody want something done before I begin?\n";
		if(on_start_)
			on_start_->execute();

		cout << "Invoker: ...doing something really important...\n";
		cout << "Invoker: Does anybody want something done after I finish?\n";
		if(on_finish_)
			on_finish_->execute();
	}

	private:
		unique_ptr<Command> on_start_;
		unique_ptr<Command> on_finish_;
};

int main()
{
	Invoker invoker;
	invoker.setOnStart(make_unique<SimpleCommand>("Say Hi!"));

	invoker.setOnFinish(make_unique<ComplexCommand>(make_unique<Receiver>(), "Send email", "Save report"));
	invoker.doSomethingImportant();
}
