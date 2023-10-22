#include "search-interface.h"
#include "search-strategies.h"
#include <algorithm>
#include <cstddef>
#include <functional>
#include <iterator>
#include <ostream>
#include <utility>
#include <queue>
#include <set>
#include <vector>


class Path { 
  public: 
    SearchAction action; 
    Path *parent;  // needs to be ptr, reference cannot store NULL
	Path(SearchAction action, Path* parent = nullptr) : action(action), parent(parent) {}
};

std::vector<SearchAction> ReconstructPath(Path *pathToCurrent)
{
	std::vector<SearchAction> path;
	Path *action = pathToCurrent;

	do
	{
		path.push_back(action->action);
	} while((action = action->parent)->parent != nullptr);

	std::reverse(path.begin(), path.end());

	return path;
}

std::vector<SearchAction> BreadthFirstSearch::solve(const SearchState &init_state) {
	std::queue<std::pair<SearchState, Path *>> open;
	std::set<SearchState> closed;

	open.push(std::make_pair(init_state, new Path(SearchAction(init_state.actions()[0]), nullptr)));  // first state
	
	while(!open.empty())
	{
		auto [currentState, pathToCurrent] = open.front();

		if(currentState.isFinal())
			return ReconstructPath(pathToCurrent);	

		open.pop();

		for(auto &action: currentState.actions())
		{
			auto nextState = action.execute(currentState);
			if (closed.count(nextState))
				continue;  // action already expanded => skip it

			closed.insert(nextState);  // OPTIM: save some description of state instead

			open.push(std::make_pair(nextState, new Path(action, pathToCurrent)));
		}	
	}
	return {};
}


int PathDepth(Path *pathToCurrent)
{
	int height = 0;
	Path *action = pathToCurrent;

	while (action->parent != nullptr)
	{
		height++;
	  	action = action->parent;
	}

  	return height;
}

std::vector<SearchAction> DepthFirstSearch::solve(const SearchState &init_state) {
	std::deque<std::pair<SearchState, Path *>> open;

	open.push_back(std::make_pair(init_state, new Path(init_state.actions()[0], nullptr)));
	
	while(!open.empty())
	{
		auto [currentState, pathToCurrent] = open.back();
		
		if(currentState.isFinal())
			return ReconstructPath(pathToCurrent);

		open.pop_back();

		if (PathDepth(pathToCurrent) == this->depth_limit_)
		  continue;

		for(auto &action : currentState.actions())
		{
			auto nextState = action.execute(currentState);

			open.push_back(std::make_pair(nextState, new Path(action, pathToCurrent)));
			if (nextState.isFinal())
				break;
		}		
	}
	return {};
}

double StudentHeuristic::distanceLowerBound(const GameState &state) const {
    int cards_out_of_home = king_value * colors_list.size();
    for (const auto &home : state.homes) {
        auto opt_top = home.topCard();
        if (opt_top.has_value())
            cards_out_of_home -= opt_top->value;
    }

    return cards_out_of_home;
}


class State{ 
  public: 
    SearchState state;
    SearchAction action;  // PERF: reoder structure elements due to padding
    unsigned int score;
    State *parent;

	State(SearchState state, SearchAction action, double score, State *parent = nullptr) : state(state), action(action), score(score), parent(parent) {}
};

struct AStarComparator {
    bool operator() (State *a, State *b) {
        return a->score > b->score;
    }
};

std::vector<SearchAction> AStarSearch::solve(const SearchState &init_state) {
  	std::priority_queue<State *, std::deque<State *>, AStarComparator> open;  // PERF: best subtype????
	std::set<SearchState> closed;

	open.push(new State(init_state, init_state.actions()[0], compute_heuristic(init_state, *heuristic_), nullptr));

	while (!open.empty())
	{
		auto current = open.top();
		open.pop();

		if (closed.count(current->state))
			continue;
		
		closed.insert(current->state);


		if (current->state.isFinal())
		{
	  	  	std::vector<SearchAction> path;
	  	  	State *action = current;

	  	  	do
	  	  	{
		  	  	path.push_back(action->action);
	  	  	} while((action = action->parent)->parent != nullptr);

	  	  	std::reverse(path.begin(), path.end());

	  	  	return path;
		}

		for (auto &action : current->state.actions())
		{
			SearchState nextState = action.execute(current->state);
			if (closed.count(nextState))
				continue;
			unsigned int score = compute_heuristic(nextState, *heuristic_) + current->score;

			open.push(new State(nextState, action, score, current));
		}

	}

	return {};
}
