#ifndef LEAGUE_SHOPPER_ITEMS_CPP
#define LEAGUE_SHOPPER_ITEMS_CPP


#include <string>
#include <vector>
#include "env.cpp"

namespace items {
    class Item {
    private:
        std::string name;
        int price;

    protected:
        int attackDamage = 0;
        int abilityPower = 0;
        int lethality = 0; // = flatArmorPen @ 18 lvl
        int flatMagicPen = 0;
        int armorPenPercent = 0;
        int magicPenPercent = 0;

        Item(const std::string& name, const int& price) {
            this->name = name;
            this->price = price;
        }

    public:
        virtual int AmplifyChampionAp(const int& curAp) const {
            return curAp;
        }

        virtual env::DamageOutput AmplifyAutoAttack(const env::GameState& gameState) const {
            env::DamageOutput aaDmg;
            aaDmg.physicalDamage = gameState.GetTotalAd();

            return aaDmg;
        }

        virtual int AmplifyAbility(const int& abilityDamage, const env::GameState& gameState) const {
            return abilityDamage;
        }
    };

    /*
     *
     * Предметы на силу умений.
     *
     */

    class LichBane : public Item {
    public:
        LichBane() : Item("Гроза личей", 3000) {
            abilityPower = 80;
            flatMagicPen = 18;
        }

        env::DamageOutput AmplifyAutoAttack(const env::GameState& gameState) const override {
            env::DamageOutput aaDmg;

            aaDmg.physicalDamage = gameState.GetTotalAd();
            aaDmg.magicDamage = // 150% Base_AD + 40% AP
                      (int) (1.5 * gameState.GetChampion().GetBaseAd())
                    + (int) (0.4 * gameState.GetCurAp()               );

            return aaDmg;
        }
    };

    class HorizonFocus : public Item {
    public:
        HorizonFocus() : Item("Хекстековый прицел", 3000) {
            abilityPower = 100;
            flatMagicPen = 39;
        }

        int AmplifyAbility(const int& abilityDamage, const env::GameState& gameState) const override {
            return (int) (1.1 * abilityDamage); // +10% общего урона (до применения защитных показателей)
        }
    };

    class RabadonsDeathcap : public Item {
    public:
        RabadonsDeathcap() : Item("Смертельная шляпа Рабадона", 3800) {
            flatMagicPen = 18;
            abilityPower = 120;
        }

        int AmplifyChampionAp(const int& curAp) const override {
            return (int) (curAp * 1.35); // +35% общей силы умений
        }
    };

    class VoidStaff : public Item {
    public:
        VoidStaff() : Item("Посох Бездны", 2500) {
            abilityPower = 65;
            flatMagicPen = 18;
            magicPenPercent = 40;
        }
    };

    /*
     *
     * Предметы на силу атаки.
     *
     */

    class YomuusGhostblade : public Item {
    public:
        YomuusGhostblade() : Item("Призрачный клинок Йомуу", 3000) {
            attackDamage = 60;
            lethality = 21;
        }
    };

    class RavenousHydra : public Item {
    public:
        RavenousHydra() : Item("Ненасытная гидра", 3300) {
            attackDamage = 65;
        }

        env::DamageOutput AmplifyAutoAttack(const env::GameState& gameState) const override {
            env::DamageOutput aaDmg;
            aaDmg.physicalDamage =  // +36% урона от автоатак по области (= mean(12, 60))
                    (int) (1.36 * gameState.GetTotalAd());

            return aaDmg;
        }
    };

    class SeryldasGrudge : public Item {
    public:
        SeryldasGrudge() : Item("Злоба Серильды", 3400) {
            attackDamage = 45;
            armorPenPercent = 30;
        }
    };

    std::vector<Item> GetAvailableApItems() {
        std::vector<Item> items;

        items.emplace_back(LichBane());
        items.emplace_back(HorizonFocus());
        items.emplace_back(RabadonsDeathcap());
        items.emplace_back(VoidStaff());

        return items;
    }

    std::vector<Item> GetAvailableAdItems() {
        std::vector<Item> items;

        items.emplace_back(YomuusGhostblade());
        items.emplace_back(RavenousHydra());
        items.emplace_back(SeryldasGrudge());

        return items;
    }
}


#endif //LEAGUE_SHOPPER_ITEMS_CPP