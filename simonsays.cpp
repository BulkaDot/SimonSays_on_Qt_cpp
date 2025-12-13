#include "simonsays.h"
#include "./ui_simonsays.h"
#include <QGridLayout>
#include <QTimer>
#include <QStyle>
#include <QMessageBox>

// Конструктор класса
SimonSays::SimonSays(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::SimonSays)
    , buttonGroup(new QButtonGroup(this))  // Инициализация группы кнопок
{
    ui->setupUi(this); // Настройка UI из *.ui файла

    // Цикл для поиска кнопок по имени и назначения им стилей
    for(int i=1;i<=4;++i)
    {
        // Находим кнопку по имени pushButton_1..4
        QPushButton *btn = findChild<QPushButton*>(QString("pushButton_%1").arg(i));
        btn->setProperty("isHovered", false); // Изначально подсветка выключена

        if(btn) // Проверяем, что кнопка найдена
        {
            // Назначаем стиль каждой кнопке в зависимости от цвета
            switch(i)
            {
            case 1: btn->setStyleSheet(blueButtonStyle  ); break;
            case 2: btn->setStyleSheet(greenButtonStyle ); break;
            case 3: btn->setStyleSheet(redButtonStyle   ); break;
            case 4: btn->setStyleSheet(yellowButtonStyle); break;
            }
            // Добавляем кнопку в группу, чтобы отслеживать нажатия
            buttonGroup->addButton(btn, i);
        }
    }

    // Подключаем сигнал нажатия кнопки к слоту обработки
    connect(buttonGroup, &QButtonGroup::buttonClicked, this, &SimonSays::onButtonPush);
}

// Деструктор класса
SimonSays::~SimonSays()
{
    delete ui; // Удаляем UI
}

// Слот обработки нажатия игроком кнопки
void SimonSays::onButtonPush(QAbstractButton *button)
{
    if(playerSequence.isEmpty()) return; // Если последовательность пуста, ничего не делаем

    int currBtnId = buttonGroup->id(button); // Получаем ID нажатой кнопки

    // Проверяем правильность нажатия (сравниваем с первым элементом playerSequence)
    if(currBtnId == playerSequence.first())
    {
        playerSequence.removeFirst(); // Удаляем первый элемент (правильный шаг сделан)

        if(playerSequence.isEmpty()) // Если последовательность полностью пройдена
        {
            ++score; // Увеличиваем счет
            // Через 1 секунду генерируем следующий шаг Simon
            QTimer::singleShot(1000, this, [this]() { SimonThinks(); });
        }
    }
    else // Игрок ошибся
    {
        // Создаём диалог с сообщением
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("Игра окончена");
        msgBox.setText(QString("Вы не запомнили правильную последовательность и не ввели ее правильно!\nВаш счет: %1").arg(score));
        msgBox.setIcon(QMessageBox::Question);

        // Добавляем кнопки "Заново" и "Закончить"
        QPushButton *btnRestart = msgBox.addButton("Заново"   , QMessageBox::AcceptRole     );
        QPushButton *btnQuit    = msgBox.addButton("Закончить", QMessageBox::DestructiveRole);

        msgBox.exec(); // Показываем диалог

        // Проверяем, что нажал пользователь
        QPushButton *clicked = qobject_cast<QPushButton*>(msgBox.clickedButton());

        if (clicked == btnRestart) restartGame(); // Перезапуск игры
        else if (clicked == btnQuit) stopGame();  // Остановка игры
    }

    // Обновляем label со счётом
    ui->scoreLabel->setText(QString("Угаданные последовательности: %1").arg(score));
}

// Генерация следующего шага Simon
void SimonSays::SimonThinks()
{
    QRandomGenerator *random = QRandomGenerator::global(); // Получаем генератор
    randomId = random->bounded(4)+1; // Генерируем случайное число от 1 до 4

    simonSaysSequence.append(randomId); // Добавляем шаг в последовательность Simon
    playerSequence = simonSaysSequence; // Игрок должен повторить всю текущую последовательность

    Game(); // Запускаем подсветку последовательности
}

// Подсветка всей последовательности
void SimonSays::Game()
{
    delay = 0; // Сбрасываем задержку
    for(int i=0; i < simonSaysSequence.size(); ++i)
    {
        // Получаем кнопку по ID
        QAbstractButton *button = buttonGroup->button(simonSaysSequence[i]);

        // Через задержку подсвечиваем кнопку
        QTimer::singleShot(delay, this, [this, button]() { setButtonIsHovered(button, true); });

        delay += stepDelay; // Увеличиваем задержку для следующей кнопки
    }
}

// Подсветка одной кнопки
void SimonSays::setButtonIsHovered(QAbstractButton *button, bool isHover)
{
    button->setProperty("isHovered", isHover); // Устанавливаем свойство isHovered
    button->style()->unpolish(button);          // Снимаем стиль
    button->style()->polish(button);            // Применяем стиль заново
    button->update();                            // Обновляем виджет

    if (isHover)
    {
        // Через highlightDuration выключаем подсветку
        QTimer::singleShot(highlightDuration, this, [this, button]() {
            button->setProperty("isHovered", false);
            button->style()->unpolish(button);
            button->style()->polish(button);
            button->update();
        });
    }
}

// Кнопка "Старт" / запуска игры
void SimonSays::on_startButton_clicked()
{
    if(simonSaysSequence.isEmpty()) // Если игра ещё не начата
    {
        ui->statusGameLabel->setText("Статус игры: запущена"); // Обновляем статус
        QTimer::singleShot(1000, this, [this]() { SimonThinks(); }); // Генерируем первый шаг
    }
    else // Если игра уже идёт
    {
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("Начать заново?");
        msgBox.setText("Игра уже идёт. Вы хотите начать заново или закончить игру?");
        msgBox.setIcon(QMessageBox::Question);

        // Добавляем три кнопки
        QPushButton *btnRestart = msgBox.addButton("Заново"   , QMessageBox::AcceptRole     );
        QPushButton *btnQuit    = msgBox.addButton("Закончить", QMessageBox::DestructiveRole);
        QPushButton *btnCancel  = msgBox.addButton("Отмена"   , QMessageBox::RejectRole     );

        msgBox.exec(); // Показываем диалог

        // Проверяем нажатую кнопку
        QPushButton *clicked = qobject_cast<QPushButton*>(msgBox.clickedButton());

        if (clicked == btnRestart) restartGame(); // Перезапуск
        else if (clicked == btnQuit) stopGame();  // Остановка
        // Отмена — ничего не делаем
    }
}

// Перезапуск игры
void SimonSays::restartGame()
{
    score = 0; // Сбрасываем счёт
    ui->scoreLabel->setText(QString("Угаданные последовательности: %1").arg(score));

    simonSaysSequence.clear(); // Очищаем последовательность Simon
    playerSequence.clear();    // Очищаем последовательность игрока

    QTimer::singleShot(1000, this, [this]() { SimonThinks(); }); // Запускаем первый шаг нового раунда
}

// Остановка игры
void SimonSays::stopGame()
{
    score = 0; // Сброс счёта
    ui->scoreLabel     ->setText(QString("Угаданные последовательности: %1").arg(score));
    ui->statusGameLabel->setText("Статус игры: не запущена"); // Обновляем статус

    simonSaysSequence.clear(); // Очищаем последовательность Simon
    playerSequence   .clear(); // Очищаем последовательность игрока
}
