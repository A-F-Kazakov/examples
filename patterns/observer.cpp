#include <iostream>
#include <string>
#include <list>

using namespace std;

struct Observer
{
	virtual ~Observer() = default;
	virtual void update(string message_from_subject) = 0;
};

struct Subject
{
	virtual ~Subject() = default;
	virtual void attach(Observer *observer) = 0;
	virtual void detach(Observer *observer) = 0;
	virtual void notify() = 0;
};

struct ConcreteSubject : Subject
{
	virtual ~ConcreteSubject() { cout << "Goodbye, I was the Subject.\n"; }

	void attach(Observer *observer) override { list_observer_.push_back(observer); }
	void detach(Observer *observer) override { list_observer_.remove(observer); }
	void notify() override
	{
		howManyObserver();
		for(const auto it : list_observer_)
			it->update(message_);
	}

	void createMessage(string message = "Empty")
	{
		message_ = move(message);
		notify();
	}

	void howManyObserver() { cout << "There are " << list_observer_.size() << " observers in the list.\n"; }

	void someBusinessLogic()
	{
		message_ = "change message message";
		notify();
		cout << "I'm about to do some thing important\n";
	}

	private:
		list<Observer*> list_observer_;
		string message_;
};

struct ConcreteObserver : Observer
{
	ConcreteObserver(Subject &subject) : subject_(subject)
	{
		subject_.attach(this);
		cout << "Hi, I'm the Observer \"" << ++static_number_ << "\".\n";
		number_ = static_number_;
	}

	virtual ~ConcreteObserver() { cout << "Goodbye, I was the Observer \"" << number_ << "\".\n"; }

	void update(string message_from_subject) override
	{
		message_from_subject_ = move(message_from_subject);
		printInfo();
	}

	void removeMeFromTheList()
	{
		subject_.detach(this);
		cout << "Observer \"" << number_ << "\" removed from the list.\n";
	}

	void printInfo() { cout << "Observer \"" << number_ << "\": a new message is available --> " << message_from_subject_ << "\n"; }

	private:
		static int static_number_;

		string message_from_subject_;
		Subject &subject_;
		int number_;
};

int ConcreteObserver::static_number_{};

int main()
{
	ConcreteSubject subject;
	ConcreteObserver observer1{subject};
	ConcreteObserver observer2{subject};
	ConcreteObserver observer3{subject};

	subject.createMessage("Hello World!");
	observer3.removeMeFromTheList();

	subject.createMessage("The weather is hot today!");
	ConcreteObserver observer4{subject};

	observer2.removeMeFromTheList();
	ConcreteObserver observer5{subject};

	subject.createMessage("My new car is great!");
	observer5.removeMeFromTheList();

	observer4.removeMeFromTheList();
	observer1.removeMeFromTheList();
}
