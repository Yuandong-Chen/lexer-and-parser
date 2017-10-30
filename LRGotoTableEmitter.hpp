#pragma once
#include <map>
#include "GrammarState.hpp"

namespace miniparser {

    class LRGotoTableEmitter
    {
    public:
    	std::map<int, std::map<int, std::tuple<int, int, int>>> gototable;
    public:
    	LRGotoTableEmitter() = default;

    	~LRGotoTableEmitter() = default;

        void crunchRules(std::map<int, Symbols>& symp) {
            miniparser::FirstSet first(symp);
            first.runFirstSets(symp);
            miniparser::ProductionManager pm;
            pm.initProductions(symp);
            miniparser::GrammarGraph gg;
            gg.buildTransitionStateMachine(pm, symp);
            miniparser::GotoTable gt;
            gt.constructGotoTable(gg, symp);
            gototable.swap(gt.gototable);
        }

        std::tuple<int, int, int> getAction(int state, int symbol) {
            try {
                return gototable.at(state).at(symbol);
            }
            catch(...)
            {
                return {-1, -1, -1};
            }
        }
    };
}
