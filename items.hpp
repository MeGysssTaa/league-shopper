#pragma once


#include <string>
#include <vector>
#include "game.hpp"

namespace items {
    enum ItemType {
        AD,
        AP
    };

    class Item {
    private:
        std::wstring name;
        int price;

    protected:
        int attackDamage = 0;
        int abilityPower = 0;
        int lethality = 0; // = flatArmorPen @ 18 lvl
        int flatMagicPen = 0;
        int armorPenPercent = 0;
        int magicPenPercent = 0;

        Item(const std::wstring& name, const int& price);

    public:
        std::wstring GetName() const;
        int GetPrice() const;

        int GetAttackDamage    () const;
        int GetAbilityPower    () const;
        int GetLethality       () const;
        int GetFlatMagicPen    () const;
        int GetArmorPenPercent () const;
        int GetMagicPenPercent () const;
        ItemType GetType       () const;

        virtual int AmplifyChampionAp(const int& curAp) const;

        virtual game::DamageOutput* AmplifyAutoAttack(const int& baseAd, const int& bonusAd,
                                                      const int& totalAd, const int& ap) const;

        virtual game::DamageOutput* AmplifyAbility(const game::DamageOutput* abilityDamage) const;
    };

    /*
     *
     * Предметы на силу умений.
     *
     */

    class LichBane : public Item {
    public:
        LichBane();
        game::DamageOutput* AmplifyAutoAttack(const int& baseAd, const int& bonusAd,
                                             const int& totalAd, const int& ap) const override;
    };

    class HorizonFocus : public Item {
    public:
        HorizonFocus();
        game::DamageOutput* AmplifyAbility(const game::DamageOutput* abilityDamage) const override;
    };

    class RabadonsDeathcap : public Item {
    public:
        RabadonsDeathcap();
        int AmplifyChampionAp(const int& curAp) const override;
    };

    class VoidStaff : public Item {
    public:
        VoidStaff();
    };

    /*
     *
     * Предметы на силу атаки.
     *
     */

    class YomuusGhostblade : public Item {
    public:
        YomuusGhostblade();
    };

    class RavenousHydra : public Item {
    public:
        RavenousHydra();
        game::DamageOutput* AmplifyAutoAttack(const int& baseAd, const int& bonusAd,
                                              const int& totalAd, const int& ap) const override;
    };

    class SeryldasGrudge : public Item {
    public:
        SeryldasGrudge();
    };

    std::vector<Item*> GetAvailableItems();
}
