#include "search-interface.h"
#include "search-strategies.h"
#include <algorithm>
#include <cstddef>
#include <ostream>
#include <utility>
#include <queue>
#include <set>


class Path { 
  public: 
    SearchAction action; 
    Path *parent;  // needs to be ptr, reference cannot store NULL
	Path(SearchAction action, Path* parent = nullptr) : action(action), parent(parent) {}
};


std::vector<SearchAction> BreadthFirstSearch::solve(const SearchState &init_state) {
		std::queue<std::pair<SearchState, Path *>> open;
		std::set<SearchState> closed;

		open.push(std::make_pair(init_state, new Path(SearchAction(init_state.actions()[0]), nullptr)));  // first state
		
		while(!open.empty())
		{
			auto [currentState, pathToCurrent] = open.front();

			if(currentState.isFinal())
			{
				// NOTE: this branch reconstructs actions path from currentState to root
				std::vector<SearchAction> path;
				Path *action = pathToCurrent;

				do
				{
					path.push_back(action->action);
				} while((action = action->parent)->parent != nullptr);

				std::reverse(path.begin(), path.end());

				return path;
			}

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


std::vector<SearchAction> DepthFirstSearch::solve(const SearchState &init_state) {
	std::vector<std::pair<SearchState, std::vector<SearchAction>>> open;
		std::vector<SearchState> close;
		SearchState working_state(init_state);
		std::vector<SearchAction> empty;

		open.push_back(std::make_pair(working_state, empty));
		
		while(!open.empty())
		{
			std::pair<SearchState, std::vector<SearchAction>> stateAndPath = open.back();
			open.pop_back();
			close.push_back(stateAndPath.first);
			
			if(stateAndPath.first.isFinal()) 
			{
				return stateAndPath.second;
			}

			auto actions = stateAndPath.first.actions();
			if(actions.size() == 0) continue;

			for(auto &action: actions)
			{
				bool found = false;
				stateAndPath.second.push_back(action);
				for(auto &state: close)
				{
					if(state<stateAndPath.first) // equals checking
					{
						found = true;
						break;
					}
				}
				for(auto &state: open)
				{
					if(state.first<stateAndPath.first) // equals checking
					{
						found = true;
						break;
					}
				}
				if(!found)
				{
					if(action.execute(stateAndPath.first).isFinal())
					{
						return stateAndPath.second;
					}
					open.push_back(std::make_pair(action.execute(stateAndPath.first), stateAndPath.second));
				}
				stateAndPath.second.pop_back();
			}		
		}
	return {};
}

double StudentHeuristic::distanceLowerBound(const GameState &state) const {
    return 0;
}

std::vector<SearchAction> AStarSearch::solve(const SearchState &init_state) {
	return {};
}
