#include "items.hpp"

namespace items {

    /*
     * Item
     */
    Item::Item(const std::wstring& name, const int& price) {
        this->name = name;
        this->price = price;
    }

    std::wstring Item::GetName() const {
        return name;
    }

    int Item::GetPrice() const {
        return price;
    }

    int Item::GetAttackDamage() const {
        return attackDamage;
    }

    int Item::GetAbilityPower() const {
        return abilityPower;
    }

    int Item::GetLethality() const {
        return lethality;
    }

    int Item::GetFlatMagicPen() const {
        return flatMagicPen;
    }

    int Item::GetArmorPenPercent() const {
        return armorPenPercent;
    }

    int Item::GetMagicPenPercent() const {
        return magicPenPercent;
    }

    ItemType Item::GetType() const {
        return attackDamage > 0 ? ItemType::AD : ItemType::AP;
    }

    int Item::AmplifyChampionAp(const int& curAp) const {
        return curAp;
    }

    game::DamageOutput* Item::AmplifyAutoAttack(const int& baseAd, const int& bonusAd,
                                                const int& totalAd, const int& ap) const {
        return new game::DamageOutput(totalAd, 0, 0);
    }

    game::DamageOutput* Item::AmplifyAbility(const game::DamageOutput* abilityDamage) const {
        return new game::DamageOutput(
                abilityDamage->physicalDamage,
                abilityDamage->magicDamage,
                abilityDamage->trueDamage
        );
    }

    /*
     * LichBane
     */
    LichBane::LichBane() : Item(L"Гроза личей", 3000) {
        abilityPower = 80;
    }

    game::DamageOutput* LichBane::AmplifyAutoAttack(const int& baseAd, const int& bonusAd,
                                                    const int& totalAd, const int& ap) const {
        return new game::DamageOutput(
                totalAd,
                (int) (1.5 * baseAd) + (int) (0.4 * ap),
                0
        );
    }

    /**
     * HorizonFocus
     */
    HorizonFocus::HorizonFocus() : Item(L"Хекстековый прицел", 3000) {
        abilityPower = 100;
    }

    game::DamageOutput* HorizonFocus::AmplifyAbility(const game::DamageOutput* abilityDamage) const {
        return new game::DamageOutput(
                (int) (1.1 * abilityDamage->physicalDamage),
                (int) (1.1 * abilityDamage->magicDamage),
                (int) (1.1 * abilityDamage->trueDamage)
        );
    }

    /**
     * RabadonsDeathcap
     */
    RabadonsDeathcap::RabadonsDeathcap() : Item(L"Смертельная шляпа Рабадона", 3800) {
        abilityPower = 120;
    }

    int RabadonsDeathcap::AmplifyChampionAp(const int& curAp) const {
        return (int) (curAp * 1.35); // +35% общей силы умений
    }

    /**
     * VoidStaff
     */
    VoidStaff::VoidStaff() : Item(L"Посох Бездны", 2500) {
        abilityPower = 65;
        magicPenPercent = 40;
    }

    /*
     * YomuusGhostblade
     */
    YomuusGhostblade::YomuusGhostblade() : Item(L"Призрачный клинок Йомуу", 3000) {
        attackDamage = 60;
        lethality = 21;
    }

    /*
     * RavenousHydra
     */
    RavenousHydra::RavenousHydra() : Item(L"Ненасытная гидра", 3300) {
        attackDamage = 65;
    }

    game::DamageOutput* RavenousHydra::AmplifyAutoAttack(const int& baseAd, const int& bonusAd,
                                                        const int& totalAd, const int& ap) const {
        return new game::DamageOutput(
                (int) (1.36 * totalAd),
                0,
                0
        );
    }

    /*
     * SeryldasGrudge
     */
    SeryldasGrudge::SeryldasGrudge() : Item(L"Злоба Серильды", 3400) {
        attackDamage = 45;
        armorPenPercent = 30;
    }
    
    /*
     * <...>
     */
    std::vector<Item*> GetAvailableItems() {
        return {
            new LichBane(),
            new HorizonFocus(),
            new RabadonsDeathcap(),
            new VoidStaff(),
            new YomuusGhostblade(),
            new RavenousHydra(),
            new SeryldasGrudge()
        };
    }

}
