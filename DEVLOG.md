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