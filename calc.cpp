#include "calc.hpp"
#include "cli.hpp"

namespace calc {

    items::ItemType GetPrefItemType(const game::Champion* champ) {
        int totalPhysicalDmg = 0;
        int totalMagicDmg = 0;

        // Измеряем урон всех способностей, считай показатели доп. силы атаки и силы умений равными 300.
        // Это просто число "из головы" - достаточно большое, чтобы значительно повлиять на урон умений,
        // но не слишком большое, чтобы выйти за рамки реально возможных атакующих показателей в игре.
        // Учитываем GetUsesPerEnemy, т.к. некоторые умения могут быть использованы намного чаще других,
        // и, соответственно, атакующих показатели, от которых масштабируется их урон, гораздо важнее.
        for (game::Ability* ability : champ->GetAbilities()) {
            game::DamageOutput* dmg = ability->GetDamage(false, 300, 300);
            totalPhysicalDmg += ability->GetUsesPerEnemy() * dmg->physicalDamage;
            totalMagicDmg += ability->GetUsesPerEnemy() * dmg->magicDamage;
        }

        // >=, т.к. при равном масштабировании сила атаки выгоднее
        // силы умений, поскольку усиливает ещё и обычные автоатаки.
        return totalPhysicalDmg >= totalMagicDmg ? items::ItemType::AD : items::ItemType::AP;
    }

    void Evaluate(const bool& verbose, const game::GameState* gameState,
                                             const std::vector<items::Item*>& allItems) {
        items::ItemType prefItemType = GetPrefItemType(gameState->GetChampion());
        game::DamageOutput* highestDmgOfAll = nullptr;
        items::Item* bestItemOfAll = nullptr;
        int squishestEnemyIdx = -1;

        for (int i = 0; i < gameState->GetEnemies().size(); i++) {
            game::DamageOutput* highestDmgVsThis = nullptr;
            items::Item* bestItemVsThis = nullptr;

            if (verbose) {
                cli::PrintLn(L"&8┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓");
                cli::PrintLn(L"&8┃  &fВраг № " + std::to_wstring(i + 1), 60, L"  &8┃");
                cli::PrintLn(L"&8┣━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┫");
            }

            for (items::Item* item : allItems) {
                if (item->GetType() == prefItemType) { // не тратим время на заведомо неподходящие предметы
                    game::EnemyData* enemy = gameState->GetEnemies()[i];
                    game::DamageOutput* damage = ComputeDamageOutput(verbose, gameState, enemy, item);

                    if (highestDmgVsThis == nullptr || *damage > *highestDmgVsThis
                                || (*damage == *highestDmgVsThis
                                            && item->GetPrice() < bestItemVsThis->GetPrice())) {
                        highestDmgVsThis = damage;
                        bestItemVsThis = item;
                    }
                }
            }

            if (verbose) {
                cli::PrintLn(L"&8┃  ┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓  ┃");
                cli::PrintLn(L"&8┃  ┃  &aЛучшее противодействие", 59, L"&8┃  ┃");
                cli::PrintLn(L"&8┃  ┣━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┫  ┃");
                cli::PrintLn(L"&8┃  ┃  &7Предмет: &f" + bestItemVsThis->GetName()
                        + L" &6(" + std::to_wstring(bestItemVsThis->GetPrice()) + L" зол.)",
                        59, L"&8┃  ┃");
                cli::PrintLn(L"&8┃  ┃  &7Урон (&c"
                        + std::to_wstring(highestDmgVsThis->GetTotalDamage()) + L" &7всего):",
                        59, L"&8┃  ┃");
                cli::PrintLn(L"&8┃  ┃      &7Физический    &e"
                        + std::to_wstring(highestDmgVsThis->physicalDamage),
                        59, L"&8┃  ┃");
                cli::PrintLn(L"&8┃  ┃      &7Магический    &d"
                        + std::to_wstring(highestDmgVsThis->magicDamage),
                        59, L"&8┃  ┃");
                cli::PrintLn(L"&8┃  ┃      &7Чистый        &f"
                        + std::to_wstring(highestDmgVsThis->trueDamage),
                        59, L"&8┃  ┃");
                cli::PrintLn(L"&8┃  ┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛  ┃");
                cli::PrintLn(L"&8┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛\n\n");
            }

            if (highestDmgOfAll == nullptr || *highestDmgVsThis > *highestDmgOfAll
                        || (*highestDmgVsThis == *highestDmgOfAll
                                    && bestItemVsThis->GetPrice() < bestItemOfAll->GetPrice())) {
                highestDmgOfAll = highestDmgVsThis;
                bestItemOfAll = bestItemVsThis;
                squishestEnemyIdx = i;
            }
        }

        cli::PrintLn(L"Больше всего урона вы можете нанести по противнику № &f"
                + std::to_wstring(squishestEnemyIdx + 1) + L": &c"
                + std::to_wstring(highestDmgOfAll->GetTotalDamage()) + L" &7всего.");
        cli::PrintLn(L"Поможет вам в этом &f" + bestItemOfAll->GetName() + L" &7за &6"
                + std::to_wstring(bestItemOfAll->GetPrice()) + L" зол.");
        cli::PrintLn(L"Полная сводка по урону:");
        cli::PrintLn(L"    Физический    &e" + std::to_wstring(highestDmgOfAll->physicalDamage));
        cli::PrintLn(L"    Магический    &d" + std::to_wstring(highestDmgOfAll->magicDamage));
        cli::PrintLn(L"    Чистый        &f" + std::to_wstring(highestDmgOfAll->trueDamage) + L"\n\n");
    }

    game::DamageOutput* ComputeDamageOutput(const bool& verbose, const game::GameState* gameState,
                                           const game::EnemyData* enemy, const items::Item* item) {
        auto* damage = new game::DamageOutput(0, 0, 0);

        // Учитываем бонусные атакующие показатели предмета.
        int baseAd = gameState->GetChampion()->GetBaseAd();
        int bonusAd = item->GetAttackDamage() + gameState->GetCurBonusAd();
        int totalAd = baseAd + bonusAd;
        int ap = item->AmplifyChampionAp(item->GetAbilityPower() + gameState->GetCurAp());

        if (verbose) {
            cli::PrintLn(L"&8┃  ┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓  ┃");
            cli::PrintLn(L"&8┃  ┃  &f" + item->GetName() + L" &6("
                    + std::to_wstring(item->GetPrice()) + L" зол.)",
                    59, L"&8┃  ┃");
            cli::PrintLn(L"&8┃  ┣━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┫  ┃");
            cli::PrintLn(L"&8┃  ┃  ┏━━━━━ &7Атакующие показатели &8━━━━━━━━━━━━━━━━━━━━━━┓  ┃  ┃");
            cli::PrintLn(L"&8┃  ┃  ┃  &7Базовая сила атаки    &e" + std::to_wstring(baseAd),
                    56, L"&8┃  ┃  ┃");
            cli::PrintLn(L"&8┃  ┃  ┃  &7Доп. сила атаки       &e" + std::to_wstring(bonusAd),
                    56, L"&8┃  ┃  ┃");
            cli::PrintLn(L"&8┃  ┃  ┃  &7Общая сила атаки      &e" + std::to_wstring(totalAd),
                    56, L"&8┃  ┃  ┃");
            cli::PrintLn(L"&8┃  ┃  ┃  &7Сила умений           &d" + std::to_wstring(ap),
                    56, L"&8┃  ┃  ┃");

            cli::PrintLn(L"&8┃  ┃  ┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛  ┃  ┃");
            cli::PrintLn(L"&8┃  ┃  ┏━━━━━ &7Урон с умений &8━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓  ┃  ┃");
            // (*) сам урон с умений (баз. + доп.) подробно выводится внутри game::Ability#GetDamage
        }

        // Вычисляем урон от умений.
        // Считаем, что каждое умение будет использовано против врага {usesPerEnemy} раз,
        // при этом каждое умение будет использовано на максимум (если его урон может различаться).
        // Каждое умение будет усилено предметом.
        for (game::Ability* ability : gameState->GetChampion()->GetAbilities()) {
            game::DamageOutput* abilityDamage = ability->GetDamage(verbose, bonusAd, ap);

            int preItemDmg = abilityDamage->GetTotalDamage();
            abilityDamage = item->AmplifyAbility(abilityDamage); // усиление умения предметом

            if (verbose) {
                int postItemDmg = abilityDamage->GetTotalDamage();
                int bonusItemDmg = postItemDmg - preItemDmg;
                std::wstring itemBonusStr = bonusItemDmg > 0
                        ? L" &f(+" + std::to_wstring(bonusItemDmg) + L")"
                        : L"";

                cli::PrintLn(L"&8┃  ┃  ┃      &7С предметом    &c"
                        + std::to_wstring(postItemDmg) + itemBonusStr,
                        56, L"&8┃  ┃  ┃");
            }

            damage->physicalDamage += ability->GetUsesPerEnemy() * abilityDamage->physicalDamage;
            damage->magicDamage += ability->GetUsesPerEnemy() * abilityDamage->magicDamage;
            damage->trueDamage += ability->GetUsesPerEnemy() * abilityDamage->trueDamage;
        }

        // Вычисляем урон от автоатак.
        // Считаем, что враг получит 3 автоатаки, одна из которых будет усилена предметом.
        // Урон усиленных автоатак может быть смешанным. Считаем урон обычных автоатак всегда физическим.
        game::DamageOutput* empoweredAaDmg = item->AmplifyAutoAttack(baseAd, bonusAd, totalAd, ap); // 1 усиленная АА
        int ordinaryAaDmg = 2 * totalAd; // 2 обычные АА
        damage->physicalDamage += ordinaryAaDmg + empoweredAaDmg->physicalDamage;
        damage->magicDamage += empoweredAaDmg->magicDamage;
        damage->trueDamage += empoweredAaDmg->trueDamage;

        if (verbose) {
            cli::PrintLn(L"&8┃  ┃  ┃  ", 56, L"&8┃  ┃  ┃"); // немного "пустоты" между строк
            cli::PrintLn(L"&8┃  ┃  ┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛  ┃  ┃");
            cli::PrintLn(L"&8┃  ┃  ┏━━━━━ &7Урон с автоатак &8━━━━━━━━━━━━━━━━━━━━━━━━━━━┓  ┃  ┃");
            cli::PrintLn(L"&8┃  ┃  ┃  &7Усиленные (x1)    &c"
                    + std::to_wstring(empoweredAaDmg->GetTotalDamage()) + L" &7= &e"
                    + std::to_wstring(empoweredAaDmg->physicalDamage) + L" &7+ &d"
                    + std::to_wstring(empoweredAaDmg->magicDamage) + L" &7+ &f"
                    + std::to_wstring(empoweredAaDmg->trueDamage),
                    56, L"&8┃  ┃  ┃");
            cli::PrintLn(L"&8┃  ┃  ┃  &7Обычные   (x2)    &e"
                    + std::to_wstring(ordinaryAaDmg),
                    56, L"&8┃  ┃  ┃");
            cli::PrintLn(L"&8┃  ┃  ┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛  ┃  ┃");
        }

        // Учитываем защитные показатели врага.
        // В рассчёт также берётся пробивание ("плоское" и "процентное"), полученное с предмета.
        // Чистый урон никакими защитными показателями уменьшен быть не может.
        int enemyArmor = (int) (enemy->GetArmor()
                * (1.0 - item->GetArmorPenPercent() / 100.0)) - item->GetLethality();
        int enemyMRes = (int) (enemy->GetMagicResistance()
                * (1.0 - item->GetMagicPenPercent() / 100.0)) - item->GetFlatMagicPen();

        if (verbose) {
            int armorShredded = enemy->GetArmor() - enemyArmor;
            std::wstring armorShredStr = armorShredded > 0
                    ? L" &f(-" + std::to_wstring(armorShredded) + L")"
                    : L"";

            int mResShredded = enemy->GetMagicResistance() - enemyMRes;
            std::wstring mResShredStr = mResShredded > 0
                    ? L" &f(-" + std::to_wstring(mResShredded) + L")"
                    : L"";

            cli::PrintLn(L"&8┃  ┃  ┏━━━━━ &7Защитные показатели врага &8━━━━━━━━━━━━━━━━━┓  ┃  ┃");
            cli::PrintLn(L"&8┃  ┃  ┃  &7Броня                  &e"
                    + std::to_wstring(enemy->GetArmor()) + L" &f--> &e"
                    + std::to_wstring(enemyArmor) + armorShredStr,
                    56, L"&8┃  ┃  ┃");
            cli::PrintLn(L"&8┃  ┃  ┃  &7Сопротивление магии    &d"
                    + std::to_wstring(enemy->GetMagicResistance()) + L" &f--> &d"
                    + std::to_wstring(enemyMRes) + mResShredStr,
                    56, L"&8┃  ┃  ┃");
            cli::PrintLn(L"&8┃  ┃  ┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛  ┃  ┃");
        }

        if (verbose) {
            cli::PrintLn(L"&8┃  ┃  ┏━━━━━ &7Общий урон &8━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓  ┃  ┃");
            cli::PrintLn(L"&8┃  ┃  ┃  ", 56, L"&8┃  ┃  ┃"); // немного "пустоты" между строк
            cli::PrintLn(L"&8┃  ┃  ┃  &7До учёта защиты врага (&c"
                    + std::to_wstring(damage->GetTotalDamage()) + L" &7всего)",
                    56, L"&8┃  ┃  ┃");
            cli::PrintLn(L"&8┃  ┃  ┃      &7Физический    &e"
                    + std::to_wstring(damage->physicalDamage),
                    56, L"&8┃  ┃  ┃");
            cli::PrintLn(L"&8┃  ┃  ┃      &7Магический    &d"
                    + std::to_wstring(damage->magicDamage),
                    56, L"&8┃  ┃  ┃");
            cli::PrintLn(L"&8┃  ┃  ┃      &7Чистый        &f"
                    + std::to_wstring(damage->trueDamage),
                    56, L"&8┃  ┃  ┃");
        }

        damage->physicalDamage = ApplyDefensiveStat(damage->physicalDamage, enemyArmor);
        damage->magicDamage    = ApplyDefensiveStat(damage->magicDamage,    enemyMRes );

        if (verbose) {
            cli::PrintLn(L"&8┃  ┃  ┃  ", 56, L"&8┃  ┃  ┃"); // немного "пустоты" между строк
            cli::PrintLn(L"&8┃  ┃  ┃  &7После учёта защиты врага (&c"
                    + std::to_wstring(damage->GetTotalDamage()) + L" &7всего)",
                    56, L"&8┃  ┃  ┃");
            cli::PrintLn(L"&8┃  ┃  ┃      &7Физический    &e"
                    + std::to_wstring(damage->physicalDamage),
                    56, L"&8┃  ┃  ┃");
            cli::PrintLn(L"&8┃  ┃  ┃      &7Магический    &d"
                    + std::to_wstring(damage->magicDamage),
                    56, L"&8┃  ┃  ┃");
            cli::PrintLn(L"&8┃  ┃  ┃      &7Чистый        &f"
                    + std::to_wstring(damage->trueDamage),
                    56, L"&8┃  ┃  ┃");
            cli::PrintLn(L"&8┃  ┃  ┃  ", 56, L"&8┃  ┃  ┃"); // немного "пустоты" между строк
            cli::PrintLn(L"&8┃  ┃  ┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛  ┃  ┃");
            cli::PrintLn(L"&8┃  ┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛  ┃");
        }

        return damage;
    }

    int ApplyDefensiveStat(const int& rawDamage, const int& defensiveStat) {
        double dmgMultiplier;

        if (defensiveStat >= 0)
            dmgMultiplier = 100.0 / (100.0 + defensiveStat);
        else
            dmgMultiplier = 2.0 - (100.0 / (100.0 - defensiveStat));

        return (int) (dmgMultiplier * rawDamage);
    }

}
