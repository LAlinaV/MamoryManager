#include <QApplication>
#include <QMainWindow>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <QRandomGenerator>
#include <QTabWidget>
#include <QDebug>
#include <cstring>
#include <vector>
#include <unordered_map>
#include "memory.h"

const int PAGES_COUNT = 30;
MemoryManager manager(PAGES_COUNT);

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr) : QMainWindow(parent) {
         QWidget *centralWidget = new QWidget(this);
        QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

        // Create a tab widget
        QTabWidget *tabWidget = new QTabWidget(this);

        // First tab for process management
        QWidget *processTab = new QWidget(this);
        QVBoxLayout *processLayout = new QVBoxLayout(processTab);

        // Create the first input field with label
        QHBoxLayout *processSizeLayout = new QHBoxLayout();
        QLabel *processSizeLabel = new QLabel("Enter process size in B", this);
        processSizeInput = new QTextEdit(this);
        processSizeInput->setFixedHeight(30);
        processSizeInput->setFixedWidth(200);
        processSizeLayout->addWidget(processSizeLabel);
        processSizeLayout->addWidget(processSizeInput);
        processLayout->addLayout(processSizeLayout);

        // Create the second input field with label
        QHBoxLayout *processIdLayout = new QHBoxLayout();
        QLabel *processIdLabel = new QLabel("Enter process id", this);
        processIdInput = new QTextEdit(this);
        processIdInput->setFixedHeight(30);
        processIdInput->setFixedWidth(200);
        processIdLayout->addWidget(processIdLabel);
        processIdLayout->addWidget(processIdInput);
        processLayout->addLayout(processIdLayout);

        // Create buttons for adding and removing processes
        QPushButton *addButton = new QPushButton("Add Process", this);
        QPushButton *removeButton = new QPushButton("Remove Process", this);
        processLayout->addWidget(addButton);
        processLayout->addWidget(removeButton);

        processTab->setLayout(processLayout);
        tabWidget->addTab(processTab, "Process Management");

        // Second tab for read/write operations
        QWidget *ioTab = new QWidget(this);
        QVBoxLayout *ioLayout = new QVBoxLayout(ioTab);

        // Create the input field for process id
        QHBoxLayout *ioProcessIdLayout = new QHBoxLayout();
        QLabel *ioProcessIdLabel = new QLabel("Enter process id", this);
        ioProcessIdInput = new QTextEdit(this);
        ioProcessIdInput->setFixedHeight(30);
        ioProcessIdInput->setFixedWidth(200);
        ioProcessIdLayout->addWidget(ioProcessIdLabel);
        ioProcessIdLayout->addWidget(ioProcessIdInput);
        ioLayout->addLayout(ioProcessIdLayout);

        // Create the input field for data
        QHBoxLayout *ioDataLayout = new QHBoxLayout();
        QLabel *ioDataLabel = new QLabel("Enter data", this);
        ioDataInput = new QTextEdit(this);
        ioDataInput->setFixedHeight(30);
        ioDataInput->setFixedWidth(200);
        ioDataLayout->addWidget(ioDataLabel);
        ioDataLayout->addWidget(ioDataInput);
        ioLayout->addLayout(ioDataLayout);

        // Create buttons for read and write operations
        QPushButton *readButton = new QPushButton("Read", this);
        QPushButton *writeButton = new QPushButton("Write", this);
        ioLayout->addWidget(readButton);
        ioLayout->addWidget(writeButton);

        ioTab->setLayout(ioLayout);
        tabWidget->addTab(ioTab, "Read/Write");

        // Add the tab widget to the main layout
        mainLayout->addWidget(tabWidget);

        // Create the graphics view to draw rectangles
        graphicsView = new QGraphicsView(this);
        mainLayout->addWidget(graphicsView);

        scene = new QGraphicsScene(this);
        graphicsView->setScene(scene);

        setCentralWidget(centralWidget);

        // Draw the initial grid of rectangles
        drawGrid(PAGES_COUNT);

        // Connect buttons to their respective slots
        connect(addButton, &QPushButton::clicked, this, &MainWindow::addProcess);
        connect(removeButton, &QPushButton::clicked, this, &MainWindow::removeProcess);
        connect(readButton, &QPushButton::clicked, this, &MainWindow::readData);
        connect(writeButton, &QPushButton::clicked, this, &MainWindow::writeData);
    }

private slots:
    void addProcess() {
        bool sizeOk;
        int size = processSizeInput->toPlainText().toInt(&sizeOk);

        bool idOk;
        int id = processIdInput->toPlainText().toInt(&idOk);

        if (sizeOk && size > 0 && idOk && id >= 0) {
            Process process;
            process.id = id;
            process.content = std::string(size, '0');

            manager.allocate_memory(process);
            std::vector<int> processPages = manager.getPages()[id];

            int cellWidth = 50; // Width of each cell
            int remainingSize = size;

            for (int i : processPages) {
                QGraphicsRectItem *rect = rectangles.at(i);
                if (rect) {
                    QColor randomColor = QColor(id % 256, (id * 53) % 256, (id * 17) % 256);

                    if (remainingSize >= 4096) {
                        // Create a filled rectangle inside the main rectangle
                        QGraphicsRectItem *filledRect = new QGraphicsRectItem(0, 0, cellWidth, rect->rect().height(), rect);
                        filledRect->setBrush(randomColor);
                        remainingSize -= 4096;
                        additionalRectangles[i].push_back(filledRect);
                    } else {
                        // Create a partially filled rectangle
                        QGraphicsRectItem *filledRect = new QGraphicsRectItem(0, 0, remainingSize * cellWidth / 4096, rect->rect().height(), rect);
                        filledRect->setBrush(randomColor);
                        remainingSize = 0;
                        additionalRectangles[i].push_back(filledRect);
                    }
                }
            }

            QMessageBox::information(this, "Process Added", "Process added with size " + QString::number(size) + " B.");
        } else {
            QMessageBox::warning(this, "Input Error", "Please enter a valid process size.");
        }
    }

    void removeProcess() {
        bool idOk;
        int id = processIdInput->toPlainText().toInt(&idOk);

        if (idOk && id >= 0) {
            std::vector<int> processPages = manager.free_memory();

            for (int i : processPages) { 
            for (QGraphicsRectItem *additionalRect : additionalRectangles[i]) {
                scene->removeItem(additionalRect);
                delete additionalRect;
            }
            additionalRectangles[i].clear();
            }
            
            

            QMessageBox::information(this, "Process Removed", "Process removed.");
        } else {
            QMessageBox::warning(this, "Input Error", "Please enter a valid process id.");
        }
    }

    void readData() {
        // Implement reading data logic here
        QMessageBox::information(this, "Read Data", "Reading data...");
    }

    void writeData() {
        // Implement writing data logic here
        QMessageBox::information(this, "Write Data", "Writing data...");
    }

    void drawGrid(int number) {
        for (int i = 0; i < number; ++i) {
            QGraphicsRectItem *rect = new QGraphicsRectItem(0, 0, 50, 50);
            rect->setPos(i * 50, 0);
            rect->setBrush(Qt::NoBrush);
            scene->addItem(rect);
            rectangles.push_back(rect);
        }
    }

private:
    QTextEdit *processSizeInput;
    QTextEdit *processIdInput;
    QTextEdit *ioProcessIdInput;
    QTextEdit *ioDataInput;
    QGraphicsView *graphicsView;
    QGraphicsScene *scene;
    std::vector<QGraphicsRectItem*> rectangles; // Store the main rectangles
    std::unordered_map<int, std::vector<QGraphicsRectItem*>> additionalRectangles; // Store the additional rectangles by process id
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    MainWindow window;
    window.resize(800, 600);
    window.setWindowTitle("Memory Manager");
    window.show();

    return app.exec();
}

#include "main.moc"
