# DEVLOG

## 2026-09-15
Удалили лишний класс MyMyCharacter, подключили EnhancedInput в Build.cs,
Move/Look через Enhanced Input работают. BP пока называется MyMyCharacter.
Урок: Build.cs/.h менять при закрытом редакторе; открывать проект только через .uproject на D:.

## 2026-09-16
Git, ROADMAP.md, DESIGN.md.

## 2026-09-16 (ночь)
Этап 2 закрыт: ECombatState, стамина, IA_Attack/IA_Block. Ловушка: в IMC_Default маппинги
класть в Default Key Mappings, а не в Mapping Profile Overrides.

## 2026-09-16 (обед)
GitHub: tuleushovbereke-bit/KazakhKhanate. MyMyCharacter → BP_MyCharacter, свой BP_GameMode.
Ловушка: IA_MouseLook жил в IMC_MouseLook, шаблонный контроллер подключал его сам — перенёс в IMC_Default.

## 2026-09-16 (день)
AM_Attack_01 из шаблонного MM_Attack_01 играет по ЛКМ через PlayAnimMontage,
длительность Attacking = длина монтажа. Движение заперто во время удара.
Root motion в удаче оставили (выпад вперёд — souls-стиль). Live Coding (Ctrl+Alt+F11) для .cpp работает.
Добавил commit.bat.

## 2026-09-16 (вечер)
AnimNotifyState HitWindow работает, HitWindow: OPEN мигает в дебаге.
Ловушка: include должен быть AnimNotifyState_HitWindow.h, не MyAnimNotifyState.h.


## 2026-09-17 (утро)
ABaseCharacter с здоровьем и TakeDamage. AEnemyBase наследует от него.
BP_Enemy на сцене стоит, анимация работает. Урон не наносится — разбираем в следующей сессии.
MyCharacter теперь наследует от ABaseCharacter вместо ACharacter.

## 2026-09-17 (день)
Урон работает: SweepMultiByChannel в SetHitWindowOpen находит врага и вызывает ApplyDamage 25.
Красная строка BP_Enemy took 25 damage в дебаге подтверждает.

## 2026-09-17 (день)
Враг атакует игрока по таймеру каждые 2 сек если в радиусе 150. 
HP игрока падает в дебаге. Рэгдолл при смерти работает.
Этап 4 базово закрыт.

## 2026-09-17 (день)
NavMesh добавлен на сцену. Враг идёт к игроку через MoveToActor.
Парит — анимация ходьбы врага следующий шаг.

## 2026-09-17 (вечер)
ABP_Enemy создан: BS_Idle_Walk_Run в AnimGraph, скорость через EventGraph.
Враг больше не парит — ходит с анимацией ходьбы.

## 2026-09-17 (вечер)
Lock-on камера: средняя кнопка мыши, плавный поворот к врагу через RInterpTo.
Тег Enemy на BP_Enemy чтобы не захватывать случайные акторы.
Камера смотрит на грудь врага (+60 по Z).

## 2026-09-17 (вечер)
Смерть игрока: рэгдолл, отключение ввода, респаун через перезагрузку уровня.
Анимация удара врага: AM_Attack_01 + DefaultSlot в ABP_Enemy.

## 2026-09-17 (вечер)
Начал HUD: WBP_HUD с Canvas Panel и HealthBar (Progress Bar).
Следующее: стамина бар, привязка к переменным персонажа, вывод на экран.

## 2026-09-18 (обед)
HUD: бары HP и стамины (бинды + вывод на экран); лок-он камера — фикс наклона вниз

## 2026-09-18 (день)
Возрождение у очага (BP_Ochag + тег): выход из рэгдолла, телепорт, восстановление HP/стамины/ввода вместо OpenLevel

## 2026-09-18 (вечер)
Респаун врагов: BP_Ochag спавнит BP_Enemy по тегу EnemySpawn, сброс старых через Destroy. Auto Possess AI = Spawned. Этап 6 закрыт

## 2026-09-18 (ночь)
Уклонение (перекат): IA_Dodge на пробел, состояние Dodging, LaunchCharacter рывок в сторону движения, трата стамины. i-frames и анимация — следующим

## 2026-09-19 (до обеда)
Рефакторинг: окно удара поднято в ABaseCharacter, игрок и враг делят одну боевую систему (SweepMultiByChannel через HitWindow notify). Враг бьёт честно в фазе анимации