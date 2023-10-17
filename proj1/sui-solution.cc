#include "search-strategies.h"

std::vector<SearchAction> BreadthFirstSearch::solve(const SearchState &init_state) {

		std::vector<std::pair<SearchState, std::vector<SearchAction>>> open;
		std::vector<SearchState> close;
		SearchState working_state(init_state);
		std::vector<SearchAction> empty;

		open.push_back(std::make_pair(working_state, empty));
		
		while(!open.empty())
		{
			std::pair<SearchState, std::vector<SearchAction>> stateAndPath = open.front();
			open.erase(open.begin());
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
