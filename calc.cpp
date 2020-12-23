#include <map>
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
            game::DamageOutput* dmg = ability->GetDamage(false, 100 + 300, 300, 300);
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

        std::map<items::Item*,
                 std::map<game::EnemyData*, game::DamageOutput*>> dmgStats;

        for (items::Item* item : allItems)
            if (item->GetType() == prefItemType) // не тратим время на заведомо неподходящие предметы
                dmgStats[item] = std::map<game::EnemyData*, game::DamageOutput*>();

        for (game::EnemyData* enemy : gameState->GetEnemies()) {
            game::DamageOutput* highestDmgVsThis = nullptr;
            items::Item* bestItemVsThis = nullptr;

            if (verbose) {
                cli::PrintLn(L"&8┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓");
                cli::PrintLn(L"&8┃  &fВраг " + enemy->GetName(), 60, L"  &8┃");
                cli::PrintLn(L"&8┣━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┫");
            }

            for (items::Item* item : allItems) {
                if (item->GetType() == prefItemType) { // не тратим время на заведомо неподходящие предметы
                    auto* damage = ComputeDamageOutput(verbose, gameState, enemy, item);
                    dmgStats[item][enemy] = damage;

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
        }

        game::DamageOutput* topSqDmg = nullptr; // наибольший возможный урон по самой тонкой цели
        game::EnemyData* squishestEnemy = nullptr;
        items::Item* bestVsSquishest = nullptr;

        game::DamageOutput* topTfDmg = nullptr; // наибольший общий урон в командных сражениях
        items::Item* bestInTeamfight = nullptr;

        for (const auto& [item, stats] : dmgStats) { // проходимся по всем подходящим предметам
            game::DamageOutput* itemTopSqDmg = nullptr;
            auto* itemTfDmg = new game::DamageOutput(0, 0, 0);

            for (const auto& [enemy, dmg] : stats) { // проходимся по сводке по урону по каждому из врагов
                // Ищем цель, по которой мы можем нанести больше всего
                // урона, и предмет, который поможет нам это сделать.
                if (itemTopSqDmg == nullptr || *dmg > *itemTopSqDmg) {
                    itemTopSqDmg = dmg;

                    if (topSqDmg == nullptr || *itemTopSqDmg > *topSqDmg
                            || (*itemTopSqDmg == *topSqDmg
                                    && item->GetPrice() < bestVsSquishest->GetPrice())) {
                        topSqDmg = itemTopSqDmg;
                        squishestEnemy = enemy;
                        bestVsSquishest = item;
                    }
                }

                // Ищем предмет, который поможет нам нанести наибольший общий урон в командных
                // сражениях, т.е. наибольший суммарный урон по всем введённым противникам.
                *itemTfDmg += *dmg;

                if (topTfDmg == nullptr || *itemTfDmg > *topTfDmg
                        || (*itemTfDmg == *topTfDmg
                                && item->GetPrice() < bestInTeamfight->GetPrice())) {
                    topTfDmg = itemTfDmg;
                    bestInTeamfight = item;
                }
            }
        }

        cli::PrintLn(L"&8┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓");
        cli::PrintLn(L"&8┃  &aРезультаты", 60, L"  &8┃");
        cli::PrintLn(L"&8┣━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┫");
        cli::PrintLn(L"&8┃  &7Больше всего урона вы можете нанести &fврагу "
                + squishestEnemy->GetName() + L"&7:",
                60, L"  &8┃");
        cli::PrintLn(L"&8┃  &c"
                + std::to_wstring(topSqDmg->GetTotalDamage()) + L" &7= &e"
                + std::to_wstring(topSqDmg->physicalDamage) + L" &7+ &d"
                + std::to_wstring(topSqDmg->magicDamage) + L" &7+ &f"
                + std::to_wstring(topSqDmg->trueDamage),
                60, L"  &8┃");
        cli::PrintLn(L"&8┃  &7А поможет вам в этом:", 60, L"  &8┃");
        cli::PrintLn(L"&8┃  &f" + bestVsSquishest->GetName() + L" &6("
                + std::to_wstring(bestVsSquishest->GetPrice()) + L" зол.)", 60, L"  &8┃");
        cli::PrintLn(L"&8┣━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┫");
        cli::PrintLn(L"&8┃  &7Максимальный урон в командных сражениях:", 60, L"  &8┃");
        cli::PrintLn(L"&8┃  &c"
                     + std::to_wstring(topTfDmg->GetTotalDamage()) + L" &7= &e"
                     + std::to_wstring(topTfDmg->physicalDamage) + L" &7+ &d"
                     + std::to_wstring(topTfDmg->magicDamage) + L" &7+ &f"
                     + std::to_wstring(topTfDmg->trueDamage),
                     60, L"  &8┃");
        cli::PrintLn(L"&8┃  &7Нанести его поможет:", 60, L"  &8┃");
        cli::PrintLn(L"&8┃  &f" + bestInTeamfight ->GetName() + L" &6("
                     + std::to_wstring(bestInTeamfight->GetPrice()) + L" зол.)", 60, L"  &8┃");
        cli::PrintLn(L"&8┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛\n\n");
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
            game::DamageOutput* abilityDamage = ability->GetDamage(verbose, totalAd, bonusAd, ap);

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
        // Считаем, что враг получит 5 автоатак, 1 из которых будет усилена предметом.
        // Урон усиленных автоатак может быть смешанным. Считаем урон обычных автоатак всегда физическим.
        game::DamageOutput* empoweredAaDmg
                = *item->AmplifyAutoAttack(baseAd, bonusAd, totalAd, ap) * 1; // 1 усиленная АА

        int ordinaryAaDmg = totalAd * 4; // 4 обычных АА

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
            cli::PrintLn(L"&8┃  ┃  ┃  &7Обычные   (x4)    &e"
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
