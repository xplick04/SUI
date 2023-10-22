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
#include <iostream>
#include <sstream>
#include <string>

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


void getCardValue(std::string card, int& value, int& color)
{
	int i = 1;
	if(card[0] == 'K') value = 13;
	else if(card[0] == 'Q') value = 12;
	else if(card[0] == 'J') value = 11;
	else if(card[0] == '1' && card[1] == '0') 
	{
		value = 10;
		i = 2;
	}
	else value = card[0] - '0'; ;

	if(card[i] == 'h' || card[i] == 'd') color = 0;
	else color = 1;
}



double StudentHeuristic::distanceLowerBound(const GameState &state) const {
    int cards_out_of_home = king_value * colors_list.size();

	int cards_in_home = 0;
    for (const auto &home : state.homes) {
        auto opt_top = home.topCard();
        if (opt_top.has_value())
            cards_in_home += opt_top->value;
    }

	int freeCells_occupied = 0;
	for (const auto &FreeCell : state.free_cells) {
        auto opt_top = FreeCell.topCard();
        if (opt_top.has_value())
            freeCells_occupied += 1;
    }

	
	int cards_in_wrong_order = 0;
	for (const auto &stack : state.stacks) {
		std::ostringstream outputStream;
		outputStream << stack;
		std::istringstream inputStream(outputStream.str());
		std::string item;
		inputStream >> item;
		if(item != "_")
		{
			int val, col, prevVal, prevCol;
			getCardValue(item, prevVal, prevCol);
			while(inputStream >> item)
			{
				getCardValue(item, val, col);
				if(!(prevVal - val == 1 && prevCol == col)) cards_in_wrong_order++;
				
				prevVal = val;
				prevCol = col;
			}
		}
    }


	int lowest_card_distance = 0;
    for (const auto &stack : state.stacks) {
        std::ostringstream outputStream;
        outputStream << stack;
        std::istringstream inputStream(outputStream.str());
        std::string item;
        inputStream >> item;
        if(item != "")
        {
            int val, col;
            int min, minDistance = 14;
            while(inputStream >> item)
            {
                getCardValue(item, val, col);
                if(val < min)
                {
                    min = val;
                    minDistance = 0;
                }
                minDistance++;
            }
        lowest_card_distance += minDistance;
        }
    }
	
	return (cards_out_of_home - cards_in_home) + freeCells_occupied + (0.05 * cards_in_wrong_order) + (0.05 * lowest_card_distance);
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
