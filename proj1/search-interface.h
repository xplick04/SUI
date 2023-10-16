#ifndef SEARCH_INTERFACE_H
#define SEARCH_INTERFACE_H

#include "move.h"
#include "game.h"

#include <ostream>

class SearchState;

class AStarHeuristicItf;

class SearchAction {
public:
	SearchAction(Location from, Location to) : from_(from), to_(to) {} ;
	SearchState execute(const SearchState& state) const ;

    friend std::ostream& operator<< (std::ostream& os, const SearchAction & action) ;

    const Location& from() const;
    const Location& to() const;
private:
	Location from_;
	Location to_;
};

class SearchState {
public:
    explicit SearchState(GameState state) : state_(state) {}

	bool isFinal() const;
	std::vector<SearchAction> actions() const;

	bool execute(const SearchAction &action);
    static unsigned long long nbExpanded();

    friend std::ostream& operator<< (std::ostream& os, const SearchState & state) ;
    friend bool operator<(const SearchState &a, const SearchState &b) ;
    friend bool operator==(const SearchState &a, const SearchState &b) ;
    friend double compute_heuristic(const SearchState &state, const AStarHeuristicItf &heuristic);
    friend size_t hash(const SearchState &state);

private:
	void runSafeMoves_();
	GameState state_;
    static unsigned long long nb_expanded;
};


class SearchStrategyItf {
public:
	virtual std::vector<SearchAction> solve(const SearchState &init_state) =0 ;
	virtual ~SearchStrategyItf() {}
};

#endif
