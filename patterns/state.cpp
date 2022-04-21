#include <iostream>
#include <memory>
#include <string>
#include <string_view>

using namespace std;

struct Material;

struct State
{
	virtual ~State() = default;

	void setContext(Material *context) { context_ = context; }

	virtual void melt() = 0;
	virtual void harden() = 0;
	virtual void evaporate() = 0;

	protected:
		Material *context_{};
};

struct Material
{
	Material(unique_ptr<State>&& state)
	{
		transitionTo(move(state));
	}

	void melt() { state_->melt(); }
	void harden() { state_->harden(); }
	void evaporate() { state_->evaporate(); }

	void transitionTo(unique_ptr<State>&& state)
	{
		cout << "Transition to " << typeid(*(state.get())).name() << ".\n";

		swap(state_, state);
		state_->setContext(this);
	}

	protected:
		unique_ptr<State> state_;
};

struct Gas : State
{
	void melt() override {}
	void harden() override;
	void evaporate() override {}
};

struct Liquid : State
{
	void melt() override {}
	void harden() override;
	void evaporate() override { context_->transitionTo(make_unique<Gas>()); }
};

struct Solid : State
{
	void melt() override { context_->transitionTo(make_unique<Liquid>()); }
	void harden() override {}
	void evaporate() override {}
};

void Gas::harden() { context_->transitionTo(make_unique<Liquid>()); }
void Liquid::harden() { context_->transitionTo(make_unique<Solid>()); }

int main()
{
	Material m{make_unique<Gas>()};

	m.evaporate();
	m.melt();
	m.harden();
	m.harden();
	m.melt();
	m.evaporate();
}
