#include "simonsays.h"
#include "./ui_simonsays.h"
#include <QGridLayout>
#include <QTimer>
#include <QStyle>
#include <QMessageBox>

SimonSays::SimonSays(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::SimonSays)
    , buttonGroup(new QButtonGroup(this))
{
    ui->setupUi(this);
    score = 0;

    // Инициализация кнопок игры
    for(int i = 1; i <= 4; ++i)
    {
        QPushButton *btn = findChild<QPushButton*>(QString("pushButton_%1").arg(i));
        btn->setProperty("isHovered", false);

        if(btn)
        {
            // Назначение цветовых стилей для каждой кнопки
            switch(i)
            {
            case 1: btn->setStyleSheet(blueButtonStyle); break;
            case 2: btn->setStyleSheet(greenButtonStyle); break;
            case 3: btn->setStyleSheet(redButtonStyle); break;
            case 4: btn->setStyleSheet(yellowButtonStyle); break;
            }
            buttonGroup->addButton(btn, i);
        }
    }

    // Подключение обработчика нажатий кнопок
    connect(buttonGroup, &QButtonGroup::buttonClicked, this, &SimonSays::onButtonPush);
}

SimonSays::~SimonSays()
{
    delete ui;
}

// Генерация и демонстрация последовательности от игры
void SimonSays::SimonThinks()
{
    // Если игра остановлена, не генерируем новую последовательность
    if(simonSaysSequence.isEmpty() && !playerSequence.isEmpty()) {
        return;
    }

    QRandomGenerator *random = QRandomGenerator::global();
    randomId = random->bounded(4) + 1;

    simonSaysSequence.append(randomId);
    playerSequence = simonSaysSequence;

    Game();
}

// Демонстрация текущей последовательности с подсветкой кнопок
void SimonSays::Game()
{
    // Проверка активности игры
    if(simonSaysSequence.isEmpty()) {
        return;
    }

    delay = 0;
    for(int i = 0; i < simonSaysSequence.size(); ++i)
    {
        int expectedSize = simonSaysSequence.size();
        QTimer::singleShot(delay, this, [this, expectedSize, i]() {
            // Проверка, что последовательность не изменилась во время задержки
            if(simonSaysSequence.size() != expectedSize || simonSaysSequence.isEmpty()) {
                return;
            }

            QAbstractButton *button = buttonGroup->button(simonSaysSequence[i]);
            if(button) {
                setButtonIsHovered(button, true);
            }
        });

        delay += stepDelay;
    }
}

// Управление подсветкой кнопок (активация/деактивация)
void SimonSays::setButtonIsHovered(QAbstractButton *button, bool isHover)
{
    if(!button) return;

    button->setProperty("isHovered", isHover);
    button->style()->unpolish(button);
    button->style()->polish(button);
    button->update();

    if (isHover)
    {
        // Автоматическое отключение подсветки через заданное время
        QTimer::singleShot(highlightDuration, this, [this, button]() {
            if(button) {
                button->setProperty("isHovered", false);
                button->style()->unpolish(button);
                button->style()->polish(button);
                button->update();
            }
        });
    }
}

// Обработка нажатия игроком кнопки
void SimonSays::onButtonPush(QAbstractButton *button)
{
    // Игнорируем нажатия, если игра не активна
    if(simonSaysSequence.isEmpty() || playerSequence.isEmpty()) {
        return;
    }

    int currBtnId = buttonGroup->id(button);

    // Проверка правильности нажатой кнопки
    if(currBtnId == playerSequence.first())
    {
        playerSequence.removeFirst();

        // Если вся последовательность угадана
        if(playerSequence.isEmpty())
        {
            ++score;
            // Запуск следующего раунда
            if(!simonSaysSequence.isEmpty()) {
                QTimer::singleShot(1000, this, [this]() {
                    if(!simonSaysSequence.isEmpty()) {
                        SimonThinks();
                    }
                });
            }
        }
    }
    else
    {
        // Обработка неправильного нажатия - конец игры
        showGameOverDialog();
    }

    ui->scoreLabel->setText(QString("Угаданные последовательности: %1").arg(score));
}

// Показ диалога окончания игры
void SimonSays::showGameOverDialog()
{
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("Игра окончена");
    msgBox.setText(QString("Вы не запомнили правильную последовательность и не ввели ее правильно!\nВаш счет: %1").arg(score));
    msgBox.setIcon(QMessageBox::Question);

    QPushButton *btnRestart = msgBox.addButton("Заново", QMessageBox::AcceptRole);
    QPushButton *btnQuit = msgBox.addButton("Закончить", QMessageBox::DestructiveRole);

    msgBox.exec();
    QPushButton *clicked = qobject_cast<QPushButton*>(msgBox.clickedButton());

    if (clicked == btnRestart) {
        restartGame();
    }
    else if (clicked == btnQuit) {
        stopGame();
    }
}

// Обработчик нажатия кнопки "Старт"
void SimonSays::on_startButton_clicked()
{
    if(simonSaysSequence.isEmpty())
    {
        ui->statusGameLabel->setText("Статус игры: запущена");
        QTimer::singleShot(1000, this, [this]() { SimonThinks(); });
    }
    else
    {
        showRestartConfirmationDialog();
    }
}

// Диалог подтверждения перезапуска игры
void SimonSays::showRestartConfirmationDialog()
{
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("Начать заново?");
    msgBox.setText("Игра уже идёт. Вы хотите начать заново или закончить игру?");
    msgBox.setIcon(QMessageBox::Question);

    QPushButton *btnRestart = msgBox.addButton("Заново", QMessageBox::AcceptRole);
    QPushButton *btnQuit = msgBox.addButton("Закончить", QMessageBox::DestructiveRole);
    QPushButton *btnCancel = msgBox.addButton("Отмена", QMessageBox::RejectRole);

    msgBox.exec();
    QPushButton *clicked = qobject_cast<QPushButton*>(msgBox.clickedButton());

    if (clicked == btnRestart) {
        restartGame();
    }
    else if (clicked == btnQuit) {
        stopGame();
    }
}

// Перезапуск игры с очисткой текущего состояния
void SimonSays::restartGame()
{
    stopGame();

    QTimer::singleShot(100, this, [this]() {
        ui->statusGameLabel->setText("Статус игры: запущена");
        QTimer::singleShot(1000, this, [this]() { SimonThinks(); });
    });
}

// Полная остановка игры и сброс состояния
void SimonSays::stopGame()
{
    simonSaysSequence.clear();
    playerSequence.clear();

    score = 0;
    ui->scoreLabel->setText(QString("Угаданные последовательности: %1").arg(score));
    ui->statusGameLabel->setText("Статус игры: не запущена");

    // Сброс подсветки всех кнопок
    for(int i = 1; i <= 4; ++i) {
        QAbstractButton *btn = buttonGroup->button(i);
        if(btn) {
            btn->setProperty("isHovered", false);
            btn->style()->unpolish(btn);
            btn->style()->polish(btn);
            btn->update();
        }
    }

    QCoreApplication::removePostedEvents(this);
}
