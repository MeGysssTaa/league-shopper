#pragma once


#include <iostream>
#include "items.hpp"

namespace calc {

    items::ItemType GetPrefItemType(const game::Champion* champ);

    void Evaluate(const bool& verbose, const game::GameState* gameState,
                  const std::vector<items::Item*>& allItems);

    game::DamageOutput* ComputeDamageOutput(const bool& verbose, const game::GameState* gameState,
                                           const game::EnemyData* enemy, const items::Item* item);

    int ApplyDefensiveStat(const int& rawDamage, const int& defensiveStat);

}
