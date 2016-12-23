// cppFunjs.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <sstream>

#include <boost/optional/optional.hpp>

enum bw { Black = 0, White = 1 };

static bool sunnySide()
{
	return false;
}

template<typename T>
struct add_optionality_t
{
	using type = boost::optional<T>;
};

template<typename T>
struct add_optionality_t <boost::optional<T>>
{
	using type = boost::optional<T>;
};

template<typename T>
using add_optionality = typename add_optionality_t<T>::type;

template<typename T, typename L>
auto operator | (boost::optional<T> const& opt, L const& withFun) -> add_optionality<decltype(withFun(*opt))>
{
	if (opt)
		return withFun(*opt);
	else
		return{};
}

template<typename T, typename L>
T value_or(boost::optional<T> const& opt, L const& orFun)
{
	if (opt)
		return *opt;
	else
		return orFun();
}

// different versions of with() as concept is developed

//template<typename T, typename L>
//void with(boost::optional<T> const& opt, L const& withFun)
//{
//	if (opt)
//		return withFun(*opt);
//}

//template<typename T, typename L>
//auto with(boost::optional<T> const& opt, L const& withFun) -> boost::optional<T>
//{
//	if (opt)
//		return withFun(*opt);
//	else
//		return{};
//}

template<typename T, typename L>
auto with(boost::optional<T> const& opt, L const& withFun) -> add_optionality<decltype(withFun(*opt))>
{
	if (opt)
		return withFun(*opt);
	else
		return {};
}

// display value of optional if it exists
template<typename T>
void displayOptional(boost::optional<T> const& opt) {
	if (opt)
		std::cout << *opt << std::endl;
}

int main()
{
	// examples to discuss principle of immutable values
	const auto px = sunnySide();
	const auto px2 =
		(sunnySide() == true)
		? bw::White
		: bw::Black;

	// optional value, used as equivalent of Maybe type
	boost::optional<int> oi;

	// no output here as optional has no value
	if (oi)
		std::cout << *oi << std::endl;

	// basic default value for optional
	auto z = oi.value_or(7);

	// default value from fixed function
	auto z2 = value_or(oi, [] {return 7;});

	//with(oi, [](int i) {std::cout << i * 2 << std::endl;});

	// optional value created from optional as param
	auto j = with(oi, [](int i) {return i * 2;});

	displayOptional(j);

	// set value of optional var
	oi = 2;

	// optional value of different type to param
	auto s = with(oi, [](int i)
	{
		std::ostringstream oss;
		oss << i;
		return oss.str();
	});

	displayOptional(s);

	// chaining by use of nested with statements
	auto k =
		with(
			with(oi, [](int x) 
				{
					return x * 2;
				}),
			[](int x) 
				{
					return x + 1;
				}
		);

	displayOptional(k);

	// refactor chaining with operator syntactic sugar
	auto k2 = oi
		| [](int x) { 
						return x * 2; 
					}
		| [](int x) { 
						return x + 1; 
					};

	displayOptional(k2);

	// if this line is uncommented then k3 is initialised
	 //oi = 6;

	// exit chaining by returning uninitialised optional dependent on param
	auto k3 = oi
		| [](int x) { 
						return x * 2; 
					}
		| [](int x) { 
						// depending on condition either pass value of
						// onto next function in chain or return
						// uninitialised optional which then cascades directly 
						// to the end, as with the use of Maybe in an FP language
						return (x>10) ? x : boost::optional<int>(); 
					}
		| [](int x) { 
						return x + 1; 
					}
		| [](int x) { 
						std::ostringstream oss; oss << x; return oss.str(); 
					};

	displayOptional(k3);
}

