#ifndef LEAGUE_SHOPPER_CALC_CPP
#define LEAGUE_SHOPPER_CALC_CPP


#include <iostream>
#include "items.cpp"

namespace calc {
    std::vector<items::Item> GetAppropriateItems(const env::Champion& champ) {
        return champ.GetMainDamageType() == env::DamageType::PHYSICAL
                ? items::GetAvailableAdItems() : items::GetAvailableApItems();
    }

    env::DamageOutput ComputeDamageOutput(const env::GameState& gameState,
                                          const env::EnemyData& enemy, const items::Item& item) {
        env::DamageOutput damage;

        // Вычисляем урон от умений.
        // Считаем, что каждое умение будет использовано против врага ровно единожды,
        // при этом каждое умение будет использовано на максимум (если его урон может различаться).
        // Каждое умение будет усилено предметом.
        for (const auto& ability : gameState.GetChampion().GetAbilities()) {
            int bonusAd = gameState.GetCurBonusAd();
            int ap = item.AmplifyChampionAp(gameState.GetCurAp());
            int abilityDamage = ability.GetBaseDamage()
                    + (int) (bonusAd * ability.GetBonusAdScale() + ap * ability.GetApScale());

            abilityDamage = item.AmplifyAbility(abilityDamage, gameState);

            switch (ability.GetDamageType()) {
                case env::DamageType::PHYSICAL:
                    damage.physicalDamage += abilityDamage;
                    break;

                case env::MAGIC:
                    damage.magicDamage += abilityDamage;
                    break;

                case env::TRUE:
                    damage.trueDamage += abilityDamage;
                    break;
            }
        }

        // Вычисляем урон от автоатак.
        // Считаем, что враг получит 3 автоатаки, одна из которых будет усилена предметом.
        env::DamageOutput empoweredAaDmg = item.AmplifyAutoAttack(gameState);
        damage.physicalDamage += empoweredAaDmg.physicalDamage; // урон усиленных автоатак
        damage.magicDamage += empoweredAaDmg.magicDamage;       // может быть смешанным (физ. + маг.)
        damage.physicalDamage += 2 * gameState.GetTotalAd(); // и 2 обычные автоатаки

        /*
         * to_kill_raw = ...
         * to_kill_def = to_kill_raw * (1 + armor / 100)
         */

        // Учитываем защитные показатели врага.
        // 1.0 - RAW
        // 2.0 - RES    =>    RES = 2.0 * RAW / 1.0
        //todo

        return damage;
    }

    class ItemCalculator {
    private:
        std::string desc;

    protected:
        explicit ItemCalculator(const std::string& desc) {
            this->desc = desc;
        }

    public:
        std::string GetDesc() const {
            return desc;
        }

        virtual void Evaluate(const env::GameState& gameState) = 0;
    };

    class DamageToSquishestItemCalc : public ItemCalculator {
    public:
        DamageToSquishestItemCalc()
                : ItemCalculator("Максимальный урон по наименее защищённой цели")  {}

        void Evaluate(const env::GameState& gameState) override {
            int squishestEnemyIdx;
            int highest;

            for (int i = 0; i < gameState.GetEnemies().size(); i++) {
                int highestDamage;
                items::Item* bestItem;
                std::vector<items::Item> items = GetAppropriateItems(gameState.GetChampion());

                for (const auto& item : items) {

                }
            }
        }
    };

    class DamageToTankestItemCalc : public ItemCalculator {
    public:
        DamageToTankestItemCalc()
                : ItemCalculator("Максимальный урон по наиболее защищённой цели")  {}

        void Evaluate(const env::GameState& gameState) override {
            //todo 2
        }
    };

    class OverallTeamfightDamageItemCalc : public ItemCalculator {
    public:
        OverallTeamfightDamageItemCalc()
                : ItemCalculator("Максимальный общий урон в командных сражениях")  {}

        void Evaluate(const env::GameState& gameState) override {
            //todo 3
        }
    };

    std::vector<ItemCalculator*> GetAvailableCalculators() {
        std::vector<ItemCalculator*> calculators;

        calculators.emplace_back(new DamageToSquishestItemCalc());
        calculators.emplace_back(new DamageToTankestItemCalc());
        calculators.emplace_back(new OverallTeamfightDamageItemCalc());

        return calculators;
    }
}


#endif //LEAGUE_SHOPPER_CALC_CPP
