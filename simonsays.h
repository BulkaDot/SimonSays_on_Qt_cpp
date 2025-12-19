#ifndef SIMONSAYS_H
#define SIMONSAYS_H

#include <QMainWindow>       // Основной класс окна Qt
#include <QButtonGroup>      // Группа кнопок, чтобы отслеживать их нажатия
#include <QRandomGenerator>  // Генератор случайных чисел
#include <QPushButton>       // Для кнопок

QT_BEGIN_NAMESPACE
namespace Ui {
class SimonSays;           // Автоматически сгенерированный класс UI
}
QT_END_NAMESPACE

// Главный класс игры Simon Says
class SimonSays : public QMainWindow
{
    Q_OBJECT   // Нужен для сигналов и слотов Qt

public:
    SimonSays(QWidget *parent = nullptr);  // Конструктор
    ~SimonSays();                           // Деструктор

private slots:
    void onButtonPush(QAbstractButton *button);   // Слот для нажатия одной из кнопок игрока
    void on_startButton_clicked();                // Слот для кнопки "Старт" / начала игры

private:
    Ui::SimonSays *ui;             // Указатель на UI
    QButtonGroup *buttonGroup;     // Группа всех цветных кнопок

    int score;                     // Счёт игрока (количество полностью угаданных последовательностей)
    int randomId;                  // Случайный ID кнопки для следующего шага Simon
    int delay = 0;                 // Задержка для последовательного подсвечивания кнопок
    int stepDelay = 1000;          // Шаг задержки между подсветкой кнопок (в миллисекундах)
    int highlightDuration = 500;   // Время подсветки кнопки (в миллисекундах)
    QVector<int> simonSaysSequence; // Полная последовательность Simon
    QVector<int> playerSequence;     // Последовательность, которую должен повторить игрок

    // Стили для кнопок (цвета, закругление, hover, pressed и подсветка)
    QString blueButtonStyle =
        "QPushButton { background-color: #1E88E5; border-radius: 12px; }"
        "QPushButton:hover { background-color: #42A5F5; }"
        "QPushButton:pressed { background-color: #1565C0; }"
        "QPushButton[isHovered=\"true\"] { background-color: #90CAF9; }";
    QString redButtonStyle =
        "QPushButton { background-color: #E53935; border-radius: 12px; }"
        "QPushButton:hover { background-color: #EF5350; }"
        "QPushButton:pressed { background-color: #B71C1C; }"
        "QPushButton[isHovered=\"true\"] { background-color: #FFCDD2; }";
    QString greenButtonStyle =
        "QPushButton { background-color: #43A047; border-radius: 12px; }"
        "QPushButton:hover { background-color: #66BB6A; }"
        "QPushButton:pressed { background-color: #2E7D32; }"
        "QPushButton[isHovered=\"true\"] { background-color: #C8E6C9; }";
    QString yellowButtonStyle =
        "QPushButton { background-color: #FDD835; border-radius: 12px; }"
        "QPushButton:hover { background-color: #FFEE58; }"
        "QPushButton:pressed { background-color: #F9A825; }"
        "QPushButton[isHovered=\"true\"] { background-color: #FFF9C4; }";

    // Основные методы игры
    void SimonThinks();                       // Генерация следующего шага Simon
    void Game();                              // Подсветка всей текущей последовательности
    void setButtonIsHovered(QAbstractButton *button, bool isHovered); // Включение / отключение подсветки кнопки
    void restartGame();                       // Перезапуск игры с нуля
    void stopGame();                          // Остановка игры (сброс всего)
    void clearAllTimers();                    // Очистка всех работающих таймеров
    void showRestartConfirmationDialog();     // Показ диалога при нажатии кнопки старт во время игры
    void showGameOverDialog();                // Показ диалога при проигрыше
};
#endif // SIMONSAYS_H
